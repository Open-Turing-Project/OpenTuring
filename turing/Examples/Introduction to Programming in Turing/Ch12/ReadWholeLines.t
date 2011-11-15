% The "ReadWholeLines" program
% Read and output a line at a time
put "Enter a series of lines of text, end with eof"
var line : string
loop
    exit when eof
    % Read a whole line
    get line : *
    put line
end loop
