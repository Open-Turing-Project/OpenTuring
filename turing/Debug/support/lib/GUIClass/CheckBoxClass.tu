unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CheckBoxClass - Class that implements check boxes much as they appear in
%                 MS Windows 3.1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of CheckBoxClass is as follows:
%   procedure Show - Display the widget.
%   procedure Hide - Hide the widget.
%   procedure Dispose - Hide the widget and free any data structures allocated
%                       by the class.  To be called before the Widget is freed.
%   procedure SetPosition (x, y : int) - Move the widget to (x, y).
%   procedure SetSize (width, height : int) - Resize the widget.
%   procedure SetPositionAndSize (x, y, width, height : int) - Move and resize
%                                                              the widget.
%   procedure Enable - Enable (activate) the widget.
%   procedure Disable - Disable (deactivate) the widget.
%   procedure SetLabel (label : string) - Set the widget's label.
%   procedure Initialize (x, y, width : int, label : string,
%                         actionProc : procedure x ())
%                       x, y - The lower left corner of the button.
%                       width - The minimum width of the button.
%                       label - The text to appear in the button
%                       actionProc - The procedure to be called when the
%                                    button is pressed.
%                       Initialize the button.
%   procedure InitializeFull (x, y, height, width : int, label : string,
%                             shortCut : char, default : boolean,
%                             actionProc : procedure x ())
%                       x, y - The lower left corner of the button.
%                       height - The minimum height of the button.
%                       width - The minimum width of the button.
%                       label - The text to appear in the button
%                       shortCut - The keystroke to use as the short cut.
%                                  Use '\0' if no short cut wanted.
%                       default - Whether the button should be the default
%                                 button or not.
%                       actionProc - The procedure to be called when the
%                                    button is pressed.
%                       Initialize the button.
%   function GetCheckBox : boolean - Return whether check box is filled.
%   procedure SetCheckBox (checked : boolean)
%                       checked - Whether to set the check box to filled.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of CheckBoxClass is as follows:
%   function ConsiderButtonDown (x, y : int) : boolean
%                       x, y - The location of of the mouse when the button
%                              was pressed.
%                       Handle the mouse button being pressed.  If it occured
%                       in the widget, handle the event.  Return true if the
%                       event is handled by this function.  Return false
%                       otherwise.  If the widget was pressed, call the
%                       actionProc for this widget.
%   function ConsiderKeystroke (key : char) : boolean
%                       key - The key pressed.
%                       Handle a keystroke. If the keystroke is the
%                       shortcut for the widget (or the user pressed ENTER and
%                       this is the default widget), the actionProc for the
%                       widget is called and the function returns true,
%                       otherwise it returns false.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of CheckBoxClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawDisabledLabel (clr, bgColour : int) - Draw the disabled
%                                                       label in the specified
%                                                       colour.
%   procedure DrawLabel (clr : int) - Draw the label in the specified colour.
%   procedure ActionProc - The action to occur when button pressed.
%   procedure DrawPressedButton - Draws a pressed button.
%   procedure DrawUnpressedButton - Draws a unpressed button.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of CheckBoxClass are as follows:
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
%   enabled - Is the widget enabled (activated).
%   labelWidth, labelHeight - The width and height of the text in the button.
%   labelTotalHeight - The maximum height of a letter.
%   labelDescent - The descent of the label.
%   labelText - The actual text of the label.
%   labelX, labelY - Position to draw the label, w.r.t. x, y
%   actionProc - The procedure to call when the button is pushed.
%   shortCutChar - The keystroke shortcut to press the button.
%   alignment - Whether the box appears to right or left of text.
%   boxIsChecked - Whether the check box is currently filled.
%   boxYNudge - How far up the box should be drawn w.r.t. x, y
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of CheckBoxClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   labelFontID - The font ID number for the label.
%   boxSize - The size of the actual check box.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class CheckBoxClass
    inherit GenericButtonClass

    import WidgetGlobals

    %
    % Exported routines for CheckBoxClass
    %
    export
    % The external interface
	Initialize, InitializeFull, SetCheckBox, GetCheckBox


    %
    % The inheritable variables of CheckBoxClass
    %
    var userActionProc : procedure x (filled : boolean)
    var alignment : int 
    var boxIsChecked : boolean := false % Whether box is currently filled
    var boxYNudge : int := 0


    %
    % The inheritable constants of CheckBoxClass
    %
    const boxSize : int := 12 % Size of the actual check box


    %
    % The external subprograms of CheckBoxClass
    %
    procedure InitializeFull (newX, newY : int, newLabel : string,
	    newActionProc : procedure x (check : boolean),
	    newAlignment : int, shortCut : char)
	SetPosition (newX, newY)
	SetLabel (newLabel)
	userActionProc := newActionProc
	shortCutChar := shortCut
	alignment := TranslateAlignment (newAlignment)
	
	GeneralInitialize
    end InitializeFull


    procedure Initialize (newX, newY : int, newLabel : string,
	    newActionProc : procedure x (check : boolean))
	InitializeFull (newX, newY, newLabel, newActionProc,
	    WidgetGlobals.LEFT, '\0')
    end Initialize


    function GetCheckBox : boolean
	result boxIsChecked
    end GetCheckBox


    procedure SetCheckBox (newChecked : boolean)
	if showing then
	    EraseWidget
	    boxIsChecked := newChecked
	    DrawWidget
	else
	    boxIsChecked := newChecked
	end if
    end SetCheckBox


    %
    % Overridden subprograms
    %
    body procedure SetSize (newWidth, newHeight : int)
	% For check boxes, the size is irrelevant
    end SetSize


    body procedure ComputeWidgetPosition
	% We must computer x, y, width, height, labelX, labelY
	if alignment = WidgetGlobals.RIGHT then
	    x := originalX - (boxSize + 5 + labelWidth)
	else
	    x := originalX
	end if
	y := originalY

	width := boxSize + 5 + labelWidth

	if alignment = WidgetGlobals.LEFT then
	    labelX := boxSize + 5
	else
	    labelX := 0
	end if
	labelY := (boxSize - (labelHeight + 1)) div 2
	if labelY < 0 then
	    boxYNudge := - labelY
	    labelY := 0
	end if
	height := max (boxSize + boxYNudge, labelY + labelTotalHeight) + 1
	drawn := true
    end ComputeWidgetPosition


    body procedure ActionProc
	boxIsChecked := not boxIsChecked
	DrawUnpressedButton
	userActionProc (boxIsChecked)
    end ActionProc


    body procedure DrawPressedButton
	if alignment = WidgetGlobals.LEFT then
	    Draw.Box (x, y + boxYNudge, x + boxSize, y + boxYNudge + boxSize,
		foregroundColour)
	    Draw.Box (x + 1, y + boxYNudge + 1, x + boxSize - 1,
		y + boxYNudge + boxSize - 1, foregroundColour)
	    Draw.FillBox (x + 3, y + boxYNudge + 3, x + boxSize - 3,
		y + boxYNudge + boxSize - 3, backgroundColour)
	    if boxIsChecked then
		Draw.Line (x, y + boxYNudge, x + boxSize,
		    y + boxYNudge + boxSize, foregroundColour)
		Draw.Line (x + boxSize, y + boxYNudge, x,
		    y + boxYNudge + boxSize, foregroundColour)
	    end if
	else
	    Draw.Box (x + width - boxSize - 1, y + boxYNudge, x + width - 1,
		y + boxYNudge + boxSize, foregroundColour)
	    Draw.Box (x + width - boxSize, y + boxYNudge + 1, x + width - 2,
		y + boxYNudge + boxSize - 1, foregroundColour)
	    Draw.FillBox (x + width - boxSize + 2, y + boxYNudge + 3,
		x + width - 4, y + boxYNudge + boxSize - 3, backgroundColour)
	    if boxIsChecked then
		Draw.Line (x + width - boxSize - 1, y + boxYNudge,
		    x + width - 1, y + boxYNudge + boxSize, foregroundColour)
		Draw.Line (x + width - 1, y + boxYNudge,
		    x + width - boxSize - 1, y + boxYNudge + boxSize,
		    foregroundColour)
	    end if
	end if
	DrawLabel (foregroundColour)
    end DrawPressedButton


    body procedure DrawUnpressedButton
	if alignment = WidgetGlobals.LEFT then
	    Draw.Box (x, y + boxYNudge, x + boxSize, y + boxYNudge + boxSize,
		foregroundColour)
	    Draw.FillBox (x + 1, y + boxYNudge + 1, x + boxSize - 1,
		y + boxYNudge + boxSize - 1, backgroundColour)
	    if boxIsChecked then
		Draw.Line (x, y + boxYNudge, x + boxSize,
		    y + boxYNudge + boxSize, foregroundColour)
		Draw.Line (x + boxSize, y + boxYNudge, x,
		    y + boxYNudge + boxSize, foregroundColour)
	    end if
	else
	    Draw.Box (x + width - boxSize - 1, y + boxYNudge, x + width - 1,
		y + boxYNudge + boxSize, foregroundColour)
	    Draw.FillBox (x + width - boxSize, y + boxYNudge + 1,
		x + width - 2, y + boxYNudge + boxSize - 1, backgroundColour)
	    if boxIsChecked then
		Draw.Line (x + width - boxSize - 1, y + boxYNudge,
		    x + width - 1, y + boxYNudge + boxSize, foregroundColour)
		Draw.Line (x + width - 1, y + boxYNudge,
		    x + width - boxSize - 1, y + boxYNudge + boxSize,
		    foregroundColour)
	    end if
	end if
	if enabled then
	    DrawLabel (foregroundColour)
	else
	    DrawDisabledLabel (foregroundColour, backgroundColour)
	end if
    end DrawUnpressedButton
end CheckBoxClass

