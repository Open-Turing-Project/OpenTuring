% The "RemovePunctuation" program
% Eliminates punctuation marks from text
% Assumes no double quotes or parentheses
var text : int
open : text, "Text", get
assert text > 0
var expurge : int
open : expurge, "Expurge", put
assert expurge > 0
var line, output : string
loop
    exit when eof (text)
    get : text, line : *
    output := ""
    for place : 1 .. length (line)
	if index (",.:;'?!", line (place)) = 0 then
	    output := output + line (place)
	end if
    end for
    put : expurge, output
end loop
