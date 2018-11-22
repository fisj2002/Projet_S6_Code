const SerialPort = require('serialport');
const EventEmitter = require('events');
const StreamDecoder = require('./streamDecoder');
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

        this._port = new SerialPort(comName, 
            {baudRate: settings.DEFAULT_BAUD_RATE});

        // The array containing the current active requests
        this.outboundRequests = [];

        this._streamDecoder = this._port.pipe(new StreamDecoder());
        this._streamDecoder.on('command',this._fulfillRequests);
    }

    /**
     * Analyze the incoming transmissions and tries to match them to
     * the requests that were issued
     * @param {Object} message The object describing the response
     */
    _fulfillRequests(message)
    {
        console.log(message);
    }

    /**
     * Queries the interface to check for changes and acquire a new sample
     */
    async update()
    {
        // Send list request
        this._port.write(Buffer.from([
            settings.prot.START_BYTE,
            settings.prot.MASTER_ADDR,
            settings.prot.LIST_COMMAND,
            settings.prot.END_BYTE,
        ]));
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

class Hive
{

}

module.exports = BeeInterface;