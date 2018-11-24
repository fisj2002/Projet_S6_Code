module.exports = 
{
    DEFAULT_BAUD_RATE : 9600,
    DEVICE_FILE_PATH: './knownDevices.json',

    SLAVE_REQUEST_INTERVAL_MS: 2000,
    REQUEST_TIMEOUT_MS: 1000,

    // Protocol settings
    prot:{
        MASTER_ADDR: 0xFF,

        START_BYTE: 'S'.charCodeAt(0),
        END_BYTE: 'E'.charCodeAt(0),

        LIST_COMMAND: 'L'.charCodeAt(0),
        SENSORS_COMMAND: 'F'.charCodeAt(0),
        SENSORS_RESPONSE: 'R'.charCodeAt(0),
        ACTUATOR_ON_COMMAND: 'A'.charCodeAt(0),
        ACTUATOR_OFF_COMMAND: 'D'.charCodeAt(0),
        ALERT_COMMAND: 'N'.charCodeAt(0),
    }
}