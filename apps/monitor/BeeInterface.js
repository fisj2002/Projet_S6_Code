const SerialPort = require('serialport');
const EventEmitter = require('events');

class BeeInterface extends EventEmitter
{

    /**
     * Ouvre une interface vers un appareil beewatch
     * @param {String} comName The name of the interface to open (ex: COM3, /tty3)
     */
    constructor(comName)
    {

    }

    /**
     * Queries the interface to check for changes and acquire a new sample
     */
    async update()
    {

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

    }
}

module.exports = BeeInterface;