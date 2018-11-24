const electron = require('electron');
const settings = require('./settings');
const BeeInterface = require('./BeeInterface');

var interface;

BeeInterface.listInterfaces().then(list =>{
    if(list[0])
    {
        console.log('Choosing the first interface');
        interface =  new BeeInterface(list[0].comName);
    }
    else
        throw 'No interface found';

}).then(()=> {
    setInterval(()=>{
        interface.update().then(()=>{
            console.log(interface.getSlaves());
        });
    }, settings.SLAVE_REQUEST_INTERVAL_MS);
});


//              electron.app.quit();