% The "PigLatin" program
% Reads a text and translates to Pig Latin
% Assumes no capital letters and no punctuation
var fileName : string
put "Enter name of file where English is stored"
get fileName
var inFile : int
open : inFile, fileName, get
assert inFile > 0
put "Enter name of file where Pig Latin is to be stored"
get fileName
var outFile : int
open : outFile, fileName, put
assert outFile > 0
var word : string
loop
    get : inFile, skip
    exit when eof (inFile)
    get : inFile, word
    if index ("aeiou", word (1)) = 0 then
	put : outFile, word (2 .. *) + word (1) + "ay "
    else
	put : outFile, word + "way "
    end if
end loop
