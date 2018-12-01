const electron = require('electron');

electron.ipcRenderer.on('hive-data', (event, slaveId, history)=>{
    console.log('hey')
})