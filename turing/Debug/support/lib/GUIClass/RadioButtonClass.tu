unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% RadioButtonClass - Class that implements radio buttons much as they appear
%                    in MS Windows 3.1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of RadioButtonClass is as follows:
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
%   procedure Initialize (x, y, label : string, actionProc : procedure x (),
%                         selectActionProc : procedure x (widget ; ^Widget))
%                       x, y - The lower left corner of the radio button.
%                       label - The text to appear beside the radio button
%                       actionProc - The procedure to be called when the
%                                    radio button is pressed.
%                       selectActionProc - The procedure to be called when the
%                                          radio button is selected.  This
%                                          procedure should deselect all other
%                                          buttons in the group.
%                       Initialize the radio button.
%   procedure InitializeFull (x, y, alignment : int, label : string,
%                             shortCut : char, actionProc : procedure x ())
%                             selectActionProc : procedure x (widget ; ^Widget))
%                       x, y - The lower left corner of the radio button.
%                       width - The minimum width of the radio button.
%                       alignment - Whether the circle appears to right or
%                                   left of text.
%                       label - The text to appear in the radio button
%                       shortCut - The keystroke to use as the short cut.
%                                  Use '\0' if no short cut wanted.
%                       actionProc - The procedure to be called when the radio
%                                    button is pressed.
%                       selectActionProc - The procedure to be called when the
%                                          radio button is selected.  This
%                                          procedure should deselect all other
%                                          buttons in the group.
%                       Initialize the radio button.
%   procedure Select (isSelected : boolean)
%                       isDefault - Whether to set the button to default or
%                                   not.
%   procedure GetSpecs (var x, y, alignment : int)
%                       x, y - The returned position of the radio button.
%                       alignment - The returned alignment of the radio button.
%   function GetVerticalSeparation : int - Return the vertical separation of
%                       two radio buttons.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of RadioButtonClass is as follows:
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
% The inheritable subprograms of RadioButtonClass are as follows:
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
% The inheritable variables of RadioButtonClass are as follows:
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
%   selectActionProc - The procedure call when the radio button is selected to
%                      deselect all other buttons in group.
%   shortCutChar - The keystroke shortcut to press the button.
%   selected - Whether the button is the currently select on in group.
%   alignment - Whether the circle appears to right or left of text.
%   boxYNudge - How far up the circle should be drawn w.r.t. x, y
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of RadioButtonClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   labelFontID - The font ID number for the label.
%   circleRadius - The size of the actual radio circle.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class RadioButtonClass
    inherit GenericRadioButtonClass

    import WidgetGlobals

    %
    % Exported routines for ButtonClass
    %
    export
    % The external interface
	Initialize, InitializeFull


    %
    % The inheritable variables of RadioButtonClass
    %
    var boxYNudge : int := 0


    %
    % The inheritable constants of RadioButtonClass
    %
    const circleRadius : int := 6
    const verticalSeparation : int := 3 * circleRadius


    %
    % The external subprograms of RadioButtonClass
    %
    procedure InitializeFull (newX, newY : int, newLabel : string,
	    newActionProc : procedure x (),
	    joinRadio : ^GenericRadioButtonClass,
	    newDirection : int, shortCut : char)
	InitRadio (newActionProc, shortCut, joinRadio)
	if newX = - 1 and newY = - 1 and joinRadio not= nil then
	    var x, y, direction : int
	    joinRadio -> GetSpecs (x, y, direction)
	    SetPosition (x, y - verticalSeparation)
	else
	    SetPosition (newX, newY)
	end if
	SetLabel (newLabel)
	if newDirection = 0 and joinRadio not= nil then
	    var direction : int
	    joinRadio -> GetSpecs (x, y, direction)
	    alignment := direction
	else
	    alignment := TranslateAlignment (newDirection)
	end if

	GeneralInitialize
    end InitializeFull


    procedure Initialize (newX, newY : int, newLabel : string,
	    newActionProc : procedure x (),
	    joinRadio : ^GenericRadioButtonClass)
	InitializeFull (newX, newY, newLabel, newActionProc,
	    joinRadio, WidgetGlobals.LEFT, '\0')
    end Initialize


    %
    % Overridden subprograms
    %
    body procedure SetSize (newWidth, newHeight : int)
	% For radio buttons, the size is irrelevant
    end SetSize


    body procedure ComputeWidgetPosition
	if alignment = WidgetGlobals.RIGHT then
	    x := originalX - (2 * circleRadius + 5 + labelWidth)
	else
	    x := originalX
	end if
	y := originalY
	width := 2 * circleRadius + 5 + labelWidth

	if alignment = WidgetGlobals.LEFT then
	    labelX := 2 * circleRadius + 5
	else
	    labelX := 0
	end if
	labelY := (2 * circleRadius - (labelHeight + 1)) div 2
	if labelY < 0 then
	    boxYNudge := - labelY
	    labelY := 0
	end if
	height := max (2 * circleRadius + boxYNudge, labelY +
	    labelTotalHeight) + 1
	drawn := true
    end ComputeWidgetPosition


    body procedure DrawPressedButton
	if alignment = WidgetGlobals.LEFT then
	    Draw.FillOval (x + circleRadius, y + circleRadius + boxYNudge,
		circleRadius, circleRadius, backgroundColour)
	    Draw.Oval (x + circleRadius, y + circleRadius + boxYNudge,
		circleRadius, circleRadius, foregroundColour)
	    Draw.Oval (x + circleRadius, y + circleRadius + boxYNudge,
		circleRadius - 1, circleRadius - 1, foregroundColour)
	    if selected then
		Draw.FillOval (x + circleRadius,
		    y + circleRadius + boxYNudge,
		    circleRadius - 3, circleRadius - 3, foregroundColour)
	    end if
	else
	    Draw.FillOval (x + width - circleRadius - 1,
		y + circleRadius + boxYNudge,
		circleRadius, circleRadius, backgroundColour)
	    Draw.Oval (x + width - circleRadius - 1,
		y + circleRadius + boxYNudge,
		circleRadius, circleRadius, foregroundColour)
	    Draw.Oval (x + width - circleRadius - 1,
		y + circleRadius + boxYNudge,
		circleRadius - 1, circleRadius - 1, foregroundColour)
	    if selected then
		Draw.FillOval (x + width - circleRadius - 1,
		    y + circleRadius + boxYNudge,
		    circleRadius - 3, circleRadius - 3, foregroundColour)
	    end if
	end if
	DrawLabel (foregroundColour)
    end DrawPressedButton


    body procedure DrawUnpressedButton
	if alignment = WidgetGlobals.LEFT then
	    Draw.FillOval (x + circleRadius, y + circleRadius + boxYNudge,
		circleRadius, circleRadius, backgroundColour)
	    Draw.Oval (x + circleRadius, y + circleRadius + boxYNudge,
		circleRadius, circleRadius, foregroundColour)
	    if selected then
		Draw.FillOval (x + circleRadius,
		    y + circleRadius + boxYNudge,
		    circleRadius - 3, circleRadius - 3, foregroundColour)
	    end if
	else
	    Draw.FillOval (x + width - circleRadius - 1,
		y + circleRadius + boxYNudge,
		circleRadius, circleRadius, backgroundColour)
	    Draw.Oval (x + width - circleRadius - 1,
		y + circleRadius + boxYNudge,
		circleRadius, circleRadius, foregroundColour)
	    if selected then
		Draw.FillOval (x + width - circleRadius - 1,
		    y + circleRadius + boxYNudge,
		    circleRadius - 3, circleRadius - 3, foregroundColour)
	    end if
	end if
	if enabled then
	    DrawLabel (foregroundColour)
	else
	    DrawDisabledLabel (foregroundColour, backgroundColour)
	end if
    end DrawUnpressedButton
end RadioButtonClass

