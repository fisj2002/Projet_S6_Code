const BeeInterface = require('./bee/BeeInterface');
const electron = require('electron');


let beeInterface;


BeeInterface.listInterfaces().then((list)=>{
    console.log(list)

    beeInterface = new BeeInterface(list[0].comName)

    beeInterface.update();


    // beeInterface.update().then(()=>{
    //     console.log(beeInterface.getSlaves())
    // }).catch((error)=>{
    //     console.log(error)
    // });
})