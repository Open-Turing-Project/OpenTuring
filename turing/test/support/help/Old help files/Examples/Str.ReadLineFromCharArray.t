% The "Str.ReadLineFromCharArray" program.
var f, size : int
var c : char (1000000)

% Read the file into a char array all at one (this is very fast)
open : f, "%oot/support/help/Examples/Str.ReadLineFromCharArray.t", read, seek
seek : f, *
tell : f, size
seek : f, 0
read : f, c : size
close : f

% Read the individual lines from the char array.  Lines that are over
% 255 characters will be read in multiple lines.
var pos : int := 1
var line : string
loop
    exit when pos > size
    put pos : 5, " " ..
    Str.ReadLineFromCharArray (c, size, line, pos)
    put line
end loop
put pos : 5
