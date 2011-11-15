% The "DisplayASCII" program.

% Displays the passed-in value as bits on the screen and sets
% the pins on the parallel port to the same value.
procedure DisplayBinary (asciiValue : int)
    var radius, x, y : int
    var bit : int
    var number : int := asciiValue

    % Draws the outline of the box containing the lights.
    drawbox (120, 220, maxx - 120, 260, black)

    % Sets size and position of the right-most circle.
    radius := 10
    x := maxx - 145
    y := 240

    % Outputs the ASCII value of the character to the parallel port.
    parallelput (number)

    % Calculates the 8-bit binary value of the ASCII value.
    % We do this from the lowest value to the highest (i.e. right to left).
    for i : 1 .. 8
	% Determines the lowest bit in the number using the "mod" operator.
	bit := number mod 2
	if bit = 1 then
	    drawfilloval (x, y, radius, radius, red)
	    drawoval (x, y, radius, radius, black)
	else
	    drawfilloval (x, y, radius, radius, white)
	    drawoval (x, y, radius, radius, black)
	end if
	locatexy (x - 5, 280)
	put bit ..
	x := x - 50
	% Divides by two to find the next higher bit.
	number := number div 2
    end for
end DisplayBinary

% Main program
setscreen ("graphics")
var name : string
var ch : string (1)
var asciiValue : int

locatexy (200, 400)
put "ASCII to BINARY CONVERTER"

% Gets the name to be converted.
put "Enter a name: " ..
get name

% Displays the key press message and sets the screen so there is no
% cursor and the keystroke does not show on the screen.
setscreen ("nocursor,noecho")
locate (maxrow, 20)
put "Press any key to view the next character." ..

% The for loop counts through each character of name.
% For each character, the loop displays a message on the screen and
% sends the ascii value of the character to the DisplayAscii procedure.
for counter : 1 .. length (name)
    % Gets the character to be displayed.
    ch := name (counter)
    % Obtains the ascii value of the character using the "ord" function.
    asciiValue := ord (ch)

    locatexy (150, 310)
    put "The ASCII value of ", ch, " is ", asciiValue, " and in binary is:"

    % Calls procedure to display ASCII values in binary.
    DisplayBinary (asciiValue)

    % Waits until the user presses a key.
    getch (ch)
end for
