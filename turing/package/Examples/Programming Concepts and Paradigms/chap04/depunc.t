% File "depunc.t".
% Eliminate all punctuation marks
% from file "text" and store result in file "text2".
% It is assumed that there are no double quotes or parentheses.
const punctuation := ",.:;?!"
var inFile, outFile : int
open : inFile, "text", get
assert inFile > 0
open : outFile, "text2", put
assert outFile > 0
var inLine, outLine : string
loop
    exit when eof (inFile)
    get : inFile, inLine : * % Read line from 'text'
    outLine := "" % Initialize to null string.
    for place : 1 .. length (inLine)
        % See if this character is a punctuation character.
        if index (punctuation, inLine (place)) = 0 then
            outLine := outLine + inLine (place)
        end if
    end for
    put : outFile, outLine % Output line to 'text2'
end loop
put "File text without punctuation now in file 'text2' "

