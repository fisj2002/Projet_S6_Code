const electron = require('electron');

let mainWindow;

electron.app.on('ready', ()=>{
    mainWindow = new electron.BrowserWindow({
        icon: './assets/icons/win/bee.ico'
    });
    mainWindow.loadFile('html/hive.html');
    mainWindow.on('closed', ()=>{electron.app.quit()})
});