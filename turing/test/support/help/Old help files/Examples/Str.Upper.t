% The "Str.Upper" program.
var line : string
loop
    put "Enter a line of text (empty to quit): " ..
    get line : *
    exit when line = ""
    put "The upper case version: ", Str.Upper (line)
end loop
