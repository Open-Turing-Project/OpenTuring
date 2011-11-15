% The "LetterAtATime" program
% Read a word and output it a letter-at-a-time
var word : string
put "Enter words, end with 'tired' "
loop
    put "Enter word: " ..
    get word
    exit when word = "tired"
    for i : 1 .. length (word)
	put word (i)
    end for
end loop
put "Why not take a rest?"
