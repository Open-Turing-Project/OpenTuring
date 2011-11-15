% File "endword.t".
% Read a sequence of words and output the last letter
% of each word until the sentinel word is read.
var word : string
const sentinel := "stop"
put "Enter a sequence of words, end with ", sentinel
% Words must have at least one letter.
loop
    get word
    exit when word = sentinel
    put "Last letter of ", word, " is ", word (*)
end loop
put "That is the end of the sequence"
