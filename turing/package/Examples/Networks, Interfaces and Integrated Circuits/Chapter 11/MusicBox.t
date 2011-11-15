% The "MusicBox" Program
%
% Here are the input and output values for a specific computer. 
% These change from machine to machine.
%
%  letter         button             LED
%  (musical)     (input)           (output)
%    "C"             111                8
%    "D"              95                1
%    "E"              63                4
%    "G"             255                2
%
% This message was included so that if this program is altered
% in the future, the new programmer has input and output
% values for the LED and buttons. If you change the input and
% output values be sure to change this comment.

const w1 := 200
const w2 := 650
const w3 := 100

% These constants represent the parameters to parallelput that
% light the LED for a particular note.
const C_LED := 8
const D_LED := 1
const E_LED := 4
const G_LED := 2
const ALL_LED_DARK := 0
const ALL_LED_LIT := 255

% These constants represent the value returned from
% parallelget for each of the buttons.
const C_BUTTON := 111
const D_BUTTON := 95
const E_BUTTON := 63
const G_BUTTON := 255

% These are the lower-left and upper-right corners of each of
% the four buttons.
const BTN_X1 := 130
const BTN_Y1 := 130
const BTN_X2 := 200
const BTN_Y2 := 200

const BTN2_X1 := 230
const BTN2_Y1 := 130
const BTN2_X2 := 300
const BTN2_Y2 := 200

const BTN3_X1 := 330
const BTN3_Y1 := 130
const BTN3_X2 := 400
const BTN3_Y2 := 200

const BTN4_X1 := 430
const BTN4_Y1 := 130
const BTN4_X2 := 500
const BTN4_Y2 := 200

% Draws the four  boxes corresponding to the notes.
procedure DrawButtons
    drawbox (BTN_X1, BTN_Y1, BTN_X2, BTN_Y2,
	brightmagenta)
    drawbox (BTN2_X1, BTN2_Y1, BTN2_X2, BTN2_Y2, brightcyan)
    drawbox (BTN3_X1, BTN3_Y1, BTN3_X2, BTN3_Y2, brightred)
    drawbox (BTN4_X1, BTN4_Y1, BTN4_X2, BTN4_Y2, brightblue)
    drawfill (BTN_X1 + 1, BTN_Y1 + 1, brightmagenta, 0)
    locate (20, 21)
    colour (brightmagenta)
    put "C" ..
    locate (20, 34)
    colour (brightcyan)
    put "D" ..
    locate (20, 47)
    colour (brightred)
    put "E" ..
    locate (20, 60)
    colour (brightblue)
    put "G" ..
end DrawButtons

% Lights the LED and the on-screen button corresponding
% to the C note. Plays the note.
procedure PlayCNote
    parallelput (C_LED)
    drawfill (BTN_X1 + 1, BTN_Y1 + 1, brightmagenta,
	brightmagenta)
    play ("8c")
    delay (w3)
    drawfillbox (BTN_X1, BTN_Y1, BTN_X2, BTN_Y2, 0)
    DrawButtons
end PlayCNote

% Lights the LED and the on-screen button corresponding
% to the D note. Plays the note.
procedure PlayDNote
    parallelput (D_LED)
    drawfill (BTN2_X1 + 1, BTN2_Y1 + 1, brightcyan, brightcyan)
    play ("8d")
    delay (w3)
    drawfillbox (BTN2_X1, BTN2_Y1, BTN2_X2, BTN2_Y2, 0)
    DrawButtons
end PlayDNote

% Lights the LED and the on-screen button corresponding
% to the E note. Plays the note.
procedure PlayENote
    parallelput (E_LED)
    drawfill (BTN3_X1 + 1, BTN3_Y1 + 1, brightred, brightred)
    play ("8e")
    delay (w3)
    drawfillbox (BTN3_X1, BTN3_Y1, BTN3_X2, BTN3_Y2, 0)
    DrawButtons
end PlayENote

% Lights the LED and the on-screen button corresponding
% to the G note. Plays the note.
procedure PlayGNote
    parallelput (G_LED)
    play ("8g")
    drawfill (BTN4_X1 + 1, BTN4_Y1 + 1, brightblue, brightblue)
    delay (w3)
    drawfillbox (BTN4_X1, BTN4_Y1, BTN4_X2, BTN4_Y2, 0)
    DrawButtons
end PlayGNote

% Blinks the LEDs the specified number of times.
procedure BlinkLEDs (numberOfBlinks : int)
    for i : 1 .. numberOfBlinks
	parallelput (ALL_LED_LIT)                       % Light all the LEDs.
	delay (200)
	parallelput (ALL_LED_DARK)              % Darkens all the LEDs.
	delay (200)
    end for
end BlinkLEDs

% Plays the a demonstration piece of "Mary Had a Little Lamb".
procedure Demo
    var xcount, ycount : int
    var character : string (1)
    xcount := 0
    ycount := 0
    parallelput (ALL_LED_DARK)                   % Darken all the LEDs.
    loop
	drawfilloval (2, 2, xcount, ycount, brightmagenta)
	xcount := xcount + 4
	ycount := ycount + 4
	exit when xcount >= 800
    end loop
    drawfillbox (0, 95, 700, 350, white)
    locate (10, 20)
    colour (gray)
    put "     This demo will play"
    colour (blue)
    put " "
    put "                       MARY HAD A LITTLE LAMB."
    put " "
    colour (red)
    put "     A light will come on for each corresponding note."
    put " "
    put "               (i.e. play-by-light-made-even-easier)"
    put " "
    put " "
    put " "
    put " "
    put " "
    put " "
    put "Press any keyboard key to start the demo, and watch "
    put "the magical light show!!"
    loop
	exit when hasch
    end loop
    getch (character)
    cls
    locate (10, 5)
    colour (green)
    put "                      YE GOOD OL' DEMO!!!!"
    DrawButtons
    PlayENote
    delay (w1)
    PlayDNote
    delay (w1)
    PlayCNote
    delay (w1)
    PlayDNote
    delay (w1)
    PlayENote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayENote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayENote
    delay (w2)
    PlayDNote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayDNote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayDNote
    delay (w2)
    PlayENote
    delay (w1)
    PlayGNote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayGNote
    delay (w2)
    PlayENote
    delay (w1)
    PlayDNote
    delay (w1)
    PlayCNote
    delay (w1)
    PlayDNote
    delay (w1)
    PlayENote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayENote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayENote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayENote
    delay (w1)
    PlayDNote
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (w1)
    PlayDNote
    delay (w1)
    PlayENote
    delay (w1)
    PlayDNote
    delay (w1)
    PlayCNote
    delay (w1)

    % Blinks all the LEDs on, then off five times.
    BlinkLEDs (5)
end Demo

% Allows the user to play with the notes until she or he is
% ready to begin.
procedure Test
    var note : int
    var character : string (1)
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    locate (5, 10)
    put "Warm up your fingers on the buttons."
    put " They are labelled according to the correct pitches..."
    put " "
    colour (brightmagenta)
    put "                        " ..
    put "C      " : 7 ..
    colour (brightcyan)
    put "D" : 7 ..
    colour (brightred)
    put "E" : 7 ..
    colour (brightblue)
    put "G" : 7
    locate (24, 10)
    put " When you're ready to start, press any keyboard key..."
    DrawButtons
    loop
	note := parallelget
	delay (50)
	if note = C_BUTTON then
	    PlayCNote
	elsif note = D_BUTTON then
	    PlayDNote
	elsif note = E_BUTTON then
	    PlayENote
	elsif note = G_BUTTON then
	    PlayGNote
	end if
	exit when hasch
    end loop
    getch (character)
end Test

% This procedure is called when the user wins the game.
procedure DrawYouWin
    % Blink all the LEDs on, then off 5 times
    BlinkLEDs (5)

    % Plays a tune, shows a message, and displays a graphic.
    colourback (white)
    locate (14, 30)
    put "DY-NO-MITE!!!!!"
    play ("gg8fe4e8de2e")
    play ("8de4e8de4ge16g2f2d")
    cls
    locate (9, 5)
    put "            WOOOOOOOOOOOWWWWWWW!!!!!"
    put " "
    put "               YOU DID IT!!!!!!!!!"
    for counter1 : 1 .. 20 * maxcolor by 20
	for counter : 1 .. 50
	    drawline (counter1 + counter, counter1 + counter,
		maxx - counter1 - counter,
		maxy - counter1 - counter,
		counter1 div 20)
	end for
    end for
    locate (20, 60)
    put "Congratulations!"
end DrawYouWin

% Plays the difficult version of the game (no hints).
procedure PlayHardDifficulty (var name : string)
    var last_note, counter, note : int
    var cnote, dnote, enote, gnote : int := 0
    var character : string (1)
    counter := 0
    last_note := -1
    DrawButtons
    locate (5, 10)
    put "   OK ", name, ", when the beep sounds, correctly play"
    colour (red)
    put " "
    put "            MARY HAD A LITTLE LAMB" ..
    colour (brightblue)
    put " at your own speed on the buttons."
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    delay (3000)
    play (">>2C<<")
    delay (w1)

    % Loop reads music box buttons until twenty-six different notes
    % have been played.
    loop
	% Loop until a different button in the music
	% box has been pressed.
	loop
	    note := parallelget
	    delay (50)
	    exit when note not= last_note
	end loop
	last_note := note

	% Plays the note corresponding to the button pressed.
	if note = C_BUTTON then
	    PlayCNote
	    cnote := cnote + 1
	    counter := counter + 1
	    delay (w3)
	elsif note = D_BUTTON then
	    PlayDNote
	    dnote := dnote + 1
	    counter := counter + 1
	    delay (w3)
	elsif note = E_BUTTON then
	    PlayENote
	    enote := enote + 1
	    counter := counter + 1
	    delay (w3)
	elsif note = G_BUTTON then
	    PlayGNote
	    gnote := gnote + 1
	    counter := counter + 1
	    delay (w3)
	end if

	% Exits the loop after twenty-six notes have been played.
	exit when counter = 26
    end loop

    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.
    if cnote = 3 and dnote = 10 and enote = 11 and gnote = 2 then
	DrawYouWin
	delay (1500)
    else
	for count : 1 .. 8
	    colour (count)
	    colourback (count + 3)
	    cls
	    locate (10, 30)
	    put "GAME OVER"
	    delay (400)
	end for
	play ("8GGG2E-")
	delay (400)
	play ("8FFF2DD")
	cls
	locate (10, 13)
	colourback (white)
	colour (red)
	put "You were not successful.  Actually, you stunk. "
	put "Try again."
	delay (2500)
    end if
end PlayHardDifficulty

% Lets the user enter her or his name.
procedure EnterName (var name : string)
    colour (darkgray)
    locate (10, 10)
    put "Please enter your name, then press enter"
    put " "
    put "      -->" ..
    get name : *
    put "      Thank you"
end EnterName

% Lights the LED indicating which note to play.
% Waits until the user presses the correct button
% in the music box and then plays the note and delay.
procedure WaitForButtonPress (ledToLight, buttonToWaitFor : int)
    var note : int
    parallelput (ledToLight)
    loop
	note := parallelget
	exit when note = buttonToWaitFor
    end loop
    if note = C_BUTTON then
	PlayCNote
    elsif note = D_BUTTON then
	PlayDNote
    elsif note = E_BUTTON then
	PlayENote
    elsif note = G_BUTTON then
	PlayGNote
    end if
    delay (w1)
end WaitForButtonPress

% Plays the easy version of the game.
procedure PlayEasyDifficulty (var name : string)
    var note : int
    var xcount, ycount : int
    var character : string (1)
    parallelput (ALL_LED_DARK)                          % Darkens all the LEDs.

    % Draws some graphics.
    xcount := 0
    ycount := 0
    loop
	drawfilloval (2, 2, xcount, ycount, brightred)
	xcount := xcount + 4
	ycount := ycount + 4
	exit when xcount >= 800
    end loop
    drawfillbox (0, 95, 700, 350, 0)
    locate (10, 20)
    colour (gray)
    put "            EASY VERSION"
    put " "
    put " "
    put "    Since we always want to help, here's an easier way "
    put " "
    put "    a real success in this game.  You can "
    put " "
    put "    PLAY-BY LIGHTS!!!!!   When you're ready, press "
    put " "
    put "    a keyboard key. After each beep, press the button"
    put " "
    put "    that matches the light that just flashed."
    put " "

    % Waits until the user presses a key.
    loop
	exit when hasch
    end loop
    getch (character)
    cls
    locate (10, 5)
    colour (green)
    put "                           PLAY-BY-LIGHT"
    DrawButtons

    delay (w2 + w1)

    % For each note, lights the LED indicating which note to play.
    % Then waits until the user presses the correct button
    % in the music box and plays the note.
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (C_LED, C_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (G_LED, G_BUTTON)
    WaitForButtonPress (G_LED, G_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (C_LED, C_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (E_LED, E_BUTTON)
    WaitForButtonPress (D_LED, D_BUTTON)
    WaitForButtonPress (C_LED, C_BUTTON)
    cls
    locate (15, 36)
    put " DONE"
    locate (17, 30)
    put "Now try the real thing."
    BlinkLEDs (3)
    delay (1000)
end PlayEasyDifficulty

% Introduces the game by blinking LEDs and playing music.
procedure StartGame (var name : string)
    play ("16CEG8>C<16E>2C")
    BlinkLEDs (3)
    locate (15, 30)
    put "Welcome ", name, "!!!!!!!!!!!!"
    play ("16CEG8>C<16E>2C<<")
    BlinkLEDs (3)
    delay (1000)
end StartGame

% Draws a button onscreen to let the user continue the game.
% Loops until the user presses the button.
procedure ContinueGame
    var xmouse, ymouse, button : int
    drawbox (44, 14, 61, 31, brightmagenta)
    colour (green)
    locate (24, 10)
    put "Click to continue "
    loop
	mousewhere (xmouse, ymouse, button)
	exit when button = 1 and 44 <= xmouse and xmouse <= 61
	    and 14 <= ymouse and ymouse <= 31
    end loop
end ContinueGame

% Lets the user choose whether to play the demo, play an
% easy game, or play a difficult game.
procedure GetSelection (var selection : int)
    var xmouse, ymouse, button : int
    cls
    locate (3, 1)
    colour (green)
    put "       In this nifty little program, you can try "
    put "       to correctly play"
    colour (gray)
    put " "
    put "                   MARY had a LITTLE LAMB"
    locate (7, 20)
    put "                Menu"
    put " "
    put " "
    put " "
    put "       Please use the mouse to click on your selection "
    locate (15, 19)
    colour (brightmagenta)
    put "DEMO" ..
    colour (brightcyan)
    put "         EASY" ..
    colour (brightred)
    put "         HARD" ..
    colour (brightblue)
    put "        QUIT"
    DrawButtons
    drawfillbox (0, 75, 700, 110, white)
    loop
	mouseshow
	selection := 0
	mousewhere (xmouse, ymouse, button)
	if button = 1 and xmouse >= BTN_X1 and xmouse <= BTN_X2
		and ymouse >= BTN_Y1 and ymouse <= BTN_Y2 then
	    selection := 1
	end if
	if button = 1 and xmouse >= BTN2_X1 and xmouse <= BTN2_X2
		and ymouse >= BTN2_Y1 and ymouse <= BTN2_Y2 then
	    selection := 2
	end if
	if button = 1 and xmouse >= BTN3_X1 and xmouse <= BTN3_X2
		and ymouse >= BTN3_Y1 and ymouse <= BTN3_Y2 then
	    selection := 3
	end if
	if button = 1 and xmouse >= BTN4_X1 and xmouse <= BTN4_X2
		and ymouse >= BTN4_Y1 and ymouse <= BTN4_Y2 then
	    selection := 4
	end if
	exit when selection = 1 or selection = 2 or selection = 3
	    or selection = 4
    end loop
end GetSelection

% Outputs some graphics when the user quits.
procedure QuitProgram
    locate (10, 10)
    colour (gray)
    put "               Goodbye!!!!!!!"
    put " "
    put "               Hope you had a real swell time."
    delay (2500)
    cls
end QuitProgram

% Main program
setscreen ("graphics")
setscreen ("nocursor")
var name : string
var selection : int

parallelput (ALL_LED_DARK)                              % Darkens all the LEDs.
cls
EnterName (name)
StartGame (name)
ContinueGame

% Each time through the loop, gets the user's selection and
% plays the demo, the easy game, the hard game, or quit.
loop
    cls
    GetSelection (selection)
    cls
    if selection = 1 then
	Demo
	delay (500)
    elsif selection = 2 then
	PlayEasyDifficulty (name)
    elsif selection = 3 then
	Test
	cls
	PlayHardDifficulty (name)
    end if
    exit when selection = 4
end loop

QuitProgram
parallelput (ALL_LED_DARK)                              % Darkens all the LEDs.
