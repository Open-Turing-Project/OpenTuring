% The "ReverseNames" program
% Reads a list of names and outputs in reverse
var howMany : int
put "How many names are there? " ..
get howMany
var name : array 1 .. howMany of string (20)
put "Enter ", howMany, " names, one to a line"
for i : 1 .. howMany
    get name (i) : *
end for
put "Here are the names in reverse order"
for decreasing i : howMany .. 1
    put name (i)
end for
