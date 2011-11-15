% The "Purge" program
% Find and output all four-letter words in script
% Assumes no punctuation marks in text
% Assign a stream number to the input file script
var script : int
open : script, "Script", get
assert script > 0
% Assign a stream number to the output file censor
var censor : int
open : censor, "Censor", put
assert censor > 0
var word : string
loop
    get : script, skip
    exit when eof (script)
    get : script, word
    if length (word) = 4 then
	put : censor, word
    end if
end loop
% Close censor file and open for reading
close : censor
open : censor, "Censor", get
assert censor > 0
% Read and output censor file to the window
loop
    get : censor, skip
    exit when eof (censor)
    get : censor, word
    put word
end loop
