% The "File.Copy" program.
const sourceFile : string := "%oot/Support/Help/Examples/Data Files/data.txt"
const destFile : string := "myfile.txt"

setscreen ("text")

% Copy file
File.Copy (sourceFile, destFile)
if Error.Last = eNoError then
    put "File copied", skip
else
    put "Did not copy the file."
    put "Error: ", Error.LastMsg, skip
end if

% Echo file to screen
var f : int
var line : string
open : f, destFile, get
loop
    exit when eof (f)
    get : f, line : *
    put line
end loop
close : f

% Delete the file
File.Delete (destFile)
if Error.Last = eNoError then
    put skip, "File deleted"
else
    put skip, "Did not delete the file."
    put "Error: ", Error.LastMsg
end if
