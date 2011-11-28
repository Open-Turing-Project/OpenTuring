% The "array4" program.
function getLines (fileName : string) : int
    var f, numLines : int
    var line : string
    open : f, fileName, get
    numLines := 0
    loop
	exit when eof (f)
	get : f, line : *
	numLines += 1
    end loop
    close : f
    result numLines
end getLines

procedure readFile (var lines : array 1 .. * of string, fileName : string)
    var f : int
    var line : string
    open : f, fileName, get
    for i : 1 .. upper (lines)
	get : f, lines (i) : *
    end for
    close : f
end readFile

setscreen ("text")
const pathName : string := "%oot/support/Help/Examples/Data Files/data.txt"
var lines : flexible array 1 .. 0 of string
var numberOfLines : int := getLines (pathName)
new lines, numberOfLines
readFile (lines, pathName)
for i : 1 .. upper (lines)
    put lines (i)
end for
