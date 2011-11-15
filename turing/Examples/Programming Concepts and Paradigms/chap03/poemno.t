% File "poemno.t".
% Read the file named "poemÓ line by line and output
% the numbered lines to a file named "poem2Ó.
var line : string
var lineNumber := 0
var streamIn, streamOut : int
open : streamIn, "poem", get
assert streamIn > 0
open : streamOut, "poem2", put
assert streamOut > 0
loop
    exit when eof (streamIn)
    get : streamIn, line : * % Read entire line.
    lineNumber := lineNumber + 1
    put : streamOut, lineNumber : 3, " " : 3, line
end loop
put "Lines of poem have been numbered"
