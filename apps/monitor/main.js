const electron = require('electron');
const settings = require('./settings');
const BeeInterface = require('./BeeInterface');

var interface;

BeeInterface.listInterfaces().then(list =>{
    console.log(list);

    console.log('Choosing the first interface');
    interface =  new BeeInterface(list[0].comName);

}).then(()=> {
    setInterval(()=>{interface.update();}, settings.SLAVE_REQUEST_TIMEOUT_MS);
});


//              electron.app.quit();