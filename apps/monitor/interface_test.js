const BeeInterface = require('./bee/BeeInterface');
const electron = require('electron');


let beeInterface;


BeeInterface.listInterfaces().then((list)=>{
    console.log(list)

    beeInterface = new BeeInterface(list[0].comName)

    beeInterface.update().then(()=>{electron.app.quit()}).catch((error)=>{
        console.log(error)
        electron.app.quit()
    });


    // beeInterface.update().then(()=>{
    //     console.log(beeInterface.getSlaves())
    // }).catch((error)=>{
    //     console.log(error)
    // });
}).catch((error)=>{
    console.log(error)
    electron.app.quit()});