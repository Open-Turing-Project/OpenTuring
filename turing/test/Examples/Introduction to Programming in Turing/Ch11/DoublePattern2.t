% The "DoublePattern2" program.
var pos, count, size : int
count := 0
var word, pattern : string
put "Enter the pattern that you want to search for: " ..
get pattern
put "Enter the word you want to search through: " ..
get word
size := length (pattern)
loop
    pos := index (word, pattern)
    exit when pos = 0
    count := count + 1
    word := word (pos + size .. *)
end loop
put "The number of occurrences: ", count
