const { Writable } = require('stream');

class StreamDecoder extends Writable {
    constructor(options) 
    {
        super(options);
    }

    _write(chunk, encoding, callback)
    {
        

        callback();
    }
}

module.exports = StreamDecoder;