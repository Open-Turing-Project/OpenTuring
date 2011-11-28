% The "eof" program.
var line : string
var fileNumber : int

% Echo file
setscreen ("text")
open : fileNumber, "%oot/Support/Help/Examples/Data Files/data.txt", get
loop
    exit when eof (fileNumber)
    get : fileNumber, line : *
    put line
end loop
