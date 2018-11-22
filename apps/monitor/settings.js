module.exports = 
{
    DEFAULT_BAUD_RATE : 9600,
    DEVICE_FILE_PATH: './knownDevices.json',

    SLAVE_REQUEST_INTERVAL_MS: 2000,
    SLAVE_REQUEST_TIMEOUT_MS: 1000,

    // Protocol settings
    prot:{
        MASTER_ADDR: 0xFF,

        START_BYTE: 'S'.charCodeAt(0),
        END_BYTE: 'E'.charCodeAt(0),

        LIST_COMMAND: 'L'.charCodeAt(0),
    }
}