% The "Error.Halt" program.
% This program asks you to enter numbers until you enter an
% odd number, in which case the program halts with an error message.
setscreen ("text")
loop
    var i : int
    put "Enter an even integer: " ..
    get i
    if i mod 2 = 1 then
	Error.Halt ("Odd input is not allowed")
    end if
end loop
