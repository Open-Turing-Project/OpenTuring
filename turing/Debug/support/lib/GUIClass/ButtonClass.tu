unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ButtonClass - Class that implements buttons much as they appear in
%               MS Windows 3.1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of ButtonClass is as follows:
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
%   procedurre SetDefault (isDefault : boolean)
%                       isDefault - Whether to set the button to default or
%                                   not.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of ButtonClass is as follows:
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
% The inheritable subprograms of ButtonClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawDisabledLabel (clr, bgColour : int) - Draw the disabled
%                                                       label in the specified
%                                                       colour.
%   procedure DrawLabel (clr : int) - Draw the label in the specified colour.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure ActionProc - The action to occur when button pressed.
%   procedure DrawPressedButton - Draws a pressed button.
%   procedure DrawUnpressedButton - Draws a unpressed button.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of ButtonClass are as follows:
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
%   default - Is this the default button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of ButtonClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   labelFontID - The font ID number for the label.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class ButtonClass
    inherit GenericButtonClass

    import WidgetGlobals

    %
    % Exported routines for ButtonClass
    %
    export
    % The external interface
	Initialize, InitializeFull, SetDefault


    %
    % The inheritable variables of ButtonLikeClass
    %
    var userActionProc : procedure actionProc ()


    %
    % The external subprograms of ButtonClass
    %
    procedure InitializeFull (newX, newY, newWidth : int,
	    newLabel : string, newActionProc : procedure x (),
	    newHeight : int, shortCut : char, newDefault : boolean)
	SetPositionAndSize (newX, newY, newWidth, newHeight)
	SetLabel (newLabel)
	userActionProc := newActionProc
	shortCutChar := shortCut
	default := newDefault

	GeneralInitialize
    end InitializeFull


    procedure Initialize (newX, newY, newWidth : int,
	    newLabel : string, newActionProc : procedure x ())
	InitializeFull (newX, newY, newWidth, newLabel, newActionProc,
	    0, '\0', false)
    end Initialize


    procedure SetDefault (isDefault : boolean)
	% Do nothing if we're already in the proper state.
	if isDefault = default then
	    return
	end if

	if isDefault then
	    % Make certain all otehr buttons in window are no default
	    var widget : ^GenericWidgetBodyClass := WidgetGlobals.firstWidget
	    loop
		exit when widget = nil
		if widget -> GetWindow = window and
			objectclass (widget) >= ButtonClass then
		    ButtonClass (widget).SetDefault (false)
		end if
		widget := widget -> GetNext
	    end loop
	end if

	if showing then
	    EraseWidget
	    default := isDefault
	    DrawWidget
	else
	    default := isDefault
	end if
    end SetDefault


    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	% We must computer x, y, width, height, labelX, labelY
	x := originalX
	y := originalY

	if originalWidth < labelWidth + 30 then
	    width := labelWidth + 30
	else
	    width := originalWidth
	end if
	if originalHeight < labelHeight + labelDescent + 12 then
	    height := labelHeight + labelDescent + 12
	else
	    height := originalHeight
	end if

	labelX := (width - labelWidth) div 2
	labelY := (height - labelHeight) div 2
	drawn := true
    end ComputeWidgetPosition


    body procedure ActionProc
	DrawUnpressedButton
	userActionProc
    end ActionProc


    body procedure Disable
	if showing then
	    enabled := false
	    DrawDisabledLabel (black, buttonColor)
	else
	    enabled := false
	end if
    end Disable


    body procedure Enable
	if showing then
	    enabled := true
	    DrawLabel (black)
	else
	    enabled := true
	end if
    end Enable
    
    
    body procedure DrawPressedButton
	DrawPressedBorder (default)
	DrawLabel (black)
    end DrawPressedButton


    body procedure DrawUnpressedButton
	DrawUnpressedBorder (default)
	if enabled then
	    DrawLabel (black)
	else
	    DrawDisabledLabel (black, buttonColor)
	end if
    end DrawUnpressedButton
end ButtonClass

