% The "Intersection" program.

% The output pin configurations for the two stop lights are as follows
% (the number in brackets is the parallelput value for the pin).
%
%               Left light     Right light
% Red            D2 (4)          D3 (8)
% Yellow      D1 (2)          D4 (16)
% Green        D0 (1)          D5 (32)
%
% The following parallelput values were used to signal the appropriate lights.
%
% Value     Left light     Right light
% 4 + 32     Red On            Green On
% 4 + 16     Red On           Yellow On
% 1 + 8       Green On        Red On
% 2 + 8       Yellow On      Red On

% The locations of the centres of each light.
const leftGreenX := 200
const leftGreenY := 150
const leftYellowX := 200
const leftYellowY := 210
const leftRedX := 200
const leftRedY := 270
const rightGreenX := 440
const rightGreenY := 150
const rightYellowX := 440
const rightYellowY := 210
const rightRedX := 440
const rightRedY := 270

% This is the radius of the lights.
const size := 20

% The following colour number is a dark yellow.
const darkYellow := 93

procedure DrawLights
    locatexy (250, 390)
    put "Intersection"

    % Left stop light.
    drawfillbox (150, 110, 250, 310, gray)
    drawbox (150, 110, 250, 310, black)
    drawfilloval (leftGreenX, leftGreenY, size, size, green)
    drawfilloval (leftYellowX, leftYellowY, size, size, 93)
    drawfilloval (leftRedX, leftRedY, size, size, red)
    % Right stop light.
    drawfillbox (390, 110, 490, 310, gray)
    drawbox (390, 110, 490, 310, black)
    drawfilloval (rightGreenX, rightGreenY, size, size, green)
    drawfilloval (rightYellowX, rightYellowY, size, size, 93)
    drawfilloval (rightRedX, rightRedY, size, size, red)
end DrawLights

procedure RunLights
    % Sets left stop light to red.
    drawfilloval (leftGreenX, leftGreenY, size, size, green)
    drawfilloval (leftYellowX, leftYellowY, size, size, darkYellow)
    drawfilloval (leftRedX, leftRedY, size, size, brightred)
    % Sets right stop light to green.
    drawfilloval (rightGreenX, rightGreenY, size, size, brightgreen)
    drawfilloval (rightYellowX, rightYellowY, size, size, darkYellow)
    drawfilloval (rightRedX, rightRedY, size, size, red)
    parallelput (4 + 32)
    delay (4000)

    % Left stop light does not change, so is not redrawn.
    % Sets right stop light to yellow.
    drawfilloval (rightGreenX, rightGreenY, size, size, green)
    drawfilloval (rightYellowX, rightYellowY, size, size, yellow)
    drawfilloval (rightRedX, rightRedY, size, size, red)
    parallelput (4 + 16)
    delay (4000)

    % Sets left stop light to green.
    drawfilloval (leftGreenX, leftGreenY, size, size, brightgreen)
    drawfilloval (leftYellowX, leftYellowY, size, size, darkYellow)
    drawfilloval (leftRedX, leftRedY, size, size, red)
    % Sets right stop light to red.
    drawfilloval (rightGreenX, rightGreenY, size, size, green)
    drawfilloval (rightYellowX, rightYellowY, size, size, darkYellow)
    drawfilloval (rightRedX, rightRedY, size, size, brightred)
    parallelput (1 + 8)
    delay (4000)

    % Sets left stop light to yellow.
    drawfilloval (leftGreenX, leftGreenY, size, size, green)
    drawfilloval (leftYellowX, leftYellowY, size, size, yellow)
    drawfilloval (leftRedX, leftRedY, size, size, red)
    % Right stop light does not change and so is not redrawn.
    parallelput (2 + 8)
    delay (4000)
end RunLights

% Main program
setscreen ("graphics,nocursor")
var ch : string (1)

% Draws initial stop lights in off position.
DrawLights
locatexy (200, 40)
put "Press any key to activate lights"

% Waits for a keystroke to start the light sequence.
getch (ch)

% Clears screen, draws the lights, and starts light sequence.
cls
DrawLights
locatexy (200, 340)
put "Intersection Lights Activated"
locatexy (240, 40)
put "Press any key to exit"

% Continuously runs lights until a key is pressed.
loop
    RunLights
    exit when hasch
end loop

% On the start of the next cycle the lights are shut off. The program ends.
locatexy (200, 340)
put "Intersection Lights De-Activated"
DrawLights
locatexy (240, 37)
put "The program has terminated."
parallelput (0)
