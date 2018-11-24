const { Writable } = require('stream');
const settings = require('./settings');

// MEF states
const MEF_DEFAULT = 0;
const MEF_WAIT_ADDR = 1;
const MEF_WAIT_MCOM = 2;
const MEF_WAIT_RCOM = 3;

const MEF_LIST_LENGTH = 4;
const MEF_LISTING = 5;
const MEF_END_CHAR = 6;

const MEF_SLAVE_DATA = 7;

class StreamDecoder extends Writable {
    constructor(options) 
    {
        super(options);

        this._mefState = MEF_DEFAULT;

        this._currentMessage = {};
        this._expectedChars = 0;
        this._byteBuffer = new ArrayBuffer(10); // Stores the incoming bytes to be decoded
        this._bufferView = new DataView(this._byteBuffer);
    }

    _write(chunk, encoding, callback)
    {
        for (const byte of chunk)
        {
            this._processByte(byte);
        }

        callback();
    }

    _processByte(nextChar)
    {
        switch(this._mefState)
        {
            case MEF_DEFAULT:
                if (nextChar == settings.prot.START_BYTE)
                {
                    this._mefState = MEF_WAIT_ADDR;
                    this._currentMessage = {};
                }
                break;

            case MEF_WAIT_ADDR:
                this._currentMessage.sourceId = nextChar;

                // If the address is the master, only expect list responses
                if (nextChar == settings.prot.MASTER_ADDR)
                    this._mefState = MEF_WAIT_MCOM;
                else
                    this._mefState = MEF_WAIT_RCOM;
                break;

            // Master steps
            case MEF_WAIT_MCOM:
                // Checking for list command
                if (nextChar == settings.prot.LIST_COMMAND)
                {
                    this._mefState = MEF_LIST_LENGTH;
                    this._currentMessage.command = settings.prot.LIST_COMMAND;
                }
                else
                    this._mefState = MEF_DEFAULT;   // Resetting if incorrect
                break;

            case MEF_LIST_LENGTH:
                this._mefState = MEF_LISTING;
                this._expectedChars = nextChar;
                this._currentMessage.length = nextChar; // Saving the list length
                this._currentMessage.list = [];
                break;

            case MEF_LISTING:
                --this._expectedChars;
                this._currentMessage.list.push(nextChar);
                if(this._expectedChars <= 0)
                    this._mefState = MEF_END_CHAR;
                break;

            // Slave steps
            case MEF_WAIT_RCOM:
                // Check if the incoming command is valid
                switch (nextChar)
                {
                    case settings.prot.SENSORS_COMMAND:
                    case settings.prot.ACTUATOR_OFF_COMMAND:
                    case settings.prot.ACTUATOR_ON_COMMAND:
                    case settings.prot.ALERT_COMMAND:
                    case settings.prot.SENSORS_RESPONSE:
                        this._currentMessage.command = nextChar;
                        this._expectedChars = 10;
                        this._mefState = MEF_SLAVE_DATA;
                        break;
                    default:    // Invaling command
                        this._mefState = MEF_DEFAULT;
                        break;
                }
                break;

            case MEF_SLAVE_DATA:
                this._bufferView.setUint8(
                    this._byteBuffer.byteLength - this._expectedChars,
                    nextChar);
                --this._expectedChars;
                if(this._expectedChars <= 0)
                {
                    // Decoding data
                    this._currentMessage.temperature = this._bufferView.getInt8(0);
                    this._currentMessage.movement = this._bufferView.getUint8(1) != 0;
                    
                    // Small endian decoding coming from micro-controller
                    this._currentMessage.longitude = this._bufferView.getFloat32(2, true);
                    this._currentMessage.latitude = this._bufferView.getFloat32(6, true);

                    // Waiting last char
                    this._mefState = MEF_END_CHAR;
                }
                break;

            // Waiting for the closing char
            case MEF_END_CHAR:
                if (nextChar == settings.prot.END_BYTE)
                {
                    // Sends the command back to the listeners
                    this.emit('command', this._currentMessage);
                }
                this._mefState = MEF_DEFAULT;
                break;
        }
    }
}

module.exports = StreamDecoder;