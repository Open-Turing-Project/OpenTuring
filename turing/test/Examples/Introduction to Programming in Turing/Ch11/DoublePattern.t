% The "DoublePattern" program
% Test to see if a word has two occurrences of a pattern
var word, pattern : string
put "Enter the pattern you want to test for: " ..
get pattern
const size := length (pattern)
put "Enter a series of words"
put "Enter 'finis' to stop"
loop
    get word
    exit when word = "finis"
    const place := index (word, pattern)
    if place = 0 then
	put word, " contains no occurrences of ", pattern
    elsif index (word (place + size .. *), pattern) = 0 then
	put word, " contains one occurrence of ", pattern
    else
	put word, " contains at least two occurrences of ", pattern
    end if
end loop
