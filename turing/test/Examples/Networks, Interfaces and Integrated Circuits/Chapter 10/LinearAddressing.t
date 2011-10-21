% The "LinearAddressing" program.
% Centering titles.
colour (red)
locate (5, 30)
put " LINEAR ADDRESSING"

colour (black)
locate (11, 20)
put "The number of the LED being illuminated is.."
% counter triggers each of the thirty-two lights.
for counter : 0 .. 31
    colour (green)
    locate (13, 40)
    % counter must be +1 since the first LED is activated with parallelput(0).
    put counter + 1

    parallelput (counter)

    delay (500)
end for
put "Program Terminated"
