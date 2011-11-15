% The "StringExample1" program.
var name : string
var size : int
put "Enter any name: " ..
get name : *    
% The user enters a name. The :* allows the user to include spaces.
size := length (name)
% Length is a reserved word that automatically calculates the number of
% characters in the actual name entered. This number is stored under
% the variable size.
for counter : 1 .. size
    put name (counter)
    % By placing brackets after the variable name, each character in the
    % name that was entered can be output one at a time.
end for
