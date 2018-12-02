const SerialPort = require('serialport');
const EventEmitter = require('events');
const StreamDecoder = require('./streamDecoder');
const Hive = require('./hive');
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
        
        // Emit an event to signify the interface is ready to communicate
        this._port.once('open', ()=>{
            this.emit('ready');
        })

        // The array containing the current active requests
        this._outboundRequests = [];

        // The array containing the hives
        this._hives = [];

        // Stream decoder interprets the incoming bytes
        this._streamDecoder = this._port.pipe(new StreamDecoder());
        this._streamDecoder.on('command', (message)=>{
            this._fulfillRequests(message);
        });
    }

    /**
     * Analyze the incoming transmissions and tries to match them to
     * the requests that were issued
     * @param {Object} message The object describing the response
     */
    _fulfillRequests(message)
    {
        if (message.command == settings.prot.LIST_COMMAND &&
            message.sourceId == settings.prot.MASTER_ADDR)
        {
            // Make sure every element of the list is accounted for
            message.list.forEach((hiveId)=>{
                if (!(this._hives[hiveId]))
                    this._hives[hiveId] = new Hive(hiveId);
            });

            // Notify all the disconnected hives
            this._hives.forEach((hive, index)=>{
                if (hive)
                {
                    if (!message.list.find((element)=> element == index))
                        hive.addConnectionLost();
                }
            });
        }
        else    // Slave response
        {
            let dissconnect = false

            if(message.command == settings.prot.SENSORS_RESPONSE)
                message.command = settings.prot.SENSORS_COMMAND;
            else if(message.command == settings.prot.DISCONNECTED_SIGNAL){
                message.command = settings.prot.SENSORS_COMMAND;
                dissconnect = true;
            }

            if (! this._hives[message.sourceId])
                this._hives[message.sourceId] = new Hive(message.sourceId);

            if(dissconnect)
                this._hives[message.sourceId].addConnectionLost();
            else
                this._hives[message.sourceId].addEvent(message);
        }

        // Call the callback of the first matching request
        var match = this._outboundRequests.find((request)=>
            request.sourceId == message.sourceId && request.command == message.command
        );
        if(match)
            match.resolve();

        // Raise an event if the message is an alert
        if (message.command == settings.prot.ALERT_COMMAND)
            this.emit('alert', message.sourceId);
    }

    /**
     * Queries the interface to check for changes and acquire a new sample
     */
    async update()
    {
        // update slave list
        await this._uartRequest(settings.prot.MASTER_ADDR, settings.prot.LIST_COMMAND);

        var promiseList = [];
        // Sensors request on every slave
        this._hives.forEach((hive)=>{
            promiseList.push(new Promise((resolve,reject)=>{
                this._uartRequest(hive.id, settings.prot.SENSORS_COMMAND).then(resolve).catch(resolve);
            }));
        });

        await Promise.all(promiseList);
    }

    /**
     * Sends request and waits of the response to resolve
     * @param {Number} destinationID The Id of the target
     * @param {Number} commandCode Code of the command
     */
    async _uartRequest(destinationID, commandCode)
    {
        var responsePromise = new Promise((resolve,reject)=>{
            // Add to pending requests
            this._outboundRequests.push({
                sourceId: destinationID,
                command: commandCode,
                resolve: resolve    // The call back for a successful functions
            });

            // Setting timeout
            setTimeout(()=>{
                reject('Request timeout')
            }, settings.REQUEST_TIMEOUT_MS);
        });

        // Send the request to the master
        this._port.write(Buffer.from([
            settings.prot.START_BYTE,
            destinationID,
            commandCode,
            settings.prot.END_BYTE,
        ]));

        var err = null;
        try
        {
            await responsePromise;
        }
        catch(error)
        {
            // Add disconnection event to slave
            if (destinationID != settings.prot.MASTER_ADDR)
            {
                if (this._hives[destinationID] && this._hives[destinationID].connection)
                    this._hives[destinationID].addConnectionLost();
            }

            err = error;
        }

        // Remove request from list
        var firstMatchIndex = this._outboundRequests.findIndex((request)=>{
            return request.sourceId == destinationID &&
                request.command == commandCode;
        });
        this._outboundRequests.splice(firstMatchIndex, 1);

        if(err)
            throw err;
    }

    /**
     * @returns The list of slaves under this master interface
     */
    getSlaves()
    {
        var slaves = [];

        this._hives.forEach((hive)=>{
            slaves.push(hive.getResume());
        });

        return slaves;
    }

    /**
     * Access the sensor history of a specific slave
     * @param {Number} Id The number of the desired slave
     * @returns The slave structure with historic data
     */
    getHistoric(Id)
    {
        return this._hives[Id].log;
    }

    /**
     * Enables the actuator of the specified slave
     * @param {Number} Id The number of the desired slave
     */
    async enableActuator(Id)
    {
        await this._uartRequest(Id, settings.prot.ACTUATOR_ON_COMMAND);
    }

    /**
     * Disables the actuator of the specified slave
     * @param {Number} Id The number of the desired slave
     */
    async disableActuator(Id)
    {
        await this._uartRequest(Id, settings.prot.ACTUATOR_OFF_COMMAND);
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