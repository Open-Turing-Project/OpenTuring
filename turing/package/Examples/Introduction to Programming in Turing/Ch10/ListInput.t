% The "ListInput" program
% Read text and output a line at a time
% numbering each line
var lineNumber := 0
var line : string
loop
    exit when eof
    get line : *       % Read an entire line
    lineNumber := lineNumber + 1
    put lineNumber : 4, " ", line
end loop
