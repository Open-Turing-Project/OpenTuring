% The "Str.Trim" program.
var line : string
loop
    put "Enter a non-empty line of text: " ..
    get line : *
    exit when Str.Trim (line) not= ""
end loop
put "You entered ", line
