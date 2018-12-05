const electron = require('electron');
const BeeInterface = require('./bee/BeeInterface');

const REFRESH_INTERVAL_MS = 1000;

let mainWindow;
let mainWindowReady = false;

let availableInterfaces = [];
let chosenInterface;
let beeInterface;

let hiveWindows = [];

// Refresh data from hardware periodically
setInterval(refreshHardware, REFRESH_INTERVAL_MS);

// Open main window
electron.app.on('ready', () => {
    mainWindow = new electron.BrowserWindow({
        icon: './assets/icons/win/beewatch.ico',
        height: 800,
        width: 1000
    });
    mainWindow.loadFile('pages/index.html');
    mainWindow.on('closed', () => { electron.app.quit() })
});

electron.ipcMain.on('main-window-ready', () => {
    mainWindowReady = true;
    updateWindows();
})


electron.ipcMain.on('actuator-order', (event, slaveId, desiredState) => {
    let promise;

    if (desiredState)
        promise = beeInterface.enableActuator(slaveId)
    else
        promise = beeInterface.disableActuator(slaveId)

    promise.then(() => {
        updateWindows();
    }).catch((error) => {
        // Inform windows that the request failed
        event.sender.send('actuator-failed', slaveId, desiredState, error);
    })
})

// Opening detailed hive
electron.ipcMain.on('open-hive', (event, slaveId) => {
    if (hiveWindows[slaveId]) {
        hiveWindows[slaveId].focus();
    }
    else {
        hiveWindows[slaveId] = new electron.BrowserWindow({
            icon: './assets/icons/win/beewatch.ico',
            title: `Ruche # ${slaveId}`,
        });
        hiveWindows[slaveId].loadFile('pages/hive.html');
        hiveWindows[slaveId].webContents.on('did-finish-load', () => { updateWindows(); })
        hiveWindows[slaveId].on('closed', () => { hiveWindows[slaveId] = null })
    }
})

// Dismissing alerts
electron.ipcMain.on('alerts-dismissed', (event, slaveId) => {
    // retransmit to main window
    mainWindow.webContents.send('ack-alert', slaveId)
})

// Start checking hardware
refreshHardware();

// Send windows updated data
function updateWindows() {
    mainWindow.webContents.send('slave-list', beeInterface ? beeInterface.getSlaves() : []);
    mainWindow.webContents.send('interface-list', availableInterfaces, chosenInterface);

    // Send data to every hive
    hiveWindows.forEach((hiveWindow, index) => {
        if (hiveWindow)
            hiveWindow.webContents.send('hive-data', index, beeInterface.getHistoric(index));
    })
}

function refreshHardware() {
    BeeInterface.listInterfaces().then(list => {
        availableInterfaces = list;

        // First connection
        if ((!chosenInterface) && list.length > 0) {
            chosenInterface = list[0];
            beeInterface = new BeeInterface(chosenInterface.comName);
            beeInterface.on('alert', (slaveId) => {
                mainWindow.webContents.send('alert', slaveId);
                updateWindows();
            });
            beeInterface.once('ready', () => {
                queryHardware();
            })
        }
    });

    if (beeInterface)
        queryHardware();
}

function queryHardware() {
    beeInterface.update().then(() => {
        if (mainWindowReady)
            updateWindows();
    }).catch((error) => {
        console.log("Failed to connect to master");
        if (chosenInterface.name.match(/arduino/i)) {
            console.log("Arduino reset delay ?");
        }
        if (mainWindowReady)
            updateWindows();
    });
}