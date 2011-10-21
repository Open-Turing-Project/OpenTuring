% The "SetLEDs" program.
% The base 10 value of 30 is 00011110 in binary.
% The computer converts 30 to this value automatically before the
% number is passed out the parallel port.
parallelput (30)
% Holds the display for half a second.
delay (500)
% Shuts the LEDs off by outputting all zeros.
parallelput (0)
