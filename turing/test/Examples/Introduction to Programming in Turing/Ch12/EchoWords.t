% The "EchoWords" program
% Read a series of words, end with stop
var word : string
put "Enter a line of text, end with stop"
loop
    get word
    exit when word = "stop"
    put word
end loop
