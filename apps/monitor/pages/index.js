const electron = require('electron');
const fs = require('fs');

var card;
var loader;

// Loading loader
fs.readFile('pages/loader.html', { encoding: 'utf8' }, (err, file) => {
    document.getElementById('map-tab').innerHTML = file;
    document.getElementById('mosaic-tab').innerHTML = file;
    loader = file;
})

// Loading cards
fs.readFile('pages/cards.html', { encoding: 'utf8' }, (err, content) => {
    card = content;
})


// Wait for data reception
electron.ipcRenderer.on('interface-list-response', (availableInterfaces, chosenInterface) => {

});

electron.ipcRenderer.on('slave-list-response', (event, slaveList) => {
    if (slaveList && slaveList.length > 0) {
        cardsHtlml = "";
        slaveList.forEach((slave) => { cardsHtlml += card; })
        document.getElementById('mosaic-tab').innerHTML = cardsHtlml;

        // Updating the values of every element
        cards = document.getElementsByClassName('hive-card');

        for(var index = 0; index <= cards.length; ++index){
            cards.item(index).getElementsByClassName('card-title').item(0).innerHTML =
                `Ruche # ${slaveList[index].id}`;

            
        }
    }
    else if (document.getElementsByClassName('hive-card').length > 0)
        document.getElementById('mosaic-tab').innerHTML = loader;
});

setInterval(() => {
    electron.ipcRenderer.send('slave-list-request');
    electron.ipcRenderer.send('interface-list-request');
}, 2000);