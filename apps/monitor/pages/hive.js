const electron = require('electron');
const Graph = require('./graph')
const settings = require('../bee/settings')
const roundTo = require('round-to')

var hiveId;
var appliedLog = 0;
var actuatorEnabled = false;
var lastValidSensorsEventIndex;

// Initialize charts
var tempChart = new Graph(document.getElementById('temp-chart'), 'analog', 'Température (°C)');
var connexionChart = new Graph(document.getElementById('connexion-chart'), 'digital', 'Connecté (I/O)');
var actuatorChart = new Graph(document.getElementById('actuator-chart'), 'digital', 'État (Activé/Désactivé');
var movementChart = new Graph(document.getElementById('movement-chart'), 'digital', 'État (Activé/Désactivé');

// Setting up collapsable menu arrows
document.addEventListener('DOMContentLoaded', function () {
    var elems = document.querySelectorAll('.collapsible');
    M.Collapsible.init(elems, {
        accordion: false,
        onOpenStart: function (element) {
            element.getElementsByClassName('flipper').item(0).innerHTML = 'arrow_drop_up';
        },
        onCloseStart: function (element) {
            element.getElementsByClassName('flipper').item(0).innerHTML = 'arrow_drop_down';
        }
    });
});

// Dismiss alerts
document.getElementById('alert-element').onclick = () => {
    document.getElementById('quick-alert').classList.remove('new');
    electron.ipcRenderer.send('alerts-dismissed', hiveId);
}

// Activate actuator
let actuatorElement = document.getElementById('actuator-switch');
actuatorElement.onclick = () => {
    actuatorElement.disabled = true;
    electron.ipcRenderer.send('actuator-order', hiveId, actuatorElement.checked)
}

// Failed actuator command
electron.ipcRenderer.on('actuator-failed', (event, slaveId, desiredState, error) => {
    let checkbox = document.getElementById('actuator-switch');
    M.toast({ html: `Failed to ${desiredState ? 'enable' : 'disable'} actuator on hive # ${slaveId}: ${error}` })

    checkbox.checked = !desiredState;
    checkbox.enabled = true;
})

electron.ipcRenderer.on('hive-data', (event, slaveId, history) => {

    // Processing all new packets
    for (; appliedLog < history.length; ++appliedLog) {
        switch (history[appliedLog].event) {
            case settings.NEW_HIVE_EVENT:
                connexionChart.addPoint(history[appliedLog].time, true);
                document.getElementById('hiveID').innerHTML = slaveId;
                hiveId = slaveId;
                break;
            case settings.ACTUATOR_OFF_EVENT:
                actuatorEnabled = false;
                document.getElementById('actuator-switch').checked = false;
                document.getElementById('actuator-switch').disabled = false;
                lastValidSensorsEventIndex = appliedLog;
                break;
            case settings.ACTUATOR_ON_EVENT:
                actuatorEnabled = true;
                document.getElementById('actuator-switch').checked = true;
                document.getElementById('actuator-switch').disabled = false;
                lastValidSensorsEventIndex = appliedLog;
                console.log('hey')
                break;
            case settings.ALERT_EVENT:
                let alertList = document.getElementById('alert-list');
                let tableRow = document.createElement('tr');
                let timeColumn = document.createElement('td');
                timeColumn.innerHTML = new Date(history[appliedLog].time).toLocaleString()
                let messageColumn = document.createElement('td');
                let messsage = document.createElement('pre');
                messsage.innerHTML = JSON.stringify(history[appliedLog], undefined, 4);
                messageColumn.appendChild(messsage);
                tableRow.appendChild(timeColumn)
                tableRow.appendChild(messageColumn)
                alertList.insertBefore(tableRow, alertList.childNodes[0]);
                document.getElementById('quick-alert').classList.add('new');
                lastValidSensorsEventIndex = appliedLog;
                break;
            case settings.SENSOR_EVENT:
                lastValidSensorsEventIndex = appliedLog;
                break;
            case settings.CONNECTION_LOST_EVENT:
                connexionChart.addPoint(history[appliedLog].time, false);
                document.getElementById('quick-connexion').innerHTML = 'signal_wifi_off';
                break;
        }

        if (history[appliedLog].event != settings.CONNECTION_LOST_EVENT) {
            connexionChart.addPoint(history[appliedLog].time, true);
            document.getElementById('quick-connexion').innerHTML = 'signal_wifi_4_bar';
        }

        // Updating sensor graphs
        actuatorChart.addPoint(history[appliedLog].time, actuatorEnabled)
        if (history[appliedLog].temperature)
            tempChart.addPoint(history[appliedLog].time, history[appliedLog].temperature);
        if (history[appliedLog].movement)
            movementChart.addPoint(history[appliedLog].time, history[appliedLog].movement);

        // Adding event to logs
        let log = document.getElementById('message-list');
        let tableRow = document.createElement('tr');
        let timeColumn = document.createElement('td');
        timeColumn.innerHTML = new Date(history[appliedLog].time).toLocaleString()
        let messageColumn = document.createElement('td');
        let messsage = document.createElement('pre');
        messsage.innerHTML = JSON.stringify(history[appliedLog], undefined, 4);
        messageColumn.appendChild(messsage);
        tableRow.appendChild(timeColumn)
        tableRow.appendChild(messageColumn)
        log.insertBefore(tableRow, log.childNodes[0]);
    }

    // Setting last values
    if (lastValidSensorsEventIndex) {
        document.getElementById('quick-temp').innerHTML = history[lastValidSensorsEventIndex].temperature;

        let latitude = history[lastValidSensorsEventIndex].latitude;
        let longitude = history[lastValidSensorsEventIndex].longitude;
        document.getElementById('quick-location').innerHTML =
            `${roundTo(Math.abs(latitude), 6)}°${latitude >= 0 ? 'N' : 'S'} \
            ${roundTo(Math.abs(longitude), 6)}°${longitude >= 0 ? 'E' : 'W'}`;

        if (history[lastValidSensorsEventIndex].movement) {
            document.getElementById('quick-movement').innerHTML = 'directions_walk'
            document.getElementById('quick-movement').classList.add('red-text');
        }
        else {
            document.getElementById('quick-movement').innerHTML = 'visibility_off'
            document.getElementById('quick-movement').classList.remove('red-text');
        }
    }

    // Update timer
    document.getElementById('sample-time').innerHTML = new Date(history[appliedLog - 1].time).toLocaleString()

    // Updating charts
    connexionChart.update();
    tempChart.update();
    actuatorChart.update();
    movementChart.update();
})