% The "BlinkLED" program.
% The for structure repeats the program ten times.
for count : 1 .. 10
    % Turns the LED on.
    parallelput (1)
    % Keeps the LED on for 500 milliseconds.
    delay (500)
    % Turns the LED off.
    parallelput (0)
    % Keeps the LED off for 300 milliseconds.
    delay (300)
    % Ends once the program has repeated ten times.
end for
