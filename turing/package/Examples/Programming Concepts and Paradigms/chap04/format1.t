% File "format1.t".
% Reformat text in file "text" in ragged-right style so that the
% number of  characters in a line is limited to a given maximum.
% Store the revised text in file "ragged".
var text, ragged : int
open : text, "text", get
assert text > 0
open : ragged, "ragged", put
assert ragged > 0
put "Enter maximum length of lines"
var maxLength : int
get maxLength
var word : string
var lineLength : int := 0
loop
    exit when eof (text)
    get : text, word
    if lineLength + length (word) <= maxLength then
        put : ragged, word + " " ..
        lineLength := lineLength + length (word) + 1
    else
        put : ragged, " " % Start new line.
        put : ragged, word + " " ..
        lineLength := length (word) + 1
    end if
end loop
put : ragged, " " % End the last line.
