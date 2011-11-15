% The "ReadWords" program
% Read words until ten have been read
% or the word "stop" is read
var word : string
put "Enter ten words one to a line, or finish with 'stop' "
put ""
for count : 1 .. 10
    get word
    exit when word = "stop"
    put word
end for
