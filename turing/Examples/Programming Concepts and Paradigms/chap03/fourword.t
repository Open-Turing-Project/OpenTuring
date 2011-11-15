% File "fourword.t".
% Create 200 four-letter words by picking letters at random.
% Store them in the file "randword".
const alphabet := "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
var word : string
var which : int
var randStream : int
open : randStream, "randword", put
assert randStream > 0
for count : 1 .. 200
    word := "" % Null string.
    for letterIndex : 1 .. 4
        which := Rand.Int (1, 26)
        word := word + alphabet (which)
    end for
    put : randStream, word
end for
put "200 four-letter words stored in file 'randword'"

