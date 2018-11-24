const electron = require('electron');
const BeeInterface = require('./bee/BeeInterface');

var interface;

BeeInterface.listInterfaces().then(list =>{
    if(list[0])
    {
        console.log('Choosing the first interface');
        interface =  new BeeInterface(list[0].comName);
    }
    else
        throw 'No interface found';

    interface.on('alert', (slaveId)=>{
        console.log('Alerte sur la ruche %d', slaveId);
    });

    setTimeout(()=>{
        console.log('Sending request')
        interface.update().then(()=>{
            var slaves = interface.getSlaves();
            interface.enableActuator(slaves[0].id);

            setTimeout(()=>{interface.enableActuator(slaves[1].id);}, 1000);
            setTimeout(()=>{interface.disableActuator(slaves[1].id);}, 2000);

            setTimeout(()=>{interface.disableActuator(slaves[0].id);}, 15000);
        });
    }, 2000);
});


//              electron.app.quit();