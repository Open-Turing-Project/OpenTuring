% The "File.Delete" program.
const pathName : string := "myfile.txt"

% Create file
var f : int
var line : string
open : f, pathName, put
put : f, "There is a line here"
close : f
put pathName, " created"

% Delete the file
File.Delete (pathName)
if Error.Last = eNoError then
    put skip, pathName, " deleted"
else
    put skip, "Did not delete ", pathName
    put "Error: ", Error.LastMsg
end if
