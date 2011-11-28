% The "Str.Lower" program.
var line : string
loop
    put "Enter a line of text (empty to quit): " ..
    get line : *
    exit when line = ""
    put "The lower case version: ", Str.Lower (line)
end loop
