unit

class TextFieldClass
    inherit GenericActiveWidgetClass

    import WidgetGlobals

    export
    % The external interface
	Initialize, InitializeFull, GetText, SetText, SetSelection,
	BlinkCursor, SetActive, SetEchoChar,
    % The package interface
	MakeInactive, RemoveCursor, IsActive

    %
    % The inheritable constants of TextFieldClass
    %
    const delayBetweenScrolling := 400
    const rightArrow : char := chr (205)
    const leftArrow : char := chr (203)
    const home : char := chr (199)
    const endKey : char := chr (207)
    const backspace : char := chr (8)
    const delete : char := chr (211)

    %
    % The inheritable variables of TextFieldClass
    %
    var border : int
    var text : string
    var textLen : int
    var inputKind : int
    var xOffsets : array 1 .. 256 of int
    var x1, y1, x2, y2, xOffset : int
    var selStart, selEnd : int
    var firstVisibleChar, lastVisibleChar : int
    var textFontID : int
    var active : boolean := false
    var userActionProc : procedure x (text : string)
    var lastCursorBlink : int := 0
    var cursorOn : boolean := false
    var cursorPic : int := -1
    var charWidth : int
    var echoChar : char := chr (0)
    const blinkSpeed : int := 250
    var leftEdgeTime, rightEdgeTime : int := 0
    var hilightColour : int
    if colourbg = black and backgroundColour = colourbg then
	hilightColour := white
    else
	hilightColour := blue
    end if
    var fontDescent : int

    %
    % The inheritable subprograms of TextFieldClass
    %
    procedure GetCharSpacing
	for count : 1 .. textLen + 1
	    if echoChar = chr (0) then
		xOffsets (count) :=
		    Font.Width (text (1 .. count - 1), textFontID)
	    else
		xOffsets (count) :=
		    Font.Width (repeat (echoChar, count - 1), textFontID)
	    end if
	end for
	xOffsets (textLen + 2) := xOffsets (textLen + 1) + charWidth
    end GetCharSpacing


    procedure SetXOffset (newXOffset : int)
	xOffset := max (0,
	    min (newXOffset, xOffsets (textLen + 1) - (x2 - x1)))
	if xOffset = 0 then
	    xOffset := -1
	end if
	for count : 1 .. textLen + 1
	    if xOffset <= xOffsets (count) then
		firstVisibleChar := count
		exit
	    end if
	end for
	if xOffset + (x2 - x1) >= xOffsets (textLen + 1) then
	    lastVisibleChar := textLen
	else
	    for count : 1 .. textLen + 1
		if xOffset + (x2 - x1) <= xOffsets (count) then
		    lastVisibleChar := count - 1
		    exit
		end if
	    end for
	end if
    end SetXOffset


    % DrawCursor draws or erases the current cursor/selection.  It should
    % not be called if the cursor/selection is already drawn or erased.
    % (i.e. call only to change state.)
    procedure DrawCursor
	% The cursor could be blinking in a non-selected text window!
	var prevWindow : int := -99
	if window not= Window.GetSelect then
	    prevWindow := Window.GetSelect
	    Window.Select (window)
	end if

	if selStart = selEnd then
	    var x := x1 + xOffsets (selStart) - xOffset
	    if cursorOn then
		if cursorPic not= -1 then
		    Pic.Free (cursorPic)
		end if
		cursorPic := Pic.New (x, y1 + 1, x, y2 - 1)
		Draw.Line (x, y1 + 1, x, y2 - 1, foregroundColour)
	    else
		Pic.Draw (cursorPic, x, y1 + 1, picCopy)
	    end if
	else
	    var cx1 := x1 + xOffsets (selStart) - xOffset
	    var cx2 := x1 + xOffsets (selEnd) - xOffset - 1
	    if cursorOn then
		View.ClipSet (x1, y1, x2, y2)
		Draw.FillBox (cx1, y1, cx2, y2, hilightColour)
		if colourbg = black and backgroundColour not= colourbg then
		    if echoChar = chr (0) then
			Font.Draw (text (selStart .. selEnd - 1), cx1,
			    y1 + fontDescent, textFontID, brightwhite)
		    else
			Font.Draw (repeat (echoChar, selEnd - selStart), cx1,
			    y1 + fontDescent, textFontID, brightwhite)
		    end if
		else
		    if echoChar = chr (0) then
			Font.Draw (text (selStart .. selEnd - 1), cx1,
			    y1 + fontDescent, textFontID, colourbg)
		    else
			Font.Draw (repeat (echoChar, selEnd - selStart), cx1,
			    y1 + fontDescent, textFontID, colourbg)
		    end if
		end if
		View.ClipOff
	    else
		View.ClipSet (x1, y1, x2, y2)
		if colourbg = black and backgroundColour not= colourbg then
		    Draw.FillBox (cx1, y1, cx2, y2, brightwhite)
		else
		    Draw.FillBox (cx1, y1, cx2, y2, colourbg)
		end if
		if echoChar = chr (0) then
		    Font.Draw (text (selStart .. selEnd - 1), cx1,
			y1 + fontDescent, textFontID, foregroundColour)
		else
		    Font.Draw (repeat (echoChar, selEnd - selStart), cx1,
			y1 + fontDescent, textFontID, foregroundColour)
		end if
		View.ClipOff
	    end if
	end if

	% If the cursor was blinking in a non-selected text window,
	% restore the selected window.
	if prevWindow not= -99 then
	    Window.Select (prevWindow)
	end if
    end DrawCursor


    procedure DrawText
	if colourbg = black and backgroundColour not= colourbg then
	    Draw.FillBox (x1, y1, x2, y2, brightwhite)
	else
	    Draw.FillBox (x1, y1, x2, y2, colourbg)
	end if
	View.ClipSet (x1, y1, x2, y2)
	if echoChar = chr (0) then
	    Font.Draw (text, x1 - xOffset, y1 + fontDescent, textFontID,
		foregroundColour)
	else
	    Font.Draw (repeat (echoChar, length (text)), x1 - xOffset,
		y1 + fontDescent, textFontID, foregroundColour)
	end if
	View.ClipOff
    end DrawText


    %
    % The package subprograms of GenericActiveWidgetClass
    %
    procedure ChangeText (newText : string, newPos : int)
	% Turn cursor off if necessary.
	var cursorWasOn : boolean := cursorOn
	if cursorOn then
	    cursorOn := false
	    DrawCursor
	end if

	text := newText
	textLen := length (text)
	GetCharSpacing

	selStart := newPos
	selEnd := newPos

	if xOffsets (selStart) < xOffset or
		xOffsets (selEnd) - xOffsets (selStart) > x2 - x1 then
	    SetXOffset (xOffsets (selStart))
	elsif xOffsets (selEnd) > xOffset + x2 - x1 then
	    SetXOffset (xOffsets (selEnd) - (x2 - x1))
	end if

	% Redraw the text if necessary.
	DrawText

	% Turn cursor back on if necessary.
	if cursorWasOn then
	    cursorOn := true
	    DrawCursor
	end if
    end ChangeText


    procedure MakeInactive
	if cursorOn then
	    cursorOn := false
	    DrawCursor
	end if

	active := false

	% If we're the blinking text field, stop it.
	if WidgetGlobals.blinkingTextField = self then
	    WidgetGlobals.blinkingTextField := nil
	end if
    end MakeInactive


    function CalculateCharPosition (mx : int) : int
	if mx < x1 then
	    var theDelay : int := delayBetweenScrolling *
		max (100 - (x1 - mx), 0) div 100
	    if leftEdgeTime + theDelay < Time.Elapsed then
		leftEdgeTime := Time.Elapsed
		result max (1, firstVisibleChar - 1)
	    else
		result firstVisibleChar
	    end if
	elsif mx > x2 then
	    var theDelay : int := delayBetweenScrolling *
		max (100 - (mx - x2), 0) div 100
	    if rightEdgeTime + theDelay < Time.Elapsed then
		rightEdgeTime := Time.Elapsed
		result min (textLen + 1, lastVisibleChar + 2)
	    else
		result lastVisibleChar + 1
	    end if
	else
	    var x : int := mx + xOffset - x1
	    for count : 1 .. textLen
		if x < (xOffsets (count) + xOffsets (count + 1)) div 2 then
		    result count
		end if
	    end for
	    result textLen + 1
	end if
    end CalculateCharPosition



    %
    % The external subprograms of TextFieldClass
    %
    procedure InitializeFull (newX, newY, newWidth : int, newText : string,
	    newActionProc : procedure x (text : string),
	    newBorder, fontID, newInputKind : int)

	active := true
	window := Window.GetSelect

	% Check widgets
	var widget : ^GenericWidgetBodyClass := WidgetGlobals.lastWidget
	loop
	    exit when widget = nil
	    if widget -> GetWindow = window and
		    objectclass (widget) >= TextFieldClass then
		active := false
	    end if
	    widget := widget -> GetPrev
	end loop

	text := newText
	textLen := length (text)
	textFontID := TranslateFont (fontID)
	border := TranslateBorder (newBorder)
	charWidth := Font.Width ("M", textFontID)
	SetPositionAndSize (newX, newY, newWidth, 0)
	if newInputKind = 0 then
	    inputKind := WidgetGlobals.ANY
	elsif newInputKind = WidgetGlobals.ANY or
		newInputKind = WidgetGlobals.INTEGER or
		newInputKind = WidgetGlobals.REAL then
	    inputKind := newInputKind
	else
	    AssertFailed ("Bad value of inputKind: " + intstr (inputKind))
	end if
	GetCharSpacing
	userActionProc := newActionProc
	selStart := 1
	selEnd := 1
	xOffset := -1
	firstVisibleChar := 1
	lastVisibleChar := 1

	if WidgetGlobals.blinkingTextField = nil then
	    WidgetGlobals.blinkingTextField := self
	end if

	GeneralInitialize
    end InitializeFull


    procedure Initialize (x, y, width : int, text : string,
	    newActionProc : procedure x (text : string))
	InitializeFull (x, y, width, text, newActionProc,
	    WidgetGlobals.LINE, WidgetGlobals.systemFontID,
	    WidgetGlobals.ANY)
    end Initialize


    function IsActive : boolean
	result active
    end IsActive


    procedure BlinkCursor
	assert active
	if selStart = selEnd then
	    if Time.Elapsed - lastCursorBlink > blinkSpeed then
		lastCursorBlink := Time.Elapsed
		cursorOn := not cursorOn
		DrawCursor
	    end if
	else
	    if not cursorOn then
		cursorOn := true
		DrawCursor
	    end if
	end if
    end BlinkCursor


    procedure RemoveCursor
	if cursorOn then
	    cursorOn := false
	    DrawCursor
	end if
    end RemoveCursor


    procedure SetActive
	% Deactivate currently active TextField for this window.
	var widget : ^GenericWidgetBodyClass := WidgetGlobals.firstWidget
	loop
	    exit when widget = nil
	    if widget -> GetWindow = window and
		    objectclass (widget) >= TextFieldClass and
		    TextFieldClass (widget).IsActive then

		% Check to see if we're already active!
		if widget = self then
		    return
		end if
		TextFieldClass (widget).MakeInactive
	    end if
	    widget := widget -> GetNext
	end loop

	active := true

	% If this text field is in the currently active window, then
	% theoretically we become the blinkingTextField.
	if window = Window.GetActive then
	    cursorOn := true
	    DrawCursor
	end if
    end SetActive


    procedure SetEchoChar (ch : char)
	echoChar := ch
    end SetEchoChar

    function GetText : string
	result text
    end GetText


    procedure SetText (newText : string)
	% Turn cursor off if necessary.
	var cursorWasOn : boolean := cursorOn
	if cursorOn then
	    cursorOn := false
	    DrawCursor
	end if

	selStart := 1
	selEnd := 1
	SetXOffset (0)
	text := newText
	textLen := length (text)
	GetCharSpacing
	DrawText

	% Turn cursor back on if necessary.
	if cursorWasOn then
	    cursorOn := true
	    DrawCursor
	end if
    end SetText


    procedure SetSelection (start, finish : int)
	if start = selStart and finish = selEnd then
	    return
	end if

	% Turn cursor off if necessary.
	var cursorWasOn : boolean := cursorOn
	if active and cursorOn then
	    cursorOn := false
	    DrawCursor
	end if

	var oldXOffset : int := xOffset
	if start = -1 and finish = -1 then
	    selStart := 1
	    selEnd := textLen + 1
	    SetXOffset (0)
	else
	    if start < 1 then
		selStart := 1
	    elsif start > textLen then
		selStart := textLen + 1
	    else
		selStart := start
	    end if
	    if finish < 1 then
		selEnd := 1
	    elsif finish > textLen then
		selEnd := textLen + 1
	    else
		selEnd := finish
	    end if
	    if selStart > selEnd then
		var tempSel := selEnd
		selEnd := selStart
		selStart := tempSel
	    end if
	    if xOffsets (selStart) < xOffset or
		    xOffsets (selEnd) - xOffsets (selStart) > x2 - x1 then
		SetXOffset (xOffsets (selStart))
	    elsif xOffsets (selEnd) > xOffset + x2 - x1 then
		SetXOffset (xOffsets (selEnd) - (x2 - x1))
	    end if
	end if

	% Redraw the text if necessary.
	if xOffset not= oldXOffset then
	    DrawText
	end if

	% Turn cursor on if necessary.
	if active then
	    cursorOn := true
	    lastCursorBlink := Time.Elapsed
	    DrawCursor
	end if
    end SetSelection


    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	var theight, ascent, internalLeading : int

	Font.Sizes (textFontID, theight, ascent, fontDescent,
	    internalLeading)

	#if _DOS_ then
	    theight -= 3
	#end if

	if border = WidgetGlobals.LINE then
	    x := originalX - 1
	    y := originalY - 1
	    width := originalWidth + 2
	    height := theight + 4
	elsif border = WidgetGlobals.INDENT then
	    x := originalX - riserSize
	    y := originalY - riserSize - 1
	    width := originalWidth + 2 * riserSize + 1
	    height := theight + 2 * riserSize + 3
	else
	    x := originalX - riserSize - 1
	    y := originalY - riserSize
	    width := originalWidth + 2 * riserSize + 1
	    height := theight + 2 * riserSize + 3
	end if
	x1 := originalX
	y1 := originalY
	x2 := originalX + originalWidth - 1
	y2 := originalY + theight + 1
	SetXOffset (xOffset)
	drawn := true
    end ComputeWidgetPosition


    body procedure Dispose
	if active then
	    MakeInactive
	end if
	GenericActiveWidgetClass.Dispose
    end Dispose


    body procedure Hide
	if active then
	    MakeInactive
	end if
	GenericActiveWidgetClass.Hide
    end Hide


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if

	% Draw the border.
	if border = WidgetGlobals.LINE then
	    Draw.Box (x, y, x + width - 1, y + height - 1, foregroundColour)
	else
	    var topLeftColour, bottomRightColour : int
	    if border = WidgetGlobals.INDENT then
		topLeftColour := darkgrey
		bottomRightColour := illuminatedColor
	    else
		topLeftColour := illuminatedColor
		bottomRightColour := darkgrey
	    end if
	    var x2 : int := x + width - 1
	    var y2 : int := y + height - 1
	    for offset : 0 .. riserSize - 1
		Draw.Line (x + offset, y + offset, x2 - offset,
		    y + offset, bottomRightColour)
		Draw.Line (x2 - offset, y + offset, x2 - offset,
		    y2 - offset, bottomRightColour)
		Draw.Line (x + offset, y + offset, x + offset,
		    y2 - offset, topLeftColour)
		Draw.Line (x + offset, y2 - offset, x2 - offset,
		    y2 - offset, topLeftColour)
	    end for
	    if border = WidgetGlobals.INDENT then
		Draw.Line (x + riserSize, y + riserSize, x2 - riserSize,
		    y + riserSize, grey)
		Draw.Line (x2 - riserSize, y + riserSize, x2 - riserSize,
		    y2 - riserSize, grey)
	    elsif border = WidgetGlobals.EXDENT then
		Draw.Line (x + riserSize, y + riserSize, x + riserSize,
		    y2 - riserSize, grey)
		Draw.Line (x + riserSize, y2 - riserSize, x2 - riserSize,
		    y2 - riserSize, grey)
	    end if
	end if

	DrawText

	if cursorOn then
	    DrawCursor
	end if
    end DrawWidget


    body procedure ActionProc
	userActionProc (text)
    end ActionProc


    body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	assert initialized
	if not enabled or not showing then
	    result false
	end if
	var mx, my, b : int
	if x <= mouseX and mouseX < x + width - 1 and
		y <= mouseY and mouseY < y + height - 1 then
	    SetActive
	    var pos : int := CalculateCharPosition (mouseX)
	    SetSelection (pos, pos)
	    var lastPos := -1
	    loop
		Mouse.Where (mx, my, b)
		exit when b = 0
		var newPos := CalculateCharPosition (mx)
		if newPos not= lastPos then
		    lastPos := newPos

		    % Turn cursor off if necessary.
		    if cursorOn then
			cursorOn := false
			DrawCursor
		    end if

		    selStart := min (pos, newPos)
		    selEnd := max (pos, newPos)

		    var oldXOffset := xOffset
		    if newPos < firstVisibleChar then
			SetXOffset (xOffsets (newPos))
		    elsif newPos > lastVisibleChar + 1 and
			    lastVisibleChar not= textLen then
			SetXOffset (xOffsets (newPos) - (x2 - x1))
		    end if

		    % Redraw the text if necessary.
		    if xOffset not= oldXOffset then
			oldXOffset := xOffset
			DrawText
		    end if

		    % Turn cursor on if necessary.
		    cursorOn := true
		    lastCursorBlink := Time.Elapsed
		    DrawCursor
		end if
	    end loop
	    result true
	else
	    result false
	end if
    end ConsiderButtonDown


    body function ConsiderKeystroke (key : char) : boolean
	assert initialized
	if enabled and showing and active then
	    if key = rightArrow then
		SetSelection (selEnd + 1, selEnd + 1)
	    elsif key = leftArrow then
		SetSelection (max (selStart - 1, 1), selStart - 1)
	    elsif key = home then
		SetSelection (1, 1)
	    elsif key = endKey then
		SetSelection (9999, 9999)
	    elsif key = backspace then
		if selStart = selEnd then
		    if selStart > 1 then
			ChangeText (text (1 .. selStart - 2) +
			    text (selEnd .. *), selStart - 1)
		    end if
		else
		    ChangeText (text (1 .. selStart - 1) +
			text (selEnd .. *), selStart)
		end if
	    elsif key = delete then
		if selStart = selEnd then
		    if selStart not= textLen + 1 then
			ChangeText (text (1 .. selStart - 1) +
			    text (selEnd + 1 .. *), selStart)
		    end if
		else
		    ChangeText (text (1 .. selStart - 1) +
			text (selEnd .. *), selStart)
		end if
	    elsif ' ' <= key and key <= '~' and textLen < 255 then
		if selStart = selEnd then
		    ChangeText (text (1 .. selStart - 1) + key +
			text (selEnd .. *), selStart + 1)
		else
		    ChangeText (text (1 .. selStart - 1) + key +
			text (selEnd .. *), selStart + 1)
		end if
	    elsif key = '\t' then
		% Check widgets
		var widget : ^GenericWidgetBodyClass := nextWidget
		loop
		    exit when widget = nil
		    if widget -> GetWindow = window and
			    objectclass (widget) >= TextFieldClass then
			TextFieldClass (widget).SetActive
			result true
		    end if
		    widget := widget -> GetNext
		end loop

		widget := WidgetGlobals.firstWidget
		loop
		    exit when widget = self
		    if widget -> GetWindow = window and
			    objectclass (widget) >= TextFieldClass then
			TextFieldClass (widget).SetActive
			result true
		    end if
		    widget := widget -> GetNext
		end loop
	    elsif key = '\n' then
		GeneralActionProc
	    end if
	    result true
	else
	    result false
	end if
    end ConsiderKeystroke
end TextFieldClass

