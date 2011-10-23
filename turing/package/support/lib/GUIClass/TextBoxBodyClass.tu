unit

class TextBoxBodyClass
    implement TextBoxClass

    import WidgetGlobals, TextBoxModule,
	HorizontalScrollBarClass, VerticalScrollBarClass

    type lineRec :
	record
	    text : string
	    next : ^lineRec
	end record

    var firstLine, lastLine : ^lineRec := nil
    var continueLine : boolean := false
    var hScroll : ^HorizontalScrollBarClass
    var vScroll : ^VerticalScrollBarClass
    var x1, y1, x2, y2 : int
    var numLines, screenLines : int
    var maxWidth : int
    var border : int
    var textFontID : int
    var baseLine : int
    var lineHeight, lineDescent : int
    % Scroll to end of textbox when text added
    var scrollOnAdd : boolean := true
    % Action when user selects a line
    var userActionProc : procedure actionProc (line : int)
    % Can the user use this text box to select
    var selectable : boolean
    % The currently selected line
    var selectedLine : int := 3

    procedure DrawText
	if colourbg = black and backgroundColour not= colourbg then
	    Draw.FillBox (x1, y1, x2, y2, brightwhite)
	else
	    Draw.FillBox (x1, y1, x2, y2, colourbg)
	end if
	const topLine : int := vScroll -> GetSliderValue
	const xOffset := hScroll -> GetSliderValue
	var linePtr : ^lineRec := firstLine
	for cnt : 1 .. topLine - 1
	    exit when linePtr = nil
	    linePtr := linePtr -> next
	end for
	var textY := y2 - lineHeight + lineDescent
	View.ClipSet (x1, y1, x2, y2)
	for cnt : 1 .. screenLines
	    exit when linePtr = nil
	    if cnt + topLine - 1 = selectedLine then
		Draw.FillBox (x1, textY - lineDescent, x2,
		    textY + lineHeight - lineDescent, blue)
		Font.Draw (linePtr -> text, x1 - xOffset + 4, textY, textFontID,
		    white)
	    else
		Font.Draw (linePtr -> text, x1 - xOffset + 4, textY, textFontID,
		    black)
	    end if
	    linePtr := linePtr -> next
	    textY -= lineHeight
	end for
	View.ClipOff
    end DrawText


    procedure DoAdd (text : string)
	if continueLine and
		length (text) + length (lastLine -> text) < 256 then
	    lastLine -> text := lastLine -> text + text
	else
	    var newLine : ^lineRec
	    new newLine
	    newLine -> text := text
	    % Covert tabs
	    var ptr := 1
	    loop
		exit when ptr > length (newLine -> text)
		if newLine -> text (ptr) = '\t' then
		    newLine -> text :=
			newLine -> text (1 .. ptr - 1) +
			repeat (" ", 9 - (ptr mod 8)) +
			newLine -> text (ptr + 1 .. *)
		end if
		ptr += 1
	    end loop
	    newLine -> next := nil

	    if lastLine = nil then
		firstLine := newLine
	    else
		lastLine -> next := newLine
	    end if
	    lastLine := newLine
	    numLines += 1
	    if numLines > screenLines then
		vScroll -> SetMinMax (1, numLines + 2)
	    else
		vScroll -> SetMinMax (1, numLines)
	    end if
	end if

	const width := Font.Width (lastLine -> text, textFontID)
	if width > maxWidth then
	    maxWidth := width
	    if maxWidth > x2 - x1 - 8 then
		hScroll -> SetMinMax (0, maxWidth + 20)
	    else
		hScroll -> SetMinMax (0, maxWidth)
	    end if
	end if

	% If scrollOnAdd is true, the scroll to the bottom of the text box
	if scrollOnAdd then
	    vScroll -> SetSliderValue (numLines - screenLines + 1)
	end if

	% If the text added is all off the bottom of the text box,
	% don't bother redrawing.
	if showing and
		numLines <= vScroll -> GetSliderValue + screenLines then
	    DrawText
	end if
    end DoAdd


    procedure DoClear
	firstLine := nil
	lastLine := nil
	hScroll -> SetMinMax (1, 1)
	vScroll -> SetMinMax (0, 0)
	numLines := 0
	maxWidth := 0
	continueLine := false
	selectedLine := 0
    end DoClear


    %
    % The external subprograms of TextBoxClass
    %
    body procedure InitializeChoice (x, y, width, height, newBorder,
	    fontID : int, newActionProc : procedure x (line : int))
	    InitializeFull (x, y, width, height, newBorder, fontID)
	    userActionProc := newActionProc
	    selectable := true
	end InitializeChoice


	body procedure InitializeFull (x, y, width, height : int,
		newBorder, fontID : int)
	    SetPositionAndSize (x, y, width, height)
	    border := TranslateBorder (newBorder)
	    textFontID := TranslateFont (fontID)
	    userActionProc := TextBoxModule.DoNothing
	    selectable := false
	    var prevShow := WidgetGlobals.showWhenInitialized
	    WidgetGlobals.showWhenInitialized := false
	    new hScroll
	    hScroll -> Initialize (100, 100, 100, 0, 100, 0,
		TextBoxModule.HScrollProc)
	    hScroll -> SetUserObject (self)
	    new vScroll
	    vScroll -> Initialize (100, 100, 100, 0, 100, 0,
		TextBoxModule.VScrollProc)
	    vScroll -> SetUserObject (self)
	    vScroll -> SetSliderReverse
	    WidgetGlobals.showWhenInitialized := prevShow

	    DoClear

	    GeneralInitialize
	end InitializeFull


	body procedure Initialize (x, y, width, height : int)
	    InitializeFull (x, y, width, height, 0, 0)
	end Initialize


	body procedure ClearText
	    % Free the line data base
	    var linePtr : ^lineRec := firstLine
	    var nextPtr : ^lineRec
	    loop
		exit when linePtr = nil
		nextPtr := linePtr -> next
		free linePtr
		linePtr := nextPtr
	    end loop

	    DoClear

	    if showing then
		DrawText
	    end if
	end ClearText


	body procedure AddLine (text : string)
	    DoAdd (text)
	    continueLine := false
	end AddLine


	body procedure AddText (text : string)
	    DoAdd (text)
	    continueLine := true
	end AddText


	body procedure SetTopLine (lineNumber : int)
	    vScroll -> SetSliderValue (lineNumber)
	end SetTopLine


	body procedure SetScrollOnAdd (newScrollOnAdd : boolean)
	    scrollOnAdd := newScrollOnAdd
	end SetScrollOnAdd


	body procedure HScrollProc (value : int)
	    if showing then
		DrawText
	    end if
	end HScrollProc


	body procedure VScrollProc (value : int)
	    if showing then
		DrawText
	    end if
	end VScrollProc


	%
	% Overridden subprograms
	%
	body procedure ComputeWidgetPosition
	    if border = WidgetGlobals.LINE then
		x := originalX - 1
		y := originalY - 1
		width := originalWidth + 2
		height := originalHeight + 2
	    elsif border = WidgetGlobals.INDENT then
		x := originalX - riserSize
		y := originalY - riserSize - 1
		width := originalWidth + 2 * riserSize + 1
		height := originalHeight + 2 * riserSize + 1
	    else
		x := originalX - riserSize - 1
		y := originalY - riserSize
		width := originalWidth + 2 * riserSize + 1
		height := originalHeight + 2 * riserSize + 1
	    end if

	    var charWidth, dummy1, dummy2 : int
	    charWidth := Font.Width ("M", textFontID)
	    Font.Sizes (textFontID, lineHeight, dummy1, lineDescent, dummy2)
	    #if _DOS_ then
		lineHeight += 4
	    #end if

	    x1 := originalX
	    y1 := originalY + WidgetGlobals.SCROLL_BAR_WIDTH + 1
	    x2 := x1 + originalWidth - WidgetGlobals.SCROLL_BAR_WIDTH - 2
	    y2 := y1 + originalHeight - WidgetGlobals.SCROLL_BAR_WIDTH - 2

	    screenLines := (y2 - y1) div lineHeight

	    hScroll -> SetPosition (originalX, originalY)
	    hScroll -> SetSliderSize (originalWidth -
		WidgetGlobals.SCROLL_BAR_WIDTH)
	    hScroll -> SetScrollAmount (1, x2 - x1 - 10, x2 - x1)
	    vScroll -> SetPosition (originalX + originalWidth -
		WidgetGlobals.SCROLL_BAR_WIDTH - 1,
		originalY + WidgetGlobals.SCROLL_BAR_WIDTH)
	    vScroll -> SetSliderSize (originalHeight -
		WidgetGlobals.SCROLL_BAR_WIDTH)
	    vScroll -> SetScrollAmount (1, screenLines - 1, screenLines)
	end ComputeWidgetPosition


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
	    hScroll -> Show
	    vScroll -> Show
	    DrawText
	end DrawWidget


	body procedure EraseWidget
	    if not drawn then
		ComputeWidgetPosition
	    end if

	    hScroll -> Hide
	    vScroll -> Hide

	    Draw.FillBox (x, y, x + width - 1, y + height - 1, backgroundColour)
	end EraseWidget

	body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	    if not selectable then
		result false
	    end if
	    if mouseX < x1 or x2 < mouseX or
		    mouseY < y1 or y2 < mouseY then
		result false
	    end if
	    var line : int := (y2 - mouseY) div lineHeight +
		vScroll -> GetSliderValue
	    if line > numLines then
		result false
	    end if
	    if line = selectedLine then
		userActionProc (line)
		result true
	    end if
	    selectedLine := line
	    DrawText
	    result true
	end ConsiderButtonDown
    end TextBoxBodyClass

