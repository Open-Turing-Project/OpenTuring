% The "Count4LEDs" program.
% Count from 1 to 15 in binary.
for count : 0 .. 15
    % Outputs count to the LEDs.
    parallelput (count)
    % Outputs count to the screen.
    put count
    delay (500)
end for
