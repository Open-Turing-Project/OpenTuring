% The "MotorOnOff" program.
var key, answer : string (1)
% Repeats program until n is entered.
loop
    % The motor is turned on.
    parallelput (1)
    % Clears the screen.
    cls
    put "Hit any key to turn the motor off."
    % The getch input structure does not wait for return to be pressed.
    getch (key)
    % The motor is turned off.
    parallelput (0)
    put "Continue (y/n)? " ..
    getch (answer)
    exit when answer = "n"
end loop
