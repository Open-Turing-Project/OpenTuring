% The "HaveAnS" program
% Test to see if a word contains an "s"
var word : string
put "Enter a series of words, end with 'last' "
loop
    put "Enter word: " ..
    get word
    exit when word = "last"
    if index (word, "s") not= 0 then
	put word, " contains an 's' "
    else
	put word, " does not contain an 's' "
    end if
end loop
