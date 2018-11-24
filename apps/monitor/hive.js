const settings = require('./settings');


class Hive
{
    /**
     * Builds the hive object
     * @param {Numer} id Identification of the hive
     */
    constructor(id)
    {
        this.id = id;
        this.connection = true;
        this.actuatorEnabled = false;
        this.log = [
            {event: 'Hive discovery', time: Date.now()}
        ];
    }

    /**
     * Add the incoming transmission to the event log
     * @param {Object} message Adds an event to the hive log
     */
    addEvent(message)
    {
        var eventType = '';

        switch (message.command)
        {
            case settings.prot.SENSORS_COMMAND:
                eventType = 'Sensors reading';
                break;
            case settings.prot.ACTUATOR_OFF_COMMAND:
                eventType = 'Actuator disbled';
                this.actuatorEnabled = false;
                break;
            case settings.prot.ACTUATOR_ON_COMMAND:
                eventType = 'Actuator enabled';
                this.actuatorEnabled = true;
                break;
            case settings.prot.ALERT_COMMAND:
                eventType = 'Alert raised';
                break;
            default:
                throw `Invalid operation: ${message.command}`;
        }

        this.connection = true;

        this.log.push({
            event: eventType,
            time: Date.now(),
            temperature: message.temperature,
            movement: message.movement,
            longitude: message.longitude,
            latitude: message.latitude
        });
    }

    /**
     * Inserts a connection loss to the hive
     */
    addConnectionLost()
    {
        if (this.connection == true)
        {
            this.log.push({event: 'connection lost', time: Date.now()});
            this.connection = false;
        }
    }

    /**
     * Create a shorter version of the object with only the most recent of data
     * @returns Concise object with most recent data
     */
    getResume()
    {
        var lastDataLog = {};

        for(var i = this.log.length-1; i >= 0; --i)
        {
            // Find the first entry with usable data
            if (this.log[i].temperature != undefined)
            {
                lastDataLog = this.log[i];
                break;
            }
        }

        return {
            id: this.id,
            connectionActive: this.connection,
            actuatorEnabled: this.actuatorEnabled,
            
            temperature: lastDataLog.temperature,
            longitude: lastDataLog.longitude,
            latitude: lastDataLog.latitude,
            movement: lastDataLog.movement,

            lastSampleTime: lastDataLog.time,
        };
    }
}

module.exports = Hive;