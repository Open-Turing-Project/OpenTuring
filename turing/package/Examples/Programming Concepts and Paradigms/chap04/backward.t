% File "backward.t".
% Read a sequence of words and reverse the order of their letters.
% Stop when the word "finis" entered.
const sentinel := "finis"
var word : string
put "Enter a sequence of words, end with ", sentinel
loop
    get word
    exit when word = sentinel
    % This next is a counted loop, counting backwards,
    % nested inside the conditional loop.
    for decreasing letter : length (word) .. 1
        put word (letter) ..
    end for
    put "" % Output null string to end this line.
end loop
put "That's all folks"

