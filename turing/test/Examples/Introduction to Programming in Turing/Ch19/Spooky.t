% The "Spooky" program
% A game for a dark night

procedure witch (row, column : int, symbol : string (1))
    locate (row, column)
    put symbol ..
    locate (row + 1, column - 1)
    put repeat (symbol, 3) ..
    locate (row + 2, column - 2)
    put repeat (symbol, 5) ..
    locate (row + 3, column - 4)
    put repeat (symbol, 8) ..
    locate (row + 4, column - 2)
    put symbol, repeat (" ", 3), symbol ..
    locate (row + 5, column - 2)
    put symbol, repeat (" ", 3), symbol ..
    locate (row + 6, column - 1)
    put symbol, " ", symbol ..
    locate (row + 7, column)
    put symbol ..
end witch

procedure monster (row, column : int, symbol : string (1))
    % you fill this in
    locate (row, column)
    put repeat (symbol, 2) ..
end monster

procedure ghost (row, column : int, symbol : string (1))
    % you fill this in
    locate (row, column)
    put repeat (symbol, 3) ..
end ghost

procedure animate (Time, column : int,
	procedure menace (row, column : int,
	symbol : string (1)),
	magic : string (1), var win : boolean)
    var finished : boolean := false
    var spell : string (1)
    %Suppress cursor and echo of input character
    setscreen ("nocursor")
    setscreen ("noecho")
    win := false
    % Move menace from top to bottom of window
    for line : 1 .. 17
	% Plot menace using asterisks
	menace (line, column, "*")
	% Test to see if a character has been typed
	if hasch then
	    % Input a single character and assign to spell
	    getch (spell)
	    if spell = magic then
		win := true
	    end if
	    % Prepare to stop the animation
	    finished := true
	end if
	delay (Time)          % Delay before erasing
	% Erase plot of menace
	menace (line, column, " ")
	% Stop animation if finished
	exit when finished
    end for
    % Unsuppress cursor and echo of input character
    setscreen ("cursor,echo")
end animate

loop
    put "Do you want to play this scary game? (y or n): " ..
    var reply : string (1)
    get reply
    exit when reply not= "y"
    var win : boolean
    cls
    color (blue)      % Draw figures in blue
    var count := 0
    var delayTime : int
    put "Choose a delay time for animation: " ..
    get delayTime
    loop
	% Choose a column at random
	var column : int
	randint (column, 4, 76)
	% Choose a spook at random
	var spook : int
	randint (spook, 1, 3)
	case spook of
	    label 1 :
		animate (delayTime, column, witch, "w", win)
	    label 2 :
		animate (delayTime, column, monster, "m", win)
	    label 3 :
		animate (delayTime, column, ghost, "g", win)
	end case
	if win then
	    count := count + 1
	else
	    exit
	end if
    end loop
    locate (24, 1)
    put "You warded off ", count, " menaces"
end loop
