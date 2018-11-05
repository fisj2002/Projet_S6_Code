const SerialPort = require('serialport');
const electron = require('electron');


// Assuming windows platform
serialport = new SerialPort('COM3',{baudRate : 9600});
parser = new SerialPort.parsers.Readline();

serialport.pipe(parser);
parser.on('data', console.log);

serialport.write('Hello World\r\n');