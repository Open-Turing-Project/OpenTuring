% The "WordsOnLine" program
% Reads 5 words and outputs all on a line
var word : string
var line := ""    % Initialize line to the null string
put "Enter 5 words"
for i : 1 .. 5
    get word
    line := line + word + " "
end for
put line
