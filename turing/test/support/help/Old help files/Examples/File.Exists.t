% The "File.Exists" program.
const pathName : string := "myfile.txt"
if File.Exists (pathName) then
    put pathName, " exists! Program ending"
    return
else
    put pathName, " does not exist"
end if

put "Creating ", pathName
var f : int
open : f, pathName, put
put : f, "Hello, World!"
close : f

if File.Exists (pathName) then
    put pathName, " exists."
else
    put pathName, " does not exist!"
end if

put "Deleting ", pathName
File.Delete (pathName)

if File.Exists (pathName) then
    put pathName, " still exists!"
else
    put pathName, " does not exist"
end if

