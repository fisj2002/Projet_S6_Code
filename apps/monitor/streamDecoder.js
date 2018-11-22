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

class StreamDecoder extends Writable {
    constructor(options) 
    {
        super(options);

        this._mefState = MEF_DEFAULT;

        this._currentMessage = {};
        this._expectedChars = 0;
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