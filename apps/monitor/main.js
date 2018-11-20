const SerialPort = require('serialport');
const electron = require('electron');
const fs = require('fs');

SerialPort.list().then(list =>{
    console.log(list);

    fs.writeFileSync('data2.json',JSON.stringify(list, null, 4));

    electron.app.quit();
})



// // Assuming windows platform
// serialport = new SerialPort('COM3',{baudRate : 9600});
// parser = new SerialPort.parsers.Readline();

// serialport.pipe(parser);
// parser.on('data', console.log);

// serialport.write('Hello World\r\n');