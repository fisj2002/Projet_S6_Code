const electron = require('electron');
const BeeInterface = require('./bee/BeeInterface');

const REFRESH_INTERVAL_MS = 1000;

let mainWindow;
let mainWindowReady = false;

let availableInterfaces = [];
let chosenInterface;
let beeInterface;

let hives = [];

// Refresh data from hardware periodically
setInterval(refreshHardware, REFRESH_INTERVAL_MS);

// Open main window
electron.app.on('ready', () => {
    mainWindow = new electron.BrowserWindow({
        icon: './assets/icons/win/bee.ico',
        height: 800,
        width: 1000
    });
    mainWindow.loadFile('pages/index.html');
    mainWindow.on('closed', () => { electron.app.quit() })
});

electron.ipcMain.on('main-window-ready', () => {
    mainWindowReady = true;
    updateMain();
})


electron.ipcMain.on('actuator-order', (event, slaveId, state) => {
    let promise;
  
    if (state)
        promise = beeInterface.enableActuator(slaveId)
    else
        promise = beeInterface.disableActuator(slaveId)
    
    promise.then(()=>{
        hives = beeInterface.getSlaves();
        updateMain();
    })
})

// Start checking hardware
refreshHardware();

// Send main window updated data
function updateMain() {
    mainWindow.webContents.send('slave-list', hives);
    mainWindow.webContents.send('interface-list', availableInterfaces, chosenInterface);
}

function refreshHardware() {
    BeeInterface.listInterfaces().then(list => {
        availableInterfaces = list;

        // First connection
        if ((!chosenInterface) && list.length > 0) {
            chosenInterface = list[0];
            beeInterface = new BeeInterface(chosenInterface.comName);
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
        hives = beeInterface.getSlaves();
        if (mainWindowReady)
            updateMain();
    }).catch((error) => {
        console.log("Failed to connect to master");
        if (chosenInterface.name.match(/arduino/i)) {
            console.log("Arduino reset delay ?");
        }
        hives = beeInterface.getSlaves();
        if (mainWindowReady)
            updateMain();
    });
}