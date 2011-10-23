unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericLabelledWidgetClass - Abstract class that implements active widgets
%                              that have a label attached to them.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of GenericLabelledWidgetClass is as follows:
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
% The package interface of GenericLabelledWidgetClass is as follows:
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
% The inheritable subprograms of GenericLabelledWidgetClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawDisabledLabel (clr, bgColour : int) - Draw the disabled
%                                                       label in the specified
%                                                       colour.
%   procedure DrawLabel (clr : int) - Draw the label in the specified colour.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of GenericLabelledWidgetClass are as follows:
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of GenericLabelledWidgetClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   labelFontID - The font ID number for the label.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
%   function ConsiderButtonDown (x, y : int) : boolean
%   function ConsiderKeystroke (key : char) : boolean
%   procedure DrawWidget - Draw the widget.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericLabelledWidgetClass
    inherit GenericActiveWidgetClass

    import WidgetGlobals

    %
    % Exported routines for GenericLabelledWidgetClass
    %
    export
    % The external interface
	SetLabel

    %
    % The inheritable variables of GenericLabelledWidgetClass
    %
    var labelWidth : int % Width of label
    var labelHeight : int % Height of label (from top to bottom of 'H')
    var labelTotalHeight : int % Total height of letters in label
    var labelDescent : int % Descent of label
    var labelText : string % Label text
    var labelX, labelY : int % Position to draw the label, w.r.t. x, y

    %
    % The inheritable constants of GenericLabelledWidgetClass
    %
    const labelFontID : int := WidgetGlobals.systemFontID


    %
    % The inheritable subprograms of GenericLabelledWidgetClass
    %
    procedure DrawDisabledLabel (clr, backgroundColour : int)
	var x : int := x + labelX
	var y : int := y + labelY
	if not initialized then
	    AssertFailed ("Labelled widget not yet initialized")
	else
	    Font.Draw (labelText, x, y, labelFontID, clr)

	    for i : x .. x + labelWidth by 2
		for j : y - labelDescent .. y -labelDescent + labelTotalHeight by 2
		    Draw.Dot (i, j, backgroundColour)
		    Draw.Dot (i + 1, j + 1, backgroundColour)
		end for
	    end for
	end if
    end DrawDisabledLabel


    procedure DrawLabel (clr : int)
	var x : int := x + labelX
	var y : int := y + labelY
	if not initialized then
	    AssertFailed ("Labelled widget not yet initialized")
	else
	    Font.Draw (labelText, x, y, labelFontID, clr)
	end if
    end DrawLabel


    %
    % The external subprograms of GenericLabelledWidgetClass
    %
    procedure SetLabel (newLabel : string)
	var height, ascent, internalLeading : int

	if showing then
	    EraseWidget
	end if

	labelText := newLabel
	labelWidth := Font.Width (newLabel, labelFontID)
	Font.Sizes (labelFontID, height, ascent, labelDescent,
	    internalLeading)
	labelHeight := ascent - internalLeading
	labelTotalHeight := labelDescent + ascent
	#if _DOS_ then
	    labelHeight -= 3
	    labelTotalHeight -= 3
	#end if

	drawn := false

	if showing then
	    DrawWidget
	end if
    end SetLabel
end GenericLabelledWidgetClass


