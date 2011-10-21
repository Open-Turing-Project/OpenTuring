% The "MotorReverse" program.
var key : string (1)
% Outputs voltage on D0, activating the motor in one direction.
parallelput (1)
% Waits until a key is pressed.
put "Press any key to stop the motor."
getch (key)
% Stops the motor when a key is pressed.
parallelput (0)
% Waits again.
put "Press any key to start the motor in reverse."
getch (key)
% Outputs voltage on D1.
parallelput (2)
% Waits.
put "Press any key to stop the motor."
getch (key)
% Stops the motor when a key is pressed.
parallelput (0)
