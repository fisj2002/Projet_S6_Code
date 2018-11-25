const electron = require('electron');
const BeeInterface = require('./bee/BeeInterface');

let mainWindow;

electron.app.on('ready', ()=>{
    mainWindow = new electron.BrowserWindow({
        icon: './assets/icons/win/bee.ico'
    });
    mainWindow.loadFile('index.html');
    mainWindow.on('closed', ()=>{electron.app.quit()})
});