% The "WordCount" program
% Counts the number of words in a text
var count : int := 0
var word : string
loop
    get skip
    exit when eof
    get word
    count := count + 1
end loop
put "There are ", count, " words in the text"
