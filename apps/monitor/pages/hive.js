const electron = require('electron');

var hiveID;
var appliedLog = 0;
var connectionAcvtive = true;
var actuatorEnabled = false;

var lastValidSensorsEvent;
var lastEvent;

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

electron.ipcRenderer.on('hive-data', (event, slaveId, history) => {

    // Processing all new packets
    for (; appliedLog < history.length; ++appliedLog) {
        switch (history[appliedLog].event) {
            case 'Hive discovery':
                document.getElementById('hiveID').innerHTML = slaveId;
                hiveID = slaveId;
                break;
            case 'Actuator disabled':
                actuatorEnabled = false;
                lastValidSensorsEvent = history[appliedLog];
                break;
        }


        lastEvent = history[appliedLog];
    }
})