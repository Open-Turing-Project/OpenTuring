% The "FinalThreeLetters" program
% Read words and if possible give last three characters
var word : string
put "Enter words one to a line, end with 'quit' "
loop
    put "Enter word: " ..
    get word
    exit when word = "quit"
    if length (word) >= 3 then
	put word (* - 2 .. *)
    else
	put "Word has fewer than 3 characters"
    end if
end loop
