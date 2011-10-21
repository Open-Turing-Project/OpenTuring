% The "ListAnyFile" program
% Reads a file and outputs its lines numbered
var fileName : string
var streamNumber : int
put "Enter name of file: " ..
get fileName
open : streamNumber, fileName, get
% Test that the open procedure has been successful
assert streamNumber > 0
var lineNumber := 0
var line : string
loop
    exit when eof (streamNumber)
    get : streamNumber, line : *
    lineNumber := lineNumber + 1
    put lineNumber : 3, " ", line
end loop
