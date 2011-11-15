% The "WordLengths" program
% Read words and find their length
put "Enter one word to line, end with 'end' "
var word : string
loop
    put "Enter word: " ..
    get word
    exit when word = "end"
    put word, " has ", length (word), " letters"
end loop
