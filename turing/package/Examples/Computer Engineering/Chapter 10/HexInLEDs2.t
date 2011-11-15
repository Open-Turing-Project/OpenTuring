% The "HexInLEDs2" program.
% The variable that the user will enter must be declared as a
% string because A, B, C, D, E, F are not recognized as numbers.
var number : string
% The user is prompted for the base 16 number.
put "Enter the base 16 digit: " ..
get number
if "0" <= number and number <= "9" then
    parallelput (ord (number) - ord ("0"))
else
    parallelput (ord (number) - ord ("A") + 10)
end if
% Holds the number for two seconds on the LEDs.
delay (2000)
% Shuts the LEDs off.
parallelput (0)
