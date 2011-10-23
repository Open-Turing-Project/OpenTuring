unit
module WidgetModule
    import GenericWidgetClass,
	GenericWidgetBodyClass,
	GenericActiveWidgetClass,
	TextFieldClass,
	WidgetGlobals,
	MenuClass,
	MenuItemClass,
	MenuBarClass

    export GetSelectedWidget, GetSelectedUserValue, Refresh,
	SetBackgroundColour, SetBackgroundColor, SetNullEventHandler,
	SetKeyEventHandler, SetMouseEventHandler, ProcessEvent,
	GetEventWidget, GetEventWindow, GetEventTime,
	SetDisplayWhenInitialized, CloseWindow, ShowWindow, HideWindow,
	GetMenuBar, GetScrollBarWidth, Quit, ResetQuit, GetMenuBarHeight,
	GetBackgroundColour, GetBackgroundColor, GetVersion,
	SetDelayInProcess

    forward procedure DefaultNullEventHandler
    forward procedure DefaultKeyEventHandler (key : char)
    forward procedure DefaultMouseEventHandler (mx, my : int)

    % This is what Window.GetActive returns if a non-OOT Run Window is active.
    const nonOOTRunWindow : int := -5

    const WIDGET_LIB_VERSION := 110

    % User event handlers
    var nullEventHandler : procedure x () := DefaultNullEventHandler
    var keyEventHandler : procedure x (key : char) := DefaultKeyEventHandler
    var mouseEventHandler : procedure x (mx, my : int) :=
	DefaultMouseEventHandler

    % The event globals
    var theEventTime, theEventWindow : int := 0

    % The current blinking text field
    var blinkingTextField : ^TextFieldClass := nil
    var lastActiveWindow : int := 0

    % Whether WidgetModule.Quit has been called
    var quitting : boolean := false

    % Whether ProcessEbent should have a small delay
    var delayInProcess : boolean := true

    body procedure DefaultNullEventHandler
    end DefaultNullEventHandler


    body procedure DefaultKeyEventHandler (key : char)
    end DefaultKeyEventHandler


    body procedure DefaultMouseEventHandler (mx, my : int)
    end DefaultMouseEventHandler


    function GetSelectedWidget : ^GenericWidgetClass
	result WidgetGlobals.selectedWidget
    end GetSelectedWidget


    function GetSelectedUserValue : int
	result
	    GenericWidgetBodyClass (WidgetGlobals.selectedWidget).GetUserValue
    end GetSelectedUserValue


    function GetMenuBarForWindow (windowID : int) : ^MenuBarClass
	for cnt : 1 .. WidgetGlobals.numWindows
	    if windowID = WidgetGlobals.window (cnt).windowID then
		result WidgetGlobals.window (cnt).menuBar
	    end if
	end for

	result nil
    end GetMenuBarForWindow


    % Window must already be set up
    procedure HandleKeystroke (window : int)
	var ch := getchar

	% Check if a menu shortcut
	var menubar : ^MenuBarClass := GetMenuBarForWindow (window)

	% Check menus
	if menubar not= nil then
	    if menubar -> ConsiderKeystroke (ch) then
		% Was a menu shortcut.
		return
	    end if
	end if

	% Check widgets
	var widget : ^GenericWidgetBodyClass := WidgetGlobals.firstWidget
	loop
	    exit when widget = nil
	    if widget -> GetWindow = window and
		    objectclass (widget) >= GenericActiveWidgetClass and
		    GenericActiveWidgetClass (widget).ConsiderKeystroke (ch)
		    then
		% Was a widget shortcut.
		return
	    end if
	    widget := widget -> GetNext
	end loop

	% Check user keystroke handler
	if keyEventHandler not= DefaultKeyEventHandler then
	    keyEventHandler (ch)
	end if
    end HandleKeystroke


    procedure HandleMouseDown (window : int)
	var x, y, dummy1, dummy2 : int
	Mouse.ButtonWait ("down", x, y, dummy1, dummy2)

	% Check if a click in the menu
	var menubar : ^MenuBarClass := GetMenuBarForWindow (window)
	if menubar not= nil then
	    if menubar -> ConsiderButtonDown (x, y) then
		% Clicked on a menu.
		return
	    end if
	end if

	% Check widgets
	var widget : ^GenericWidgetBodyClass := WidgetGlobals.firstWidget
	loop
	    exit when widget = nil
	    if widget -> GetWindow = window and
		    objectclass (widget) >= GenericActiveWidgetClass and
		    not (objectclass (widget) >= MenuClass) and
		    not (objectclass (widget) >= MenuItemClass) and
		    GenericActiveWidgetClass (widget).ConsiderButtonDown (x, y)
		    then
		% Was a widget shortcut.
		return
	    end if
	    widget := widget -> GetNext
	end loop

	% Check user mouse handler
	if mouseEventHandler not= DefaultMouseEventHandler then
	    mouseEventHandler (x, y)
	end if
    end HandleMouseDown


    % Returns false if OOT generated an error when attempting to switch
    % to the window.
    function SelectWindow (window : int) : boolean
	handler (reason)
	    if reason = 10000 then
		WidgetGlobals.RemoveWindowFromArray (window)
		result false
	    else
		quit >
	    end if
	end handler
	Window.Select (window)
	result true
    end SelectWindow


    % Set GUI.ProcessEvent to quit when WidgetModule.Quit is called.
    procedure Quit
	quitting := true
    end Quit


    % Set GUI.ProcessEvent to quit when WidgetModule.Quit is called.
    procedure ResetQuit
	quitting := false
    end ResetQuit


    procedure SetDelayInProcess (newDelayInProcess : boolean)
	delayInProcess := newDelayInProcess
    end SetDelayInProcess


    function GetMenuBarHeight : int
	result WidgetGlobals.MENU_BAR_HEIGHT + 1
    end GetMenuBarHeight


    function GetBackgroundColour : int
	result backgroundColour
    end GetBackgroundColour


    function GetBackgroundColor : int
	result backgroundColour
    end GetBackgroundColor

    % Check for an event in the window.  Return true if an event
    % occurred in the window.
    function CheckForEvent (window : int) : boolean
	var prevWindow := Window.GetSelect
	theEventWindow := window

	% Don't look for events from non OOT Run Windows.
	if window = nonOOTRunWindow then
	    result false
	end if

	% This may trigger the handler if the run window no longer exists.
	if not SelectWindow (window) then
	    result false
	end if

	if hasch then
	    HandleKeystroke (window)
	    theEventWindow := 0
	    if Window.GetSelect = window then
		Window.Select (prevWindow)
	    end if
	    result true
	elsif Mouse.ButtonMoved ("down") then
	    HandleMouseDown (window)
	    theEventWindow := 0
	    if Window.GetSelect = window then
		Window.Select (prevWindow)
	    end if
	    result true
	else
	    theEventWindow := 0
	    Window.Select (prevWindow)
	    result false
	end if
    end CheckForEvent


    % If the active window has changed, start the cursor blinking in the
    % new active window.
    procedure CheckForDifferentBlinkingTextField
	var activeWindow : int := Window.GetActive
	const blinkingTextField : ^TextFieldClass :=
	    WidgetGlobals.blinkingTextField

	% We check to see if we need to select a new blinking text field.
	% we do if (1) the active window has changed or (2) the current
	% blinking text field is no longer active or (3) the current
	% blinking text field is not in the active window.
	if activeWindow not= lastActiveWindow or
		(blinkingTextField not= nil and
		(not blinkingTextField -> IsActive or
		blinkingTextField -> GetWindow not= activeWindow)) then
	    if blinkingTextField not= nil then
		blinkingTextField -> RemoveCursor
	    end if
	    % Find the active field for the new window.
	    WidgetGlobals.blinkingTextField := nil
	    lastActiveWindow := activeWindow

	    % There's no blinking text field if this isn't an OOT Run window.
	    if activeWindow = nonOOTRunWindow then
		return
	    end if

	    var widget : ^GenericWidgetBodyClass := WidgetGlobals.firstWidget
	    loop
		exit when widget = nil
		if widget -> GetWindow = activeWindow and
			objectclass (widget) >= TextFieldClass and
			TextFieldClass (widget).IsActive then
		    WidgetGlobals.blinkingTextField := widget
		    TextFieldClass (widget).BlinkCursor
		    exit
		end if
		widget := widget -> GetNext
	    end loop
	end if
    end CheckForDifferentBlinkingTextField


    % Process a single event
    function ProcessEvent : boolean
	const activeWin : int := Window.GetActive
	theEventTime := Time.Elapsed
	CheckForDifferentBlinkingTextField

	if activeWin not= -5 and Window.IsVisible (activeWin) and
		CheckForEvent (activeWin) then
	    result quitting
	else
	    for count : 1 .. WidgetGlobals.numWindows
		var winID : int := WidgetGlobals.window (count).windowID
		if winID not= activeWin and Window.IsVisible (winID) then
		    if CheckForEvent (winID) then
			result quitting
		    end if
		end if
	    end for
	end if
	% Blink cursor if necessary.
	if WidgetGlobals.blinkingTextField not= nil then
	    TextFieldClass (WidgetGlobals.blinkingTextField).BlinkCursor
	end if

	if nullEventHandler not= DefaultNullEventHandler then
	    nullEventHandler
	end if

	theEventTime := 0

	if delayInProcess then
	    delay (3)
	end if

	result quitting
    end ProcessEvent


    % Refresh all the widget's in the currently selected window
    procedure Refresh
	var window : int := Window.GetSelect
	var widget : ^GenericWidgetBodyClass := WidgetGlobals.firstWidget
	loop
	    exit when widget = nil
	    if widget -> GetWindow = window then
		widget -> Refresh
	    end if
	    widget := widget -> GetNext
	end loop
    end Refresh


    procedure SetBackgroundColour (clr : int)
	% There is a problem with the DOS vs Mac/Windows palette.
	% In DOS, grey is set to R=0.33, G=0.33, B=0.33, while in
	% Windows, grey is R=0.67, G=0.67, B=0.67.  Anyway, to
	% compensate for this, if we are in a standard DOS palette
	% (colourbg = black) and the background colour is to be set
	% to grey, we substitute while (which under DOS is defined
	% R=0.67, G=0.67, B=0.67).
	if colourbg = black and clr = grey then
	    backgroundColour := white
	    Draw.FillBox (0, 0, maxx, maxy, white)
	else
	    backgroundColour := clr
	    Draw.FillBox (0, 0, maxx, maxy, clr)
	end if
	Refresh
    end SetBackgroundColour


    procedure SetBackgroundColor (clr : int)
	SetBackgroundColour (clr)
    end SetBackgroundColor


    procedure SetNullEventHandler (actionProc : procedure x ())
	nullEventHandler := actionProc
    end SetNullEventHandler


    procedure SetKeyEventHandler (actionProc : procedure x (key : char))
	keyEventHandler := actionProc
    end SetKeyEventHandler


    procedure SetMouseEventHandler (actionProc : procedure x (mx, my : int))
	mouseEventHandler := actionProc
    end SetMouseEventHandler


    function GetEventTime : int
	result theEventTime
    end GetEventTime


    function GetEventWindow : int
	result theEventWindow
    end GetEventWindow


    function GetEventWidget : ^GenericWidgetClass
	result WidgetGlobals.selectedWidget
    end GetEventWidget


    procedure SetDisplayWhenInitialized (display : boolean)
	WidgetGlobals.SetDisplayWhenInitialized (display)
    end SetDisplayWhenInitialized


    procedure CloseWindow (window : int)
	% Dispose of all the widgets in the window.
	var widget : ^GenericWidgetBodyClass := WidgetGlobals.firstWidget
	var next : ^GenericWidgetClass
	loop
	    exit when widget = nil
	    next := widget -> GetNext
	    if widget -> GetWindow = window then
		widget -> Dispose
	    end if
	    widget := next
	end loop

	WidgetGlobals.RemoveWindowFromArray (window)
	Window.Close (window)
    end CloseWindow


    procedure ShowWindow (window : int)
	WidgetGlobals.AddWindowToArray (window)
	Window.Show (window)
    end ShowWindow


    procedure HideWindow (window : int)
	WidgetGlobals.RemoveWindowFromArray (window)
	Window.Hide (window)
    end HideWindow


    % Get the nenu bar if there is one, otherwise create it.
    function GetMenuBar (windowID : int) : ^MenuBarClass
	var menuBar : ^MenuBarClass
	var windowElement : int := 0

	for cnt : 1 .. WidgetGlobals.numWindows
	    if windowID = WidgetGlobals.window (cnt).windowID then
		menuBar := WidgetGlobals.window (cnt).menuBar
		if menuBar = nil then
		    new menuBar
		    WidgetGlobals.window (cnt).menuBar := menuBar
		end if
		result menuBar
	    end if
	end for

	WidgetGlobals.AddWindowToArray (windowID)
	new menuBar
	WidgetGlobals.window (WidgetGlobals.numWindows).menuBar := menuBar
	result menuBar
    end GetMenuBar


    function GetScrollBarWidth : int
	result WidgetGlobals.SCROLL_BAR_WIDTH
    end GetScrollBarWidth


    function GetVersion : int
	result WIDGET_LIB_VERSION
    end GetVersion
end WidgetModule

