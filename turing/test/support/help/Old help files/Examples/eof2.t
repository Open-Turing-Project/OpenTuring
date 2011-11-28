% The "eof2" program
var numLines : int := 1
var line : string
put "Enter one word per line, ending with a Ctrl+D or Ctrl+Z on a line"
loop
    put numLines : 3, ": " ..
    exit when eof
    get line
    numLines += 1
end loop
put "You entered ", numLines, " words"
