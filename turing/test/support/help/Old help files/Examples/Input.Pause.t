% The "Input.Pause" program.
var f : int
var line : string
put "Press any key to see the next line in the file.", skip
open : f, "%oot/support/help/Examples/data files/data.txt", get
loop
    exit when eof (f)
    get : f, line : *
    put line
    Input.Pause
end loop
close : f
put skip, "Finished"
