var f, size : int
var c : char (1000000)
open : f, "TuringTelnet.t", read, seek
seek : f, *
tell : f, size
seek : f, 0
read : f, c : size
close : f
var pos : int := 1
var line : string
loop
    exit when pos > size
    Str.ReadLineFromCharArray (c, size, line, pos)
    put pos : 5, " ", line
end loop

