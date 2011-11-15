% The "CountWithLEDs" program.
% The for loop counts from 0 to 255.
for count : 0 .. 255
    % The present value of count is passed to the LEDs in binary form.
    parallelput (count)
    % Displays the base 10 value of count on the screen.
    put count
    % The delay function stops the computer from going
    % on to the next command.  The length of the delay is measured
    % in milliseconds and appears just after it in the brackets.
    delay (500)
end for
