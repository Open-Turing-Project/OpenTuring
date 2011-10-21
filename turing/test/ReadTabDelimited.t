% The "ReadTabDelimited" program


function ReadField (var line : int) : string
    var res : string
    var tabPosition := index (line, "\t")
    if tabPosition = 0 then
	res = line
	line := ""
    else
	res = line (1 .. tabPosition - 1)
	line := line (tabPosition + 1 .. *)
    end if
    result res
end ReadField

var f : int
var line : string

open : f, "file.dat", get
loop
    exit when eof
    get line : *
    for i : 1 .. 5
	put i, ": ", ReadField (line)
    end for
end loop
close : f
