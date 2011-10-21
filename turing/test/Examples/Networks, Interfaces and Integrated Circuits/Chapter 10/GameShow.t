% The "GameShow" program.

% The following input numbers were used:
% Pin selected   Associated Number
%  pin 10 (I0)            64
%  pin 11 (I1)           128
%  pin 12 (I2)            32
%  pin 13 (I3)            16
% To determine whether a specific pin is pressed, divide (div) by the
% associated number and then use "mod 2" to determine whether or not the
% bottom bit is set. For example: Parallelget returns 10110000.  To determine if
% pin 12 is set, divide by 32 giving 00000101 and then "mod 2" the result
% (which in this case gives "1", meaning pin 12 was set).

setscreen ("graphics")

var starter : string (1)
var value : int
var delayTime : int
% These variables are set to true if a player pressed the button
% before the tone was sounded.
var player1DNQ, player2DNQ, player3DNQ, player4DNQ : boolean := false
var winnerFound : boolean := false

put "Game Show"
put ""
put "After pressing any key, there is a delay from 1-4 seconds"
put "followed by a beep.  The first person to press a button AFTER"
put "the beep wins.  Pressing a button before the beep disqualifies you."
put ""
put "Press the Enter key when contestants are ready."
getch (starter)

% Clears the screen.
cls
locate (5, 35)
put "Wait for the beep!"
locate (7, 1)
% Sets delayTime to a random number from 1000 to 4000.
randint (delayTime, 1000, 4000)
% Adjusts these numbers for your computer.
delay (delayTime)
% Obtains the state of the buttons before the beep.
value := parallelget
play ("16C")

% Determines who gets disqualified for pressing their button before the beep.
if (value div 64) mod 2 = 1 then
    player1DNQ := true
    put "Player 1 disqualified for pressing the button too soon!"
end if
if (value div 128) mod 2 = 1 then
    player2DNQ := true
    put "Player 2 disqualified for pressing the button too soon!"
end if
if (value div 32) mod 2 = 1 then
    player3DNQ := true
    put "Player 3 disqualified for pressing the button too soon!"
end if
if (value div 16) mod 2 = 1 then
    player4DNQ := true
    put "Player 4 disqualified for pressing the button too soon!"
end if

locate (15, 35)
put "Press your button!"
locate (20, 1)

% If all four players disqualify themselves, no one wins!
loop
    % Value stores the state of the buttons.
    value := parallelget

    % If a player has pressed the button and she or he is not already
    % disqualified, that player wins!  There can be more than
    % one winner if several players press their buttons at the same time.

    if (value div 64) mod 2 = 1 and not player1DNQ then
	winnerFound := true
	put "Player 1 is a winner!"
    end if
    if (value div 128) mod 2 = 1 and not player2DNQ then
	winnerFound := true
	put "Player 2 is a winner!"
    end if
    if (value div 32) mod 2 = 1 and not player3DNQ then
	winnerFound := true
	put "Player 3 is a winner!"
    end if
    if (value div 16) mod 2 = 1 and not player4DNQ then
	winnerFound := true
	put "Player 4 is a winner!"
    end if

    % Exits the loop once a winner is found.
    exit when winnerFound
end loop
