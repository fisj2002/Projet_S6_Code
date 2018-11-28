const electron = require('electron');
const BeeInterface = require('./bee/BeeInterface');

const REFRESH_INTERVAL_MS = 1500;

let mainWindow;
let availableInterfaces = [];
let chosenInterface;
let beeInterface;

let hives;

// Refresh data from hardware periodically
setInterval(()=>{
    BeeInterface.listInterfaces().then(list =>{
        availableInterfaces = list;

        if((!chosenInterface) && list.length > 0) 
        {
            chosenInterface = list[0];
            beeInterface = new BeeInterface(chosenInterface.comName);
        }
    });

    if(beeInterface)
    {
        beeInterface.update().then(()=>{
            hives = beeInterface.getSlaves();
        }).catch((error)=>{
            if (chosenInterface.name.match(/arduino/i))
            {   
                console.log("Arduino reset delay");
            }
            else
                throw error;
        });
    }

}, REFRESH_INTERVAL_MS);


electron.app.on('ready', ()=>{
    mainWindow = new electron.BrowserWindow({
        icon: './assets/icons/win/bee.ico'
    });
    mainWindow.loadFile('pages/index.html');
    mainWindow.on('closed', ()=>{electron.app.quit()})
});