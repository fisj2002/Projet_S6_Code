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
        this.log = [
            {event: 'creation', time: Date.now()}
        ];
    }

    /**
     * Add the incoming transmission to the event log
     * @param {Object} message Adds an event to the hive log
     */
    addEvent(message)
    {

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

    }
}

module.exports = Hive;