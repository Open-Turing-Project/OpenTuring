/*
** Stream module
**
** NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
**      IT WILL BE IMPLICITLY IMPORTED.
*/

unit
module Stream
    export /* Flush, */ FlushAll

%    external "stream_flush" procedure Flush (fileId: int)

    external "stream_flushall" procedure FlushAll

end Stream
