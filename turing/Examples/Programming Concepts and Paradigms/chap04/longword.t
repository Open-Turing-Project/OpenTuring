% File "longword.t".
% Read a file and output all words longer than a given maximum length.
var word, fileName : string
var streamIn, maxLength : int
var count := 0
put "Count long words in a file"
put "What is name of file to be read? "
get fileName
open : streamIn, fileName, get
assert streamIn > 0
put "What is maximum length of word? "
get maxLength
loop
    exit when eof (streamIn)
    get : streamIn, word
    if length (word) > maxLength then
        count += 1
        put word
    end if
end loop
put "There were ", count, " words longer than ", maxLength, " characters"
