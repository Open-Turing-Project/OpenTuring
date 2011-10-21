% The "Demultiplexer" program.

% The LED selected to be activated.
var selection : int

loop
    % Displays title and instructions.
    colour (red)
    locate (1, 30)
    put "DEMULTIPLEXER"
    colour (black)
    locate (3, 15)
    put "Enter the number of the LED to be activated - 0 to 15."
    locate (5, 25)
    put "Enter any other number to exit: " ..

    % Gets the input.
    get selection
    % Exits loop when selection out of range.
    exit when selection < 0 or selection > 15

    % Clears the screen for more output.
    cls
    locate (10, 27)
    put "Light number ", selection, " is activated."

    % Sets the pins on the parallel port.
    parallelput (selection)
end loop

% Turns off all the LEDs by setting the pins to 0 and terminates the program.
parallelput (0)
locate (maxrow - 2, 30)
put "All lights are deactivated."
