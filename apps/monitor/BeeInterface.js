const SerialPort = require('serialport');
const EventEmitter = require('events');
const settings = require('./settings');
const fs = require('fs');

class BeeInterface extends EventEmitter
{
    /**
     * Ouvre une interface vers un appareil beewatch
     * @param {String} comName The name of the interface to open (ex: COM3, /tty3)
     */
    constructor(comName)
    {
        super();

        this.port = new SerialPort(comName, 
            {baudRate: settings.DEFAULT_BAUD_RATE});

        // The array containing the current active requests
        this.outboundRequests = [];

        this.port.on('data', data =>{
            console.log(data);
        });
    }

    /**
     * Queries the interface to check for changes and acquire a new sample
     */
    async update()
    {
        // Send list request
        this.port.write(Buffer.from([0x53, 0xFF, 0x4C, 0x45]));
    }

    /**
     * @returns The list of slaves under this master interface
     */
    getSlaves()
    {

    }

    /**
     * Access the sensor history of a specific slave
     * @param {Number} Id The number of the desired slave
     * @returns The slave structure with historic data
     */
    getHistoric(Id)
    {

    }

    /**
     * Enables the actuator of the specified slave
     * @param {Number} Id The number of the desired slave
     */
    enableActuator(Id)
    {

    }

    /**
     * Disables the actuator of the specified slave
     * @param {Number} Id The number of the desired slave
     */
    disableActuator(Id)
    {

    }

    /**
     * Lists the available interfaces part of the known interfaces.
     * @returns The list
     */
    static async listInterfaces()
    {
        var p_knownDevices = new Promise((resolve, reject)=>{
            fs.readFile(settings.DEVICE_FILE_PATH, (err, devices)=>{
                if(err)
                    reject(err);
                else 
                    resolve(JSON.parse(devices));
            });
        });

        var p_portList = SerialPort.list();

        var [portList, knownDevices] = await Promise.all([p_portList, p_knownDevices]);

        var curatedList = [];

        portList.forEach(port => {
            var referenceDevice = knownDevices.find(
                device => device.vendorId == port.vendorId 
                && device.productId == port.productId);

            if(referenceDevice)
                curatedList.push({name: referenceDevice.displayName, comName: port.comName});
        });

        return curatedList;
    }
}

module.exports = BeeInterface;