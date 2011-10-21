unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericWidgetBodyClass - Body that implements WidgetClass
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of WidgetClass is as follows:
%   procedure Show - Display the widget.
%   procedure Hide - Hide the widget.
%   procedure Dispose - Hide the widget and free any data structures allocated
%                       by the class.  To be called before the Widget is freed.
%   procedure SetPosition (x, y : int) - Move the widget to (x, y).
%   procedure SetSize (width, height : int) - Resize the widget.
%   procedure SetPositionAndSize (x, y, width, height : int) - Move and resize
%                                                              the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of WidgetClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of WidgetClass are as follows:
%   originalX, originalY - The x, y values specified by the user.
%   originalWidth, originalHeight - The width, height values specified by the
%                                   user.
%   x, y - The location of the lower left corner of the widget.
%   width, height - The width and height of the widget.
%   showing - Is the widget visible.
%   initialized - Has the widget been initialized.
%   drawn - The widget has been drawn at least once.  Used to calculate the
%           position for drawing.
%   window - The window the widget is in.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of WidgetClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
%   procedure DrawWidget - Draw the widget.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericWidgetBodyClass
    implement GenericWidgetClass

    import WidgetGlobals

    export SetNext, SetPrev, GetNext, GetPrev, GetWindow, GeneralInitialize

    deferred procedure GeneralInitialize

    %
    % Inheritable subprograms
    %
    deferred procedure DrawWidget
    deferred procedure EraseWidget


    %
    % The inheritable variables of GenericWidgetClass
    %
    % The x, y values specified by the user.
    var originalX, originalY : int
    % The width, height values specified by the user.
    var originalWidth, originalHeight : int
    % The lower left corner of the widget.
    var x, y : int
    % The width and height of the widget.
    var width, height : int
    % Is the widget visible.
    var showing : boolean := false
    % Has widget been initialized.
    var initialized : boolean := false
    % Has the widget has been drawn at least once.  Used to calculate the
    % position for drawing.
    var drawn : boolean := false
    % The window that the widget is in
    var window : int
    % The main colour the widget should be drawn in (usually colourfg)
    var foregroundColour : int
    % A user value that can be used as desired.
    var userValue : int
    % A user object that can be used as desired
    var userObject : ^anyclass
    % The previous and next widget in the linked list of widgets
    var prevWidget, nextWidget : ^GenericWidgetClass
    % The colour of the top of a raised button.
    var buttonColor : int
    % The colour of the hilight on the raised button.
    var illuminatedColor : int
    % The colour of the shadow on the raised button.
    var shadowColor : int

    %
    % The inheritable constants of GenericWidgetClass
    %
    % The size in pixels of the border on a raised button.
    const riserSize : int := 2


    %
    % The inheritable subprograms of GenericWidgetClass
    %
    procedure AssertFailed (msg : string)
	Window.Select (defWinId)
	locate (1, 1)
	put msg
	assert false
    end AssertFailed


    deferred procedure ComputeWidgetPosition


    procedure DrawPressedBorder (default : boolean)
	var x1, y1, x2, y2 : int
	if default then
	    x1 := x + 1
	    y1 := y + 1
	    x2 := x + width - 2
	    y2 := y + height - 2
	else
	    x1 := x
	    y1 := y
	    x2 := x + width - 1
	    y2 := y + height - 1
	end if
	const rise : int := riserSize + 1

	if default then
	    if backgroundColour = black then
		Draw.Box (x1 - 1, y1 - 1, x2 + 1, y2 + 1, foregroundColour)
	    else
		Draw.Box (x1 - 1, y1 - 1, x2 + 1, y2 + 1, foregroundColour)
		Draw.Box (x1, y1, x2, y2, foregroundColour)
	    end if
	else
	    if backgroundColour not= black then
		Draw.Box (x1, y1, x2, y2, foregroundColour)
	    end if
	end if

	Draw.FillBox (x1 + 1, y1 + 1, x2 - 1, y2 - 1, buttonColor)

	for offset : 1 .. rise - 1
	    Draw.Line (x1 + offset, y1 + offset, x1 + offset, y2 - offset,
		shadowColor)
	    Draw.Line (x1 + offset, y2 - offset, x2 - offset, y2 - offset,
		shadowColor)
	end for
    end DrawPressedBorder


    procedure DrawUnpressedBorder (default : boolean)
	var x1, y1, x2, y2 : int
	if default then
	    x1 := x + 1
	    y1 := y + 1
	    x2 := x + width - 2
	    y2 := y + height - 2
	else
	    x1 := x
	    y1 := y
	    x2 := x + width - 1
	    y2 := y + height - 1
	end if
	const rise : int := riserSize + 1

	if default then
	    if backgroundColour = black then
		Draw.Box (x1 - 1, y1 - 1, x2 + 1, y2 + 1, foregroundColour)
	    else
		Draw.Box (x1 - 1, y1 - 1, x2 + 1, y2 + 1, foregroundColour)
		Draw.Box (x1, y1, x2, y2, foregroundColour)
	    end if
	else
	    if backgroundColour not= black then
		Draw.Box (x1, y1, x2, y2, foregroundColour)
	    end if
	end if

	for offset : 1 .. riserSize
	    Draw.Line (x1 + offset, y1 + offset, x2 - offset, y1 + offset,
		shadowColor)
	    Draw.Line (x2 - offset, y1 + offset, x2 - offset, y2 - offset,
		shadowColor)
	    Draw.Line (x1 + offset, y1 + offset, x1 + offset, y2 - offset,
		illuminatedColor)
	    Draw.Line (x1 + offset, y2 - offset, x2 - offset, y2 - offset,
		illuminatedColor)
	end for
	Draw.FillBox (x1 + rise, y1 + rise, x2 - rise, y2 - rise,
	    buttonColor)
    end DrawUnpressedBorder


    body procedure EraseWidget
	if not initialized then
	    AssertFailed ("The object has not been initialized yet")
	else
	    if not drawn then
		ComputeWidgetPosition
	    end if
	    Draw.FillBox (x, y, x + width - 1, y + height - 1,
		backgroundColour)
	end if
    end EraseWidget


    function GetNext : ^GenericWidgetClass
	result nextWidget
    end GetNext


    function GetPrev : ^GenericWidgetClass
	result prevWidget
    end GetPrev


    procedure SetNext (widget : ^GenericWidgetClass)
	nextWidget := widget
    end SetNext


    procedure SetPrev (widget : ^GenericWidgetClass)
	prevWidget := widget
    end SetPrev


    function GetWindow : int
	result window
    end GetWindow


    body procedure GeneralInitialize
	if initialized then
	    AssertFailed ("Widget already initialized")
	end if
	initialized := true
	window := Window.GetSelect
	WidgetGlobals.AddWindowToArray (window)

	% Add to the linked list of widgets.
	SetPrev (WidgetGlobals.lastWidget)
	SetNext (nil)
	if WidgetGlobals.lastWidget = nil then
	    WidgetGlobals.firstWidget := self
	else
	    GenericWidgetBodyClass (WidgetGlobals.lastWidget).SetNext (self)
	end if
	WidgetGlobals.lastWidget := self

	% Set the foreground colour
	if colourfg = backgroundColour then
	    foregroundColour := black
	else
	    foregroundColour := colourfg
	end if

	% Set the button colours
	if colourbg = black then
	    buttonColor := white
	    illuminatedColor := brightwhite
	else
	    buttonColor := grey
	    illuminatedColor := white
	    shadowColor := darkgrey
	end if

	% Draw the widget, if appropriate.
	if WidgetGlobals.showWhenInitialized then
	    showing := true
	    DrawWidget
	end if
    end GeneralInitialize


    function TranslateBorder (border : int) : int
	if border = 0 then
	    result WidgetGlobals.LINE
	elsif border = WidgetGlobals.LINE or border = WidgetGlobals.INDENT or
		border = WidgetGlobals.EXDENT then
	    result border
	else
	    AssertFailed ("Bad value of border: " + intstr (border))
	end if
    end TranslateBorder


    function TranslateFont (font : int) : int
	if font = 0 then
	    result WidgetGlobals.systemFontID
	else
	    result font
	end if
    end TranslateFont


    function TranslateAlignment (alignment : int) : int
	if alignment = 0 then
	    result WidgetGlobals.LEFT
	elsif alignment = WidgetGlobals.LEFT or
		alignment = WidgetGlobals.RIGHT then
	    result alignment
	else
	    AssertFailed ("Bad value of alignment: " + intstr (alignment))
	end if
    end TranslateAlignment


    %
    % The external subprograms of GenericWidgetClass
    %
    body procedure Show
	if not initialized then
	    AssertFailed ("The object has not been initialized yet")
	else
	    showing := true
	    DrawWidget
	end if
    end Show


    body procedure Hide
	showing := false
	EraseWidget
    end Hide


    body procedure Refresh
	if showing then
	    DrawWidget
	end if
    end Refresh


    body function GetX : int
	result x
    end GetX


    body function GetY : int
	result y
    end GetY


    body function GetWidth : int
	result width
    end GetWidth


    body function GetHeight : int
	result height
    end GetHeight


    body procedure SetColour (newColour : int)
	var r, g, b : real
	buttonColor := newColour
	RGB.GetColor (newColour, r, g, b)
	r := r * 2 / 3
	g := g * 2 / 3
	b := b * 2 / 3
	shadowColor := RGB.AddColour (r, g, b)
	if showing then
	    DrawWidget
	end if
    end SetColour


    body procedure Dispose
	EraseWidget

	% Remove oneself from the list of widgets.
	var prev : ^GenericWidgetBodyClass := GetPrev
	var next : ^GenericWidgetBodyClass := GetNext

	if WidgetGlobals.firstWidget = self then
	    WidgetGlobals.firstWidget := next
	else
	    prev -> SetNext (next)
	end if

	if WidgetGlobals.lastWidget = self then
	    WidgetGlobals.lastWidget := prev
	else
	    next -> SetPrev (prev)
	end if
    end Dispose


    body procedure SetPosition (newX, newY : int)
	if showing then
	    EraseWidget
	end if

	originalX := newX
	originalY := newY
	drawn := false

	if showing then
	    DrawWidget
	end if
    end SetPosition


    body procedure SetSize (newWidth, newHeight : int)
	if showing then
	    EraseWidget
	end if

	originalWidth := newWidth
	originalHeight := newHeight
	drawn := false

	if showing then
	    DrawWidget
	end if
    end SetSize


    body procedure SetPositionAndSize (newX, newY, newWidth, newHeight : int)
	if showing then
	    EraseWidget
	    showing := false
	    SetPosition (newX, newY)
	    SetSize (newWidth, newHeight)
	    showing := true
	    DrawWidget
	else
	    SetPosition (newX, newY)
	    SetSize (newWidth, newHeight)
	end if
    end SetPositionAndSize


    body procedure SetUserValue (value : int)
	userValue := value
    end SetUserValue


    body function GetUserValue : int
	result userValue
    end GetUserValue


    body procedure SetUserObject (object : ^anyclass)
	userObject := object
    end SetUserObject


    body function GetUserObject : ^anyclass
	result userObject
    end GetUserObject
end GenericWidgetBodyClass

