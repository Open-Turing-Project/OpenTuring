% The "TimesTable" program
% Outputs a multiplication table
var number : int
put "Choose a number between 1 and 12 " ..
get number
put "Here is the multiplication table for ", number
for i : 1 .. 12
    put i : 2, " times ", number, " is ", i * number : 2
end for
