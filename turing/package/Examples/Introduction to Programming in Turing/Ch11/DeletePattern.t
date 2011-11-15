% The "DeletePattern" program.
var pos, size : int
var word, pattern : string
put "Enter the pattern that you want to search for: " ..
get pattern
put "Enter the word you want to search through: " ..
get word
size := length (pattern)
loop
    pos := index (word, pattern)
    exit when pos = 0
    word := word (1 .. pos - 1) + word (pos + size .. *)
end loop
put "Here is the word with the pattern removed: ", word
