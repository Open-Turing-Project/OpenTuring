% The "File.Rename" program.
const sourceFile : string := "data.txt"
const destFile : string := "myfile.txt"

setscreen ("text")

% Create file
var f : int
var line : string
open : f, sourceFile, put
put : f, "Hello, World!"
close : f
put sourceFile, " created"

% Copy file
File.Rename (sourceFile, destFile)
if Error.Last = eNoError then
    put sourceFile, " renamed to ", destFile
else
    put "Did not rename ", sourceFile
    put "Error: ", Error.LastMsg
end if

% Indicate which file exists
if File.Exists (sourceFile) then
    put sourceFile, " exists."
else
    put sourceFile, " does not exist."
end if
if File.Exists (destFile) then
    put destFile, " exists."
else
    put destFile, " does not exist."
end if

% Delete the file
File.Delete (destFile)
if Error.Last = eNoError then
    put destFile, " deleted"
else
    put "Did not delete ", destFile
    put "Error: ", Error.LastMsg
end if
