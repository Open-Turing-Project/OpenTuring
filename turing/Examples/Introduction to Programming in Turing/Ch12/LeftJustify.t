% The "LeftJustify" program
% Reads file text and left-justifies it
var text : int
open : text, "Text", get
put "Enter length of output lines: " ..
var maxLength : int
get maxLength
var word : string
var lineLength : int := 0
loop
    get : text, skip
    exit when eof (text)
    get : text, word
    if lineLength + length (word) < maxLength then
	put " ", word ..
	lineLength := lineLength + length (word) + 1
    else
	put ""            % Start new line
	put word ..
	lineLength := length (word)
    end if
end loop
put ""   % End last line
