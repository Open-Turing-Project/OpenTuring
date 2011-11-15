% The "RemoveVowels" program
% Eliminates the vowels from a word
var word : string
put "Enter a series of words, end with '*' "
const vowels := "aeiou"
loop
    get word
    exit when word = "*"
    var newWord := ""        % empty string
    for i : 1 .. length (word)
	if index (vowels, word (i)) = 0 then
	    % Letter is not a vowel
	    newWord := newWord + word (i)
	end if
    end for
    put "Word without vowels ", newWord
end loop
