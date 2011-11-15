% The "HexInLEDs" program.
% The variable that the user will enter must be declared as a
% string because A, B, C, D, E, F are not recognized as numbers.
var number : string
% The user is prompted for the base 16 number.
put "Enter the base 16 digit: " ..
get number
if number = "0" then
    parallelput (0)
elsif number = "1" then
    parallelput (1)
elsif number = "2" then
    parallelput (2)
elsif number = "3" then
    parallelput (3)
elsif number = "4" then
    parallelput (4)
elsif number = "5" then
    parallelput (5)
elsif number = "6" then
    parallelput (6)
elsif number = "7" then
    parallelput (7)
elsif number = "8" then
    parallelput (8)
elsif number = "9" then
    parallelput (9)
elsif number = "A" then
    parallelput (10)
elsif number = "B" then
    parallelput (11)
elsif number = "C" then
    parallelput (12)
elsif number = "D" then
    parallelput (13)
elsif number = "E" then
    parallelput (14)
elsif number = "F" then
    parallelput (15)
end if
% Holds the number for two seconds on the LEDs.
delay (2000)
% Shuts the LEDs off.
parallelput (0)
