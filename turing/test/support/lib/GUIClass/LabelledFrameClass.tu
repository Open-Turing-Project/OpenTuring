unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% LabelledFrameClass - Class that implements frames with a label much as they
%                      appear in MS Windows 3.1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of LabelledFrameClass is as follows:
%   procedure Show - Display the widget.
%   procedure Hide - Hide the widget.
%   procedure Dispose - Hide the widget and free any data structures allocated
%                       by the class.  To be called before the Widget is freed.
%   procedure SetPosition (x, y : int) - Move the widget to (x, y).
%   procedure SetSize (width, height : int) - Resize the widget.
%   procedure SetPositionAndSize (x, y, width, height : int) - Move and resize
%                                                              the widget.
%   procedure Initialize (x1, y1, x2, y2, kind : int, text : string) -
%                               Create a frame from (x1, y2) to (x2, y2).
%                       kind - One of GUI.NONE, GUI.INDENT or GUI.EXDENT
%                       text - The text displayed in the frame.
%   procedure SetLabel (text : string) - Change the label in the frame to text.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of LabelledFrameClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%   procedure DrawText (clr : int) - Draws the label in specified colour.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of LabelledFrameClass are as follows:
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
%   kind - The kind of line to be displayed.
%   text - The frame's label.
%   labelX, labelY - Position to draw the label.
%   labelWidth - Width of the label.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of LabelledFrameClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class LabelledFrameClass
    inherit GenericWidgetBodyClass

    import WidgetGlobals

    %
    % Exported routines for LabelledFrameClass
    %
    export
    % The external interface
	Initialize, SetLabel

    %
    % The inheritable variables of LabelledFrameClass
    %
    % The kind of line to be displayed.
    var kind : int
    % The text to draw
    var text : string
    % The font ID that the label is drawn in
    var textFontID : int := WidgetGlobals.systemFontID
    % Position to draw the label
    var labelX, labelY : int
    % Width of the label
    var labelWidth : int

    %
    % The external subprograms of LabelledFrameClass
    %
    procedure Initialize (newX1, newY1, newX2, newY2, newKind : int,
	    newText : string)
	SetPositionAndSize (min (newX1, newX2), min (newY1, newY2),
	    max (newX1, newX2) - min (newX1, newX2),
	    max (newY1, newY2) - min (newY1, newY2))
	kind := TranslateBorder (newKind)
	text := newText

	GeneralInitialize
    end Initialize


    procedure SetLabel (newText : string)
	if showing then
	    EraseWidget
	    text := newText
	    drawn := false
	    DrawWidget
	else
	    text := newText
	    drawn := false
	end if
    end SetLabel



    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	var height, ascent, descent, internalLeading : int

	x := originalX
	y := originalY

	labelWidth := Font.Width (text, textFontID)
	Font.Sizes (textFontID, height, ascent, descent, internalLeading)
	height := ascent - internalLeading
	#if _DOS_ then
	    height -= 3
	#end if

	labelX := (originalWidth - labelWidth) div 2
	if kind = WidgetGlobals.LINE then
	    labelY := originalHeight - (height) div 2
	else
	    labelY := originalHeight - (riserSize + height div 2)
	end if
	drawn := true
    end ComputeWidgetPosition


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if
	var drawTop : boolean := labelWidth < originalWidth - 8
	var x2 : int := x + originalWidth
	var y2 : int := y + originalHeight
	if kind = WidgetGlobals.LINE then
	    if drawTop then
		Draw.Line (x, y, x2, y, foregroundColour)
		Draw.Line (x, y, x, y2, foregroundColour)
		Draw.Line (x2, y, x2, y2, foregroundColour)
		Draw.Line (x, y2, x + labelX - 4, y2, foregroundColour)
		Draw.Line (x + labelX + labelWidth + 3, y2, x2, y2, 
		    foregroundColour)
	    else
		Draw.Line (x, y, x2, y, foregroundColour)
		Draw.Line (x, y, x, y + labelY - 2, foregroundColour)
		Draw.Line (x2, y, x2, y + labelY - 2, foregroundColour)
	    end if
	else
	    var topLeftColour, bottomRightColour : int
	    if kind = WidgetGlobals.INDENT then
		topLeftColour := darkgrey
		bottomRightColour := illuminatedColor
	    else
		topLeftColour := illuminatedColor
		bottomRightColour := darkgrey
	    end if
	    if drawTop then
		for offset : 0 .. riserSize - 1
		    Draw.Line (x + offset, y + offset, x2 - offset,
			y + offset, bottomRightColour)
		    Draw.Line (x2 - offset, y + offset, x2 - offset,
			y2 - offset, bottomRightColour)
		    Draw.Line (x + offset, y + offset, x + offset,
			y2 - offset, topLeftColour)
		    Draw.Line (x + offset, y2 - offset, x + labelX - 4,
			y2 - offset, topLeftColour)
		    Draw.Line (x + labelX + labelWidth + 3, y2 - offset,
			x2 - offset, y2 - offset, topLeftColour)
		end for
	    else
		for offset : 0 .. riserSize - 1
		    Draw.Line (x + offset, y + offset, x2 - offset,
			y + offset, bottomRightColour)
		    Draw.Line (x2 - offset, y + offset, x2 - offset,
			y + labelY - offset, bottomRightColour)
		    Draw.Line (x + offset, y + offset, x + offset,
			y + labelY - offset, topLeftColour)
		end for
	    end if
	end if
	Font.Draw (text, x + labelX, y + labelY, textFontID, 
	    foregroundColour)
    end DrawWidget


    body procedure EraseWidget
	if not drawn then
	    ComputeWidgetPosition
	end if
	var drawTop : boolean := labelWidth < originalWidth - 8
	var x2 : int := x + originalWidth
	var y2 : int := y + originalHeight
	if kind = WidgetGlobals.LINE then
	    Draw.Line (x, y, x2, y, backgroundColour)
	    if drawTop then
		Draw.Line (x, y, x, y2, backgroundColour)
		Draw.Line (x2, y, x2, y2, backgroundColour)
		Draw.Line (x, y2, x + labelX - 4, y2, backgroundColour)
		Draw.Line (x + labelX + labelWidth + 3, y2, x2, y2,
		    backgroundColour)
	    else
		Draw.Line (x, y, x, y + labelY - 2, backgroundColour)
		Draw.Line (x2, y, x2, y + labelY - 2, backgroundColour)
	    end if
	else
	    var topLeftColour, bottomRightColour : int
	    if kind = WidgetGlobals.INDENT then
		topLeftColour := darkgrey
		bottomRightColour := illuminatedColor
	    else
		topLeftColour := illuminatedColor
		bottomRightColour := darkgrey
	    end if
	    if drawTop then
		for offset : 0 .. riserSize - 1
		    Draw.Line (x + offset, y + offset, x2 - offset,
			y + offset, backgroundColour)
		    Draw.Line (x2 - offset, y + offset, x2 - offset,
			y2 - offset, backgroundColour)
		    Draw.Line (x + offset, y + offset, x + offset,
			y2 - offset, backgroundColour)
		    Draw.Line (x + offset, y2 - offset, x + labelX - 4,
			y2 - offset, backgroundColour)
		    Draw.Line (x + labelX + labelWidth + 3, y2 - offset,
			x2 - offset, y2 - offset, backgroundColour)
		end for
	    else
		for offset : 0 .. riserSize - 1
		    Draw.Line (x + offset, y + offset, x2 - offset,
			y + offset, backgroundColour)
		    Draw.Line (x2 - offset, y + offset, x2 - offset,
			y + labelY - offset, backgroundColour)
		    Draw.Line (x + offset, y + offset, x + offset,
			y + labelY - offset, backgroundColour)
		end for
	    end if
	end if
	Font.Draw (text, x + labelX, y + labelY, textFontID, 
	    backgroundColour)
    end EraseWidget
end LabelledFrameClass

