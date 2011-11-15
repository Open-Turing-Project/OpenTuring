% The "CharValues" program
% See what characters are available
var value : int
loop
    put "Enter a character value, 1 to 255 inclusive " ..
    get value
    put "Character whose value is ", value, " is ", chr (value)
end loop
