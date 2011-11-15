% The "Obey" program
% Read in a series of words
% until the word "stop" is read
var word : string
put "Enter a series of words, one to a line"
put "If you want to stop say so"
loop
    get word
    exit when word = "stop"
end loop
put "This is the end"
