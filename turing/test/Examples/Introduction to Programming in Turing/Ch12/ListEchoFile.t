% The "ListEchoFile" program
% Reads lines of text from file "Echo" and numbers them
var streamNumber : int
% Open the file "Echo" for reading using "get"
open : streamNumber, "Echo", get
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
