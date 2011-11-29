unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% PictureRadioButtonClass - Class that implements buttons with pictures in
%                           them with radio button like behaviour.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of PictureRadioButtonClass is as follows:
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
% The package interface of PictureRadioButtonClass is as follows:
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
% The inheritable subprograms of PictureRadioButtonClass are as follows:
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
% The inheritable variables of PictureRadioButtonClass are as follows:
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
% The inheritable constants of PictureRadioButtonClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   labelFontID - The font ID number for the label.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class PictureRadioButtonClass
    inherit GenericRadioButtonClass

    %
    % Exported routines for ButtonClass
    %
    export
    % The external interface
	Initialize, InitializeFull


    %
    % The inheritable variables of PictureButtonClass
    %
    var buttonPic : int
    var picX, picY : int
    #if _DOS_ then
	var drawMode : int := picCopy
    #else
	var drawMode : int := picMerge
    #end if


    %
    % The inheritable constants of PictureButtonClass
    %


    %
    % Exported subprograms
    %
    procedure InitializeFull (newX, newY, newPic : int,
	    newActionProc : procedure x (),
	    joinRadio : ^GenericRadioButtonClass,
	    newHeight, newWidth : int, shortCut : char, mergePic : boolean)
	InitRadio (newActionProc, shortCut, joinRadio)
	if newX = - 1 and newY = - 1 and joinRadio not= nil then
	    var x, y, direction : int
	    joinRadio -> GetSpecs (x, y, direction)
	    if newWidth = 0 then
		SetPositionAndSize (x,
		    y - (Pic.Height (newPic) + 2 * (riserSize + 1)),
		    newWidth, newHeight)
	    else
		SetPositionAndSize (x,
		    y - (newWidth + 2 * (riserSize + 1)),
		    newWidth, newHeight)
	    end if
	else
	    SetPositionAndSize (newX, newY, newWidth, newHeight)
	end if
	buttonPic := newPic
	if not mergePic then
	    drawMode := picCopy
	end if
	alignment := 0 % Not actually used, but prevents uninit var error.

	GeneralInitialize
    end InitializeFull


    procedure Initialize (newX, newY, newPic : int,
	    newActionProc : procedure x (),
	    joinRadio : ^GenericRadioButtonClass)
	InitializeFull (newX, newY, newPic, newActionProc,
	    joinRadio, 0, 0, '\0', true)
    end Initialize


    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	x := originalX
	y := originalY
	if originalWidth = 0 then
	    width := Pic.Width (buttonPic) + 2 * (riserSize + 1)
	else
	    width := originalWidth + 2 * (riserSize + 1)
	end if
	picX := max (0, (originalWidth - Pic.Width (buttonPic)) div 2) +
	    x + riserSize + 1
	if originalHeight = 0 then
	    height := Pic.Height (buttonPic) + 2 * (riserSize + 1)
	else
	    height := originalHeight + 2 * (riserSize + 1)
	end if
	picY := max (0, (originalHeight - Pic.Height (buttonPic)) div 2) +
	    y + riserSize + 1
    end ComputeWidgetPosition


    body procedure DrawPressedButton
	const x1 : int := x
	const y1 : int := y
	const x2 : int := x + width - 1
	const y2 : int := y + height - 1

	DrawPressedBorder (false)
	View.ClipSet (x1 + riserSize + 1, y1 + riserSize + 1,
	    x2 - riserSize - 1, y2 - riserSize - 1)
	Pic.Draw (buttonPic, picX, picY, drawMode)
	if not enabled then
	    for i : x1 + riserSize + 1 .. x2 - riserSize by 2
		for j : y1 + riserSize + 1 .. y2 - riserSize by 2
		    Draw.Dot (i, j, backgroundColour)
		    Draw.Dot (i + 1, j + 1, backgroundColour)
		end for
	    end for
	end if
	View.ClipOff
    end DrawPressedButton


    body procedure DrawUnpressedButton
	const x1 : int := x
	const y1 : int := y
	const x2 : int := x + width - 1
	const y2 : int := y + height - 1

	if selected then
	    DrawPressedButton
	else
	    DrawUnpressedBorder (false)
	    View.ClipSet (x1 + riserSize + 1, y1 + riserSize + 1,
		x2 - riserSize - 1, y2 - riserSize - 1)
	    Pic.Draw (buttonPic, picX, picY, drawMode)
	    if not enabled then
		for i : x1 + riserSize + 1 .. x2 - riserSize by 2
		    for j : y1 + riserSize + 1 .. y2 - riserSize by 2
			Draw.Dot (i, j, backgroundColour)
			Draw.Dot (i + 1, j + 1, backgroundColour)
		    end for
		end for
	    end if
	    View.ClipOff
	end if
    end DrawUnpressedButton


    body procedure Disable
	if showing then
	    enabled := false
	    DrawUnpressedButton
	else
	    enabled := false
	end if
    end Disable


    body procedure Enable
	if showing then
	    enabled := true
	    DrawUnpressedButton
	else
	    enabled := true
	end if
    end Enable
end PictureRadioButtonClass

