% The "char" program.
var c : char
var counter := 0
put "Enter text ending with a period"
loop
    exit when eof
    get c                   % Read a single character
    exit when c = '.'       % Single quotes for char constant
    counter := counter + 1
end loop
put counter, " characters before the period"
