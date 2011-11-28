% The "File.Parent" program.
var pathName : string
put "Enter a path: " ..
get pathName
loop
    pathName := File.Parent (pathName)
    exit when Error.Last not= eNoError
    put pathName
end loop
