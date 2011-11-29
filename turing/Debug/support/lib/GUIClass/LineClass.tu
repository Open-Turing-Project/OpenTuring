unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% LineClass - Class that implements horizontal and vertical lines much as they
%             appear in MS Windows 3.1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of LineClass is as follows:
%   procedure Show - Display the widget.
%   procedure Hide - Hide the widget.
%   procedure Dispose - Hide the widget and free any data structures allocated
%                       by the class.  To be called before the Widget is freed.
%   procedure SetPosition (x, y : int) - Move the widget to (x, y).
%   procedure SetSize (width, height : int) - Resize the widget.
%   procedure SetPositionAndSize (x, y, width, height : int) - Move and resize
%                                                              the widget.
%   procedure Initialize (x1, y1, x2, y2, kind : int) - Create a line from
%                                                       (x1, y2) to (x2, y2).
%                       kind - One of GUI.NONE, GUI.INDENT or GUI.EXDENT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of LineClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%   procedure DrawText (clr : int) - Draws the label in specified colour.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of LineClass are as follows:
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of LineClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class LineClass
    inherit GenericWidgetBodyClass

    import WidgetGlobals

    %
    % Exported routines for LineClass
    %
    export
    % The external interface
	Initialize

    %
    % The inheritable variables of LineClass
    %
    var kind : int


    %
    % The external subprograms of LineClass
    %
    procedure Initialize (newX1, newY1, newX2, newY2, newKind : int)
	SetPositionAndSize (min (newX1, newX2), min (newY1, newY2),
	    max (newX1, newX2) - min (newX1, newX2),
	    max (newY1, newY2) - min (newY1, newY2))
	kind := TranslateBorder (newKind)

	GeneralInitialize
    end Initialize


    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	if originalWidth not= 0 and originalHeight not= 0 then
	    AssertFailed ("Line must be horizontal or vertical")
	end if
	x := originalX
	y := originalY
	if originalWidth = 0 then
	    height := originalHeight + 1
	    if kind = WidgetGlobals.LINE then
		width := 1
	    else
		width := 2 * riserSize
	    end if
	else
	    width := originalWidth + 1
	    if kind = WidgetGlobals.LINE then
		height := 1
	    else
		height := 2 * riserSize
	    end if
	end if
	drawn := true
    end ComputeWidgetPosition


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if

	if kind = WidgetGlobals.LINE then
	    Draw.Line (x, y, x + originalWidth, y + originalHeight, 
		foregroundColour)
	else
	    var topLeftColour, bottomRightColour : int
	    if kind = WidgetGlobals.INDENT then
		topLeftColour := darkgrey
		bottomRightColour := illuminatedColor
	    else
		topLeftColour := illuminatedColor
		bottomRightColour := darkgrey
	    end if
	    if originalWidth = 0 then
		% Draw vertical
		for offset : 0 .. riserSize - 1
		    Draw.Line (x + offset, y, x + offset, y + originalHeight,
			topLeftColour)
		    Draw.Line (x + riserSize + offset, y,
			x + riserSize + offset, y + originalHeight,
			bottomRightColour)
		end for
	    else
		% Draw horizontal
		for offset : 0 .. riserSize - 1
		    Draw.Line (x, y + offset, x + width, y + offset,
			bottomRightColour)
		    Draw.Line (x, y + riserSize + offset,
			x + width, y + riserSize + offset, topLeftColour)
		end for
	    end if
	end if
    end DrawWidget
end LineClass

