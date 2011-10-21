setscreen ("graphics:680;540")

const boxSize := 50
const leftX := 20
const topY := maxy - 20
const numberButtonWidth := 50
const textButtonWidth := 115
const buttonHeight := 30
const topFontMargin := 0.25
const boardNumFontSize := 30
const boardSmallNumFontSize := 9
const possibleValuesFontSize := 20
const buttonFontSize := 14
const blinkTime := 300
const buttonDown := true
const buttonUp := false
const up := true
const down := false
const cursorColour := brightpurple
const errorColour := brightred
const boardNumFont := Font.New ("Arial:" + intstr (boardNumFontSize) + ":bold")
const boardSmallNumFont :=
    Font.New ("Arial:" + intstr (boardSmallNumFontSize) + ":bold")
const f1 := Font.New ("Arial:" + intstr (possibleValuesFontSize) + ":bold")
const f2 := Font.New ("Arial:" + intstr (buttonFontSize))
const defaultDefaultFileName := "sudoku.txt"
const iniFile := "sudoku.ini"

% The heights of the fonts
var boardNumFontHeight, boardSmallNumFontHeight, boardSmallNumFontDescent : int
var f1Height, f2Height : int
% The number in each square (0 = blank)
var nums : array 1 .. 9, 1 .. 9 of int
% The array of possible values
var vals : array 1 .. 9, 1 .. 9 of array 1 .. 9 of boolean
% The array of values that user has ruled out
var cantBeVals : array 1 .. 9, 1 .. 9 of array 1 .. 9 of boolean
% The array of empty squares that must hold a particular value
var mustBe : array 1 .. 9, 1 .. 9 of int
% The number of squares that can only hold a single value
var numSolvable : int
% The undo list.  Format is:
%    kind = 1 user entered a number, kind = 2 user added a cantBeVal
%    x, y: coordinate of the square
%    i: for kind = 1, i = number entered, kind = 2, i = number can't be.
var numUndo : int := 0
type UndoKind : enum (Number, CantBe)
var undoList : array 1 .. 1000 of
    record
	kind : UndoKind
	x, y, i : int
    end record
% The coordinates of each square
var x1, y1, x2, y2 : array 1 .. 9, 1 .. 9 of int
% The coordinates of the last set of possible values displayed
% (so that we don't show the same thing over and over, causing flashing)
var lastDisplayedX, lastDisplayedY : int := 0
% The coordinates of each number button
var numButtonX1, numButtonY1, numButtonX2, numButtonY2 : array 1 .. 9 of int
% The coordinates of the solve button
%var solveButtonX1, solveButtonY1, solveButtonX2, solveButtonY2 : int
% The coordinates of the quit button
%var quitButtonX1, quitButtonY1, quitButtonX2, quitButtonY2 : int
% Whether do display possible number values in each square
var displayAllMode : boolean := false

type Buttons : enum (SolveOne, SolveMany, SaveBoard, DisplayAll, Undo, Quit)

var button : array Buttons of
    record
	x1 : int
	y1 : int
	x2 : int
	y2 : int
	text : string (12)
	enabled : boolean
    end record

procedure BadKeySound
    % TW
end BadKeySound

procedure Initialize
    var d1, d2, d3 : int

    Font.Sizes (boardNumFont, boardNumFontHeight, d1, d2, d3)
    Font.Sizes (boardSmallNumFont, boardSmallNumFontHeight, d1,
	boardSmallNumFontDescent, d3)
    Font.Sizes (f1, f1Height, d1, d2, d3)
    Font.Sizes (f2, f2Height, d1, d2, d3)

    % Initialize the locations on the board
    for i : 1 .. 9
	x1 (1, i) := leftX + 1
	x1 (2, i) := leftX + boxSize + 1
	x1 (3, i) := leftX + 2 * boxSize + 1
	x1 (4, i) := leftX + 3 * boxSize + 2
	x1 (5, i) := leftX + 4 * boxSize + 1
	x1 (6, i) := leftX + 5 * boxSize + 1
	x1 (7, i) := leftX + 6 * boxSize + 2
	x1 (8, i) := leftX + 7 * boxSize + 1
	x1 (9, i) := leftX + 8 * boxSize + 1
	x2 (1, i) := leftX + boxSize - 1
	x2 (2, i) := leftX + 2 * boxSize - 1
	x2 (3, i) := leftX + 3 * boxSize - 1
	x2 (4, i) := leftX + 4 * boxSize - 1
	x2 (5, i) := leftX + 5 * boxSize - 1
	x2 (6, i) := leftX + 6 * boxSize - 1
	x2 (7, i) := leftX + 7 * boxSize - 1
	x2 (8, i) := leftX + 8 * boxSize - 1
	x2 (9, i) := leftX + 9 * boxSize - 1
	y1 (i, 1) := topY - boxSize + 1
	y1 (i, 2) := topY - 2 * boxSize + 1
	y1 (i, 3) := topY - 3 * boxSize + 1
	y1 (i, 4) := topY - 4 * boxSize + 1
	y1 (i, 5) := topY - 5 * boxSize + 1
	y1 (i, 6) := topY - 6 * boxSize + 1
	y1 (i, 7) := topY - 7 * boxSize + 1
	y1 (i, 8) := topY - 8 * boxSize + 1
	y1 (i, 9) := topY - 9 * boxSize + 1
	y2 (i, 1) := topY - 1
	y2 (i, 2) := topY - boxSize - 1
	y2 (i, 3) := topY - 2 * boxSize - 1
	y2 (i, 4) := topY - 3 * boxSize - 2
	y2 (i, 5) := topY - 4 * boxSize - 1
	y2 (i, 6) := topY - 5 * boxSize - 1
	y2 (i, 7) := topY - 6 * boxSize - 2
	y2 (i, 8) := topY - 7 * boxSize - 1
	y2 (i, 9) := topY - 8 * boxSize - 1
    end for

    % Initialize the board values to empty
    for x : 1 .. 9
	for y : 1 .. 9
	    nums (x, y) := 0
	    mustBe (x, y) := 0
	    for i : 1 .. 9
		cantBeVals (x, y) (i) := false
	    end for
	end for
    end for

    % Initialize the position of the number buttons
    % Draw the number buttons
    var btnLeftX := x2 (9, 1) + 20
    var y := topY

    for i : 1 .. 9
	numButtonX1 (i) := btnLeftX
	numButtonX2 (i) := btnLeftX + numberButtonWidth
	numButtonY1 (i) := y - buttonHeight
	numButtonY2 (i) := y
	y -= 40
    end for

    btnLeftX += numberButtonWidth + 20
    y := topY

    for i : Buttons
	button (i).x1 := btnLeftX
	button (i).x2 := btnLeftX + textButtonWidth
	button (i).y1 := y - buttonHeight
	button (i).y2 := y

	button (i).enabled := true

	y -= 60
    end for

    button (Buttons.SolveOne).text := "Solve One"
    button (Buttons.SolveMany).text := "Solve Many"
    button (Buttons.Quit).text := "Quit Game"
    button (Buttons.SaveBoard).text := "Save Board"
    button (Buttons.DisplayAll).text := "Show Nums"
    button (Buttons.Undo).text := "Undo"
    button (Buttons.Undo).enabled := false
end Initialize

procedure DrawBoardSquare (x, y, clr : int)
    const leftSideOfBox := x1 (x, y)
    const rightSideOfBox := x2 (x, y)
    const bottomEdgeOfBox := y1 (x, y)
    const topEdgeOfBox := y2 (x, y)

    % Clear the box
    Draw.FillBox (x1 (x, y), y1 (x, y), x2 (x, y), y2 (x, y), clr)
    if nums (x, y) not= 0 then
	const w := Font.Width (intstr (nums (x, y)), boardNumFont)
	Font.Draw (intstr (nums (x, y)),
	    leftSideOfBox + (boxSize - w) div 2,
	    topY - round ((y - topFontMargin) * boxSize), boardNumFont, black)
    elsif mustBe (x, y) not= 0 then
	const w := Font.Width (intstr (mustBe (x, y)), boardNumFont)
	Font.Draw (intstr (mustBe (x, y)),
	    leftSideOfBox + (boxSize - w) div 2,
	    topY - round ((y - topFontMargin) * boxSize), boardNumFont, grey)
    elsif displayAllMode then
	for i : 0 .. 2
	    for j : 0 .. 2
		if vals (x, y) (i * 3 + j + 1) then
		    var w := Font.Width ("0", boardSmallNumFont)
		    const horSpace := (boxSize - 3 * w) div 4
		    const vertSpace := (boxSize - 3 * boardSmallNumFontHeight) div 4
		    var xLoc := leftSideOfBox + horSpace + j * (w + horSpace)
		    var yLoc := topEdgeOfBox - vertSpace - i * (boardSmallNumFontHeight + vertSpace) - boardSmallNumFontHeight + 4
		    Font.Draw (intstr (i * 3 + j + 1), xLoc, yLoc, boardSmallNumFont, black)
		    w := x2 (x, y) - x1 (x, y)

		    Draw.Line (leftSideOfBox + w div 3, bottomEdgeOfBox,
			leftSideOfBox + w div 3, topEdgeOfBox, grey)
		    Draw.Line (leftSideOfBox + 2 * w div 3, bottomEdgeOfBox,
			leftSideOfBox + 2 * w div 3, topEdgeOfBox, grey)
		    w := y2 (x, y) - y1 (x, y)
		    Draw.Line (leftSideOfBox, bottomEdgeOfBox + w div 3,
			rightSideOfBox, bottomEdgeOfBox + w div 3, grey)
		    Draw.Line (leftSideOfBox, bottomEdgeOfBox + 2 * w div 3,
			rightSideOfBox, bottomEdgeOfBox + 2 * w div 3, grey)
		end if
	    end for
	end for
    end if
end DrawBoardSquare

procedure FlashNumber (x, y, clr : int)
    % Clear the box
    for i : 1 .. 6
	if i mod 2 = 1 then
	    Draw.FillBox (x1 (x, y), y1 (x, y), x2 (x, y), y2 (x, y), clr)
	else
	    Draw.FillBox (x1 (x, y), y1 (x, y), x2 (x, y), y2 (x, y), white)
	end if
	if nums (x, y) not= 0 then
	    var w := Font.Width (intstr (nums (x, y)), boardNumFont)
	    Font.Draw (intstr (nums (x, y)),
		leftX + (x - 1) * boxSize + (boxSize - w) div 2,
		topY - round ((y - topFontMargin) * boxSize),
		boardNumFont, black)
	end if
	delay (100)
    end for
end FlashNumber

procedure DrawBoard
    Draw.Box (leftX, topY, leftX + 9 * boxSize, topY - 9 * boxSize, black)
    Draw.Box (leftX - 1, topY + 1,
	leftX + 9 * boxSize + 1, topY - 9 * boxSize - 1, black)
    for i : 1 .. 8
	Draw.Line (leftX + i * boxSize, topY,
	    leftX + i * boxSize, topY - 9 * boxSize, black)
	if i mod 3 = 0 then
	    Draw.Line (leftX + i * boxSize + 1, topY,
		leftX + i * boxSize + 1, topY - 9 * boxSize, black)
	end if
	Draw.Line (leftX, topY - i * boxSize,
	    leftX + 9 * boxSize, topY - i * boxSize, black)
	if i mod 3 = 0 then
	    Draw.Line (leftX, topY - i * boxSize - 1,
		leftX + 9 * boxSize, topY - i * boxSize - 1, black)
	end if
    end for
    for x : 1 .. 9
	for y : 1 .. 9
	    DrawBoardSquare (x, y, white)
	end for
    end for
end DrawBoard

function ValidateBoard : boolean
    var sx, sy : int

    for x : 1 .. 9
	for y : 1 .. 9
	    if nums (x, y) not= 0 then
		% First vertically
		for i : 1 .. 9
		    if i not= y and nums (x, i) = nums (x, y) then
			result false
		    end if
		end for
		% Second vertically
		for i : 1 .. 9
		    if i not= x and nums (i, y) = nums (x, y) then
			result false
		    end if
		end for
		% Third, in the square
		sx := (x - 1) div 3 * 3
		sy := (y - 1) div 3 * 3
		for i : sx + 1 .. sx + 3
		    for j : sy + 1 .. sy + 3
			if i not= x and i not= y and
				nums (i, j) = nums (x, y) then
			    result false
			end if
		    end for
		end for
	    end if
	end for
    end for
    result true
end ValidateBoard

procedure ClearTextArea
    const left := x1 (1, 1)
    const right := x2 (9, 1)

    Draw.FillBox (left, 0, right, y1 (1, 9) - 3, white)
end ClearTextArea


function GetTextInput (prompt, default : string) : string
    var line : string
    var defaultValue : string := default
    var len : int
    var f : int

    if defaultValue not= "" and defaultValue (1) = "*" then
	defaultValue := defaultValue (2 .. *)
	len := length (defaultValue)
	open : f, iniFile, get
	if f > 0 then
	    loop
		exit when eof (f)
		get : f, line : *
		if length (line) > len and
			line (1 .. len + 1) = default + "=" then
		    defaultValue := line (len + 2 .. *)
		    exit
		end if
	    end loop
	    close : f
	end if
    end if

    ClearTextArea
    locate (maxrow - 1, 4)
    if default not= "" then
	put prompt, " (default: ", defaultValue, "): " ..
    else
	put prompt, ": " ..
    end if
    loop
	get line : *
	if line = "" then
	    line := defaultValue
	end if
	exit when line not= ""
	BadKeySound
    end loop
    result line
end GetTextInput


% Display instructions
procedure Instructions (line1, line2, line3 : string)
    const left := x1 (1, 1)
    const right := x2 (9, 1)

    ClearTextArea

    var y := y1 (1, 9) - f2Height - 3
    var w := Font.Width (line1, f2)
    Font.Draw (line1, left + (right - left - w) div 2, y, f2, black)
    y -= f2Height
    w := Font.Width (line2, f2)
    Font.Draw (line2, left + (right - left - w) div 2, y, f2, black)
    y -= f2Height
    w := Font.Width (line3, f2)
    Font.Draw (line3, left + (right - left - w) div 2, y, f2, black)
end Instructions


procedure SaveBoard
    var fileName : string
    var f : int

    fileName := GetTextInput ("Enter save file name", "*lastname")
    open : f, fileName, put

    var success : boolean := false
    if f <= 0 then
	Instructions ("Unable to open '" + fileName + "'",
	    Error.LastMsg, "")
	return
    end if
    for y : 1 .. 9
	for x : 1 .. 9
	    put : f, nums (x, y), " " ..
	end for
	put : f, ""
    end for
    close : f
    Instructions ("Board Saved", "", "")
end SaveBoard


function LoadBoard : boolean
    var fileName : string
    var f : int
    var line : string

    ClearTextArea

    % Read default name from file
    fileName := GetTextInput ("Enter a file to load from disk", "*lastname")

    open : f, fileName, get
    var success : boolean := false
    if f < 0 then
	Instructions ("Unable to open '" + fileName + "'",
	    Error.LastMsg, "")
	result false
    end if
    for y : 1 .. 9
	for x : 1 .. 9
	    get : f, skip
	    if eof (f) then
		close : f
		Instructions ("Unable to read '" + fileName + "'",
		    Error.LastMsg, "")
		result false
	    end if
	    get : f, nums (x, y)
	end for
    end for
    close : f

    open : f, iniFile, put
    if f > 0 then
	put : f, "lastfile=", fileName
	close : f
    end if

    DrawBoard

    result true
end LoadBoard


procedure GetInitialSetupFromUser
    var x, y : int := 1
    var cursorDisplayed : boolean := false
    var cursorTime : int := Time.Elapsed
    var ch : string (1)
    var prevNum : int

    Instructions ("Use numbers and arrow keys to set up board",
	"(space to erase) or L to load a board from a file.",
	"Press ESC when completed.")

    loop
	cursorTime := 0
	loop
	    exit when hasch
	    if Time.Elapsed - cursorTime > blinkTime then
		% Reverse the cursor
		cursorDisplayed := not cursorDisplayed
		if cursorDisplayed then
		    DrawBoardSquare (x, y, cursorColour)
		else
		    DrawBoardSquare (x, y, white)
		end if
		cursorTime := Time.Elapsed
	    end if
	end loop
	% The user typed something
	% If the cursor is on, then turn it off before processing
	if cursorDisplayed then
	    cursorDisplayed := not cursorDisplayed
	    DrawBoardSquare (x, y, white)
	end if

	ch := getchar
	if ch = " " or ("1" <= ch and ch <= "9") then
	    prevNum := nums (x, y)
	    if ch = " " then
		nums (x, y) := 0
	    else
		nums (x, y) := strint (ch)
	    end if
	    DrawBoardSquare (x, y, white)
	    if not ValidateBoard then
		BadKeySound
		FlashNumber (x, y, errorColour)
		nums (x, y) := prevNum
		DrawBoardSquare (x, y, white)
	    else
		x := x + 1
		if x > 9 then
		    x := 1
		    y += 1
		    if y > 9 then
			y := 1
		    end if
		end if
	    end if
	elsif ch = KEY_RIGHT_ARROW then
	    x := x mod 9 + 1
	elsif ch = KEY_LEFT_ARROW then
	    x := x - 1
	    if x = 0 then
		x := 9
	    end if
	elsif ch = KEY_DOWN_ARROW then
	    y := y mod 9 + 1
	elsif ch = KEY_UP_ARROW then
	    y := y - 1
	    if y = 0 then
		y := 9
	    end if
	elsif ch = KEY_ESC then
	    return
	elsif ch = 'l' or ch = 'L' then
	    if not LoadBoard then
	    end if
	else
	    BadKeySound
	end if
    end loop
end GetInitialSetupFromUser

% Calculate the allowable values for each square
procedure CalculatePossibleValuesForEachSquare
    var sx, sy : int
    var unique : boolean

    numSolvable := 0
    button (Buttons.SolveOne).enabled := false
    button (Buttons.SolveMany).enabled := false
    for x : 1 .. 9
	for y : 1 .. 9
	    mustBe (x, y) := 0

	    if nums (x, y) = 0 then
		% Start with all values possible
		for i : 1 .. 9
		    vals (x, y) (i) := not cantBeVals (x, y) (i)
		end for

		% First vertically
		for i : 1 .. 9
		    if nums (x, i) not= 0 then
			vals (x, y) (nums (x, i)) := false
		    end if
		end for

		% Second horizontally
		for i : 1 .. 9
		    if nums (i, y) not= 0 then
			vals (x, y) (nums (i, y)) := false
		    end if
		end for

		% Third, in the square
		sx := (x - 1) div 3 * 3
		sy := (y - 1) div 3 * 3
		for i : 1 .. 3
		    for j : 1 .. 3
			if nums (sx + i, sy + j) not= 0 then
			    vals (x, y) (nums (sx + i, sy + j)) := false
			end if
		    end for
		end for

		% What happens if nothing fits?
		var numPossibleValues := 0
		for i : 1 .. 9
		    if vals (x, y) (i) = true then
			numPossibleValues += 1
		    end if
		end for
		if numPossibleValues = 0 then
		    % TW - Big problem. Nothing fits.
		    assert false
		end if
	    else
		% Value is already in square, no possible values
		for i : 1 .. 9
		    vals (x, y) (i) := false
		end for
	    end if
	end for
    end for

    % If there's only one value left in the square then it must be the sol'n
    for x : 1 .. 9
	for y : 1 .. 9
	    var num : int := 0
	    for i : 1 .. 9
		if vals (x, y) (i) then
		    if num = 0 then
			num := i
		    else
			num := -1
		    end if
		end if
	    end for
	    if num > 0 then
		mustBe (x, y) := num
		numSolvable += 1
		button (Buttons.SolveOne).enabled := true
		button (Buttons.SolveMany).enabled := true
		%locate (maxrow, 1)
		%Iput "U",x, " ", y, " ", num..
		%Input.Pause
	    end if
	end for
    end for

    % Now, for each line, see if there's only a single type of value
    % in which case, the value *must* be located there.
    for x : 1 .. 9
	for y : 1 .. 9
	    if nums (x, y) = 0 then
		for val : 1 .. 9
		    if vals (x, y) (val) then
			unique := true
			% First vertically
			for i : 1 .. 9
			    if i not= y and vals (x, i) (val) then
				unique := false
				exit
			    end if
			end for
			%if unique then
			%locate (maxrow, 1)
			%put "V",x, " ", y, " ", val..
			%Input.Pause
			%end if

			% Second horizontally - only test if we haven't found
			% uniqueness previously
			if not unique then
			    unique := true
			    for i : 1 .. 9
				%locate (maxrow, 1)
				%put "*",i, " ", y, " ", vals (x, i) (val)
				%Input.Pause
				if i not= x and vals (i, y) (val) then
				    unique := false
				    exit
				end if
			    end for
			    %if unique then
			    %locate (maxrow, 1)
			    %put "H",x, " ", y, " ", val..
			    %Input.Pause
			    %end if
			end if

			% Third, in the square - only test if we haven't found
			% uniqueness previously
			if not unique then
			    unique := true
			    sx := (x - 1) div 3 * 3
			    sy := (y - 1) div 3 * 3
			    for i : 1 .. 3
				for j : 1 .. 3

				    %locate (maxrow, 1)
				    %put "*",sx + i, " ", sy + j, " ", vals (sx + i, sy + j) (val)
				    %Input.Pause
				    if (sx + i not= x or sy + j not= y) and
					    vals (sx + i, sy + j) (val) then
					unique := false
					exit
				    end if
				end for
				exit when not unique
			    end for
			    %if unique then
			    %locate (maxrow, 1)
			    %put "S",x, " ", y, " ", val..
			    %Input.Pause
			    %end if
			end if
			%locate (maxrow, 1)
			%put x, " ", y, " ", val, " ", unique
			%Input.Pause
			if unique then
			    if mustBe (x, y) not= 0 and
				    mustBe (x, y) not= val then
				% TW: Trouble - This position must be held by
				% two numbers!
				put x, " ", y, " ", mustBe (x, y), " ", val
				assert false
			    else
				mustBe (x, y) := val
				for i : 1 .. 9
				    if i not= val then
					vals (x, y) (i) := false
				    end if
				end for
				numSolvable += 1
				button (Buttons.SolveOne).enabled := true
				button (Buttons.SolveMany).enabled := true
			    end if
			end if
		    end if
		end for
	    end if
	end for
    end for
end CalculatePossibleValuesForEachSquare

procedure DrawNumberButton (btnNumber : int, position : boolean)
    var buttonClr : int

    if position = up then
	buttonClr := brightcyan
    else
	buttonClr := cyan
    end if

    Draw.Box (numButtonX1 (btnNumber), numButtonY1 (btnNumber),
	numButtonX2 (btnNumber), numButtonY2 (btnNumber), black)
    Draw.Box (numButtonX1 (btnNumber) - 1, numButtonY1 (btnNumber) - 1,
	numButtonX2 (btnNumber) + 1, numButtonY2 (btnNumber) + 1, black)
    Draw.FillBox (numButtonX1 (btnNumber) + 1, numButtonY1 (btnNumber) + 1,
	numButtonX2 (btnNumber) - 1, numButtonY2 (btnNumber) - 1, buttonClr)
    var w := Font.Width (intstr (btnNumber), f1)
    Font.Draw (intstr (btnNumber),
	numButtonX1 (btnNumber) + (numberButtonWidth - w) div 2,
	numButtonY1 (btnNumber) + 5, f1, black)
end DrawNumberButton

procedure DrawButton (btn : Buttons, position : boolean)
    var textClr : int
    var buttonClr : int
    var myButton := button (btn)

    if myButton.enabled then
	textClr := black
    else
	textClr := grey
    end if
    if position = up then
	buttonClr := brightcyan
    else
	buttonClr := cyan
    end if

    Draw.Box (myButton.x1, myButton.y1,
	myButton.x2, myButton.y2, black)
    Draw.Box (myButton.x1 - 1, myButton.y1 - 1,
	myButton.x2 + 1, myButton.y2 + 1, black)
    Draw.FillBox (myButton.x1 + 1, myButton.y1 + 1,
	myButton.x2 - 1, myButton.y2 - 1, buttonClr)
    var w := Font.Width (myButton.text, f2)
    Font.Draw (myButton.text,
	myButton.x1 + 2 + (textButtonWidth - w) div 2,
	myButton.y1 + 8, f2, textClr)
end DrawButton

procedure DrawButtons
    const leftX := x2 (9, 1) + 20
    var y := topY

    % Draw the number buttons
    for i : 1 .. 9
	DrawNumberButton (i, up)
    end for

    for i : Buttons
	DrawButton (i, up)
    end for
end DrawButtons

function MouseOverSquare (buttonDown : boolean, var x, y, sm : int) : boolean
    var mx, my, b, dx, dy, w, smI, smJ : int
    Mouse.Where (mx, my, b)
    if not buttonDown and b > 0 then
	result false
    end if
    if buttonDown and b = 0 then
	result false
    end if
    for i : 1 .. 9
	for j : 1 .. 9
	    if x1 (i, j) <= mx and mx <= x2 (i, j) and
		    y1 (i, j) <= my and my <= y2 (i, j) then
		x := i
		y := j
		dx := mx - x1 (i, j)
		dy := my - y1 (i, j)
		w := x2 (i, j) - x1 (i, j)
		if dx * 3 < w then
		    smI := 0
		elsif dx * 3 < 2 * w then
		    smI := 1
		else
		    smI := 2
		end if
		w := y2 (i, j) - y1 (i, j)
		if dy * 3 < w then
		    smJ := 2
		elsif dy * 3 < 2 * w then
		    smJ := 1
		else
		    smJ := 0
		end if
		sm := smJ * 3 + smI + 1
		%                put mx, " ", my, " ", smI, " ", smJ, " ", x1 (i, j), " ", x2 (i, j), " ", dx
		%                Input.Pause
		result true
	    end if
	end for
    end for
    x := -1
    y := -1
    result false
end MouseOverSquare

procedure DisplayPossibleValuesOfSquare (x, y : int)
    const msgY := y1 (1, 9) - 35

    if lastDisplayedX = x and lastDisplayedY = y then
	return
    end if

    ClearTextArea
    lastDisplayedX := x
    lastDisplayedY := y

    if x = -1 then
	return
    end if

    for i : 1 .. 9
	if vals (x, y) (i) = true then
	    var w := Font.Width (intstr (i), f1)
	    Font.Draw (intstr (i), x1 (i, y) + (boxSize - w) div 2,
		msgY, f1, grey)
	end if
    end for
end DisplayPossibleValuesOfSquare

function ClickedButton (btn : Buttons) : boolean
    var mx, my, b : int
    var buttonDown := true
    var buttonClicked := button (btn)

    Mouse.Where (mx, my, b)
    if buttonClicked.x1 <= mx and mx <= buttonClicked.x2 and
	    buttonClicked.y1 <= my and my <= buttonClicked.y2 and b > 0 then
	if not buttonClicked.enabled then
	    BadKeySound
	    result false
	end if
	DrawButton (btn, down)
	loop
	    Mouse.Where (mx, my, b)
	    if buttonClicked.x1 <= mx and mx <= buttonClicked.x2 and
		    buttonClicked.y1 <= my and my <= buttonClicked.y2 then
		if not buttonDown then
		    DrawButton (btn, down)
		    buttonDown := true
		end if
	    else
		if buttonDown then
		    DrawButton (btn, up)
		    buttonDown := false
		end if
	    end if
	    exit when b = 0
	end loop
	result buttonDown
    end if
    result false
end ClickedButton

proc DoUndo
    if numUndo = 0 then
	BadKeySound
	return
    end if

    if undoList (numUndo).kind = UndoKind.Number then
	nums (undoList (numUndo).x, undoList (numUndo).y) :=
	    undoList (numUndo).i
    else
	% Undo a cantBeVal
	cantBeVals (undoList (numUndo).x, undoList (numUndo).y) (undoList (numUndo).i) := false
    end if
    numUndo -= 1
    if numUndo = 0 then
	button (Buttons.Undo).enabled := false
    end if
    CalculatePossibleValuesForEachSquare
    DrawBoard
end DoUndo

% Add to undoList
proc AddToUndoList (kind : UndoKind, x, y, i : int)
    numUndo += 1
    undoList (numUndo).kind := kind
    undoList (numUndo).x := x
    undoList (numUndo).y := y
    undoList (numUndo).i := i
    button (Buttons.Undo).enabled := true
    DrawButton (Buttons.Undo, up)
end AddToUndoList

procedure SolveOnePosition
    for y : 1 .. 9
	for x : 1 .. 9
	    if mustBe (x, y) not= 0 then
		AddToUndoList (UndoKind.Number, x, y, nums (x, y))
		nums (x, y) := mustBe (x, y)
		FlashNumber (x, y, brightgreen)
		CalculatePossibleValuesForEachSquare
		DrawBoard
		DrawButton (Buttons.SolveOne, up)
		return
	    end if
	end for
    end for
end SolveOnePosition

function NumberButtonPressed (var number : int) : boolean
    var mx, my, b : int
    var buttonDown := true

    Mouse.Where (mx, my, b)

    if b = 0 then
	result false
    end if

    for i : 1 .. 9
	if numButtonX1 (i) <= mx and mx <= numButtonX2 (i) and
		numButtonY1 (i) <= my and my <= numButtonY2 (i) then
	    DrawNumberButton (i, down)
	    number := i
	    result true
	end if
    end for

    result false
end NumberButtonPressed

procedure DisplayPositionsWhereNumberPossible (number : int)
    var mx, my, b1, b2 : int

    for x : 1 .. 9
	for y : 1 .. 9
	    if vals (x, y) (number) then
		DrawBoardSquare (x, y, gray)
	    end if
	end for
    end for

    % TW - Display positions
    Mouse.ButtonWait ("up", mx, my, b1, b2)
    DrawNumberButton (number, up)

    for x : 1 .. 9
	for y : 1 .. 9
	    if vals (x, y) (number) then
		DrawBoardSquare (x, y, white)
	    end if
	end for
    end for
end DisplayPositionsWhereNumberPossible

% Main program
var x, y, sm : int
var insertMode : boolean := false
var cursorX, cursorY : int
var cursorDisplayed : boolean := false
var cursorTime : int
var prevNum : int
var lastMouseX, lastMouseY : int := 0
var number : int
var ch : string (1)
var quitting : boolean := false

Initialize

DrawBoard

GetInitialSetupFromUser

CalculatePossibleValuesForEachSquare

DrawBoard

DrawButtons

ClearTextArea

% Main loop
loop
    %
    % If mouse over square and button not pressed, display possible values
    %
    if MouseOverSquare (buttonUp, x, y, sm) then
	if lastMouseX not= x or lastMouseY not= y then
	    DisplayPossibleValuesOfSquare (x, y)
	    lastMouseX := x
	    lastMouseY := y
	end if
    end if

    %
    % Click on square to enter insertion mode
    %
    if MouseOverSquare (buttonDown, x, y, sm) then
	% Check to make certain we're not already on this square
	if not insertMode or cursorX not= x or cursorY not= y then
	    if displayAllMode then
		% Clicked on one of the small numbers.  Turn it off if
		% not already off.
		if not cantBeVals (x, y) (sm) then
		    % Turn it off
		    cantBeVals (x, y) (sm) := true
		    AddToUndoList (UndoKind.CantBe, x, y, sm)
		    CalculatePossibleValuesForEachSquare
		    DrawBoard
		else
		    BadKeySound
		end if
	    else
		% User clicked on square.
		% If cursor already displayed, hide it
		if cursorDisplayed then
		    cursorDisplayed := not cursorDisplayed
		    DrawBoardSquare (cursorX, cursorY, white)
		end if
		insertMode := true
		cursorTime := 0
		cursorX := x
		cursorY := y
	    end if
	end if
    end if

    %
    % Blink cursor if in insertion mode
    %
    if insertMode then
	if Time.Elapsed - cursorTime > blinkTime then
	    % Reverse the cursor
	    cursorDisplayed := not cursorDisplayed
	    if cursorDisplayed then
		DrawBoardSquare (cursorX, cursorY, cursorColour)
	    else
		DrawBoardSquare (cursorX, cursorY, white)
	    end if
	    cursorTime := Time.Elapsed
	end if
    end if

    %
    % The user pressed a key
    %
    if hasch then
	ch := getchar

	% If we're not in insert mode, ignore it
	if not insertMode then
	    BadKeySound
	    Instructions ("Mouse click on a square to enter insert mode",
		"", "")
	else
	    if cursorDisplayed then
		cursorDisplayed := not cursorDisplayed
		DrawBoardSquare (cursorX, cursorY, white)
		cursorTime := 0
	    end if

	    % Enter a value if in insertion mode
	    if ch = " " or ("1" <= ch and ch <= "9") then
		prevNum := nums (cursorX, cursorY)
		if ch = " " then
		    nums (cursorX, cursorY) := 0
		else
		    nums (cursorX, cursorY) := strint (ch)
		end if

		DrawBoardSquare (cursorX, cursorY, white)
		if not ValidateBoard then
		    BadKeySound
		    FlashNumber (cursorX, cursorY, errorColour)
		    nums (cursorX, cursorY) := prevNum
		    DrawBoardSquare (cursorX, cursorY, white)
		else
		    cursorX := cursorX + 1
		    if cursorX > 9 then
			cursorX := 1
			cursorY += 1
			if cursorY > 9 then
			    cursorY := 1
			end if
		    end if

		    % Add to undoList
		    AddToUndoList (UndoKind.Number, cursorX, cursorY, prevNum)

		    CalculatePossibleValuesForEachSquare
		    DisplayPossibleValuesOfSquare (cursorX, cursorY)
		    DrawBoard
		end if
	    elsif ch = KEY_RIGHT_ARROW then
		cursorX := cursorX mod 9 + 1
		DisplayPossibleValuesOfSquare (cursorX, cursorY)
	    elsif ch = KEY_LEFT_ARROW then
		cursorX := cursorX - 1
		if cursorX = 0 then
		    cursorX := 9
		end if
		DisplayPossibleValuesOfSquare (cursorX, cursorY)
	    elsif ch = KEY_DOWN_ARROW then
		cursorY := cursorY mod 9 + 1
		DisplayPossibleValuesOfSquare (cursorX, cursorY)
	    elsif ch = KEY_UP_ARROW then
		cursorY := cursorY - 1
		if cursorY = 0 then
		    cursorY := 9
		end if
		DisplayPossibleValuesOfSquare (cursorX, cursorY)
	    elsif ch = KEY_ESC then
		if cursorDisplayed then
		    cursorDisplayed := not cursorDisplayed
		    DrawBoardSquare (cursorX, cursorY, white)
		end if
		insertMode := false
		lastMouseX := 0
		lastMouseY := 0
		ClearTextArea
	    else
		BadKeySound
	    end if
	end if
    end if

    for i : Buttons
	if ClickedButton (i) then
	    case i of
		label Buttons.SolveOne :
		    % The user clicked on the Solve button - solve a number
		    SolveOnePosition
		label Buttons.SolveMany :
		    % The user clicked on the Solve Many button - solve as
		    % many as you can, one at a time
		    loop
			exit when not button (Buttons.SolveOne).enabled
			SolveOnePosition
		    end loop
		    DrawButton (Buttons.SolveMany, up)
		label Buttons.Quit :
		    % The user clicked on the Quit button - exit the program
		    quitting := true
		    exit
		label Buttons.SaveBoard :
		    SaveBoard
		    DrawButton (Buttons.SaveBoard, up)
		label Buttons.DisplayAll :
		    displayAllMode := not displayAllMode
		    DrawBoard
		    DrawButton (Buttons.DisplayAll, not displayAllMode)
		label Buttons.Undo :
		    DoUndo
		    DrawButton (Buttons.Undo, up)
	    end case
	end if
    end for

    exit when quitting

    if NumberButtonPressed (number) then
	% The user is pressing on a number button
	DisplayPositionsWhereNumberPossible (number)
    end if
end loop

% Clear the buttons to make it clear the game is over
Draw.FillBox (x2 (9, 1) + 3, 0, maxx, maxy, white)
ClearTextArea
