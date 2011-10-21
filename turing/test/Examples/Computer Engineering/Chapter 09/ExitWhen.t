% The "ExitWhen" program.
% This program runs until the user enters the number 10.
var number : int
loop
    put "Please enter a number."
    get number
    exit when number = 10
end loop
put "You just entered number 10."
