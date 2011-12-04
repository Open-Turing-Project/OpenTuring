unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericButtonClass - Abstract class that implements button-like objects that
%                   activate when clicked.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of GenericButtonClass is as follows:
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of GenericButtonClass is as follows:
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
% The inheritable subprograms of GenericButtonClass are as follows:
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
% The inheritable variables of GenericButtonClass are as follows:
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
% The inheritable constants of GenericButtonClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   labelFontID - The font ID number for the label.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%   procedure ActionProc - The action to occur when button pressed.
%   procedure DrawPressedBorder - Draws a pressed button.
%   procedure DrawUnpressedBorder - Draws a unpressed button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericButtonClass
    inherit GenericLabelledWidgetClass

    %
    % The inheritable variables of GenericButtonClass
    %
    var shortCutChar : char
    var default : boolean := false


    %
    % The inheritable subprograms of GenericButtonClass
    %
    deferred procedure DrawPressedButton
    deferred procedure DrawUnpressedButton


    %
    % Overridden subprograms
    %
    body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	assert initialized
	if not enabled or not showing then
	    result false
	end if
	if x <= mouseX and mouseX < x + width - 1 and
		y <= mouseY and mouseY < y + height - 1 then
	    DrawPressedButton
	    var mx, my, b : int
	    var pressed : boolean := true
	    loop
		Mouse.Where (mx, my, b)
		if x <= mx and mx < x + width and
			y <= my and my < y + height and
			not pressed then
		    pressed := true
		    DrawPressedButton
		elsif (mx < x or x + width <= mx or
			my < y or y + height <= my) and pressed then
		    pressed := false
		    DrawUnpressedButton
		end if
		exit when b = 0
	    end loop
	    if pressed then
		DrawUnpressedButton
		GeneralActionProc
		result true
	    else
		result true
	    end if
	else
	    result false
	end if
    end ConsiderButtonDown


    body function ConsiderKeystroke (key : char) : boolean
	assert initialized
	if enabled and showing and
		( (key = shortCutChar) or (default and key = '\n')) then
	    GeneralActionProc
	    result true
	else
	    result false
	end if
    end ConsiderKeystroke


    body procedure Disable
	if showing then
	    enabled := false
	    DrawDisabledLabel (foregroundColour, backgroundColour)
	else
	    enabled := false
	end if
    end Disable


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if
	DrawUnpressedButton
    end DrawWidget


    body procedure Enable
	if showing then
	    enabled := true
	    DrawLabel (foregroundColour)
	else
	    enabled := true
	end if
    end Enable
end GenericButtonClass

