var HC = (function() {
    var expectedCommands = {};
    var connection;
    var nextSeqID = 0;

    var connect = function( port ) {
        alert("Connecting to gui on port " + port);

        nextSeqID = 0;
        connection = new WebSocket('ws://localhost:' + port, 'command');

        connection.onopen = function () {
            alert("Connected!");
        }
        connection.onerror = function (e) {
            alert("WebSockets error :" + e);
        }
        connection.onmessage = function (e) {
            var data = JSON.parse(e.data);
            replyhandler( data );
        }

    };

    var replyhandler = function ( data ) {
        if (data.type !== 'response') { return; }

        // if the msg matches one of our outstanding IDs, handle it and remove.
        var cb = expectedCommands[data.seqID];
        if (cb !== undefined) {
            cb(data.args); 
            delete expectedCommands[data.seqID];
        }
    };

    var issue = function( target, cmd, args, onReply) {
        //alert("Issuing command " + cmd + " to " + target);

        // if a reply is expected, we need to remember that.
        expectedCommands[ nextSeqID ] = onReply;

        // wrap the command into our line format.
        var command = JSON.stringify( {
            type:"command",
            target : target,
            command : cmd,
            args : args,
            seqID: nextSeqID
        });
        connection.send( command );

        // advance our sequence number.
        nextSeqID++;
    };

    return {
        connect : connect,
        issue: issue
    };
})();