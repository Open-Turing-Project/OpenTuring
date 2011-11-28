% The "ord" program.
var str : string := "Hello"

% Output str with the letters spaced out 
put "  "..
for i : 1 .. length (str)
    put str (i) : 4 ..
end for
put ""

% Output the 'ord' values of each letter underneath the letter 
for i : 1 .. length (str)
    put ord (str (i)) : 4 ..
end for
put ""
