unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericWidgetClass - Abstract class that implements widgets
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of GenericWidgetClass is as follows:
%   procedure Show - Display the widget.
%   procedure Hide - Hide the widget.
%   procedure Dispose - Hide the widget and free any data structures allocated
%                       by the class.  To be called before the Widget is freed.
%   procedure SetPosition (x, y : int) - Move the widget to (x, y).
%   procedure SetSize (width, height : int) - Resize the widget.
%   procedure SetPositionAndSize (x, y, width, height : int) - Move and resize
%                                                              the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of GenericWidgetClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of GenericWidgetClass are as follows:
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
% The inheritable constants of GenericWidgetClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
%   procedure DrawWidget - Draw the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericWidgetClass
    implement by GenericWidgetBodyClass
    
    %
    % Exported routines for GenericWidgetClass
    %
    export
    % The external interface
	Show, Hide, Refresh, GetX, GetY, GetWidth, GetHeight, SetColour,
	Dispose, SetPosition, SetSize, SetPositionAndSize,
	SetUserValue, GetUserValue, SetUserObject, GetUserObject


    %
    % External subprograms
    %
    deferred procedure Show
    deferred procedure Hide
    deferred procedure Refresh
    deferred function GetX : int
    deferred function GetY : int
    deferred function GetWidth : int
    deferred function GetHeight : int
    deferred procedure SetColour (newColour : int)
    deferred procedure Dispose
    deferred procedure SetPosition (newX, newY : int)
    deferred procedure SetSize (newWidth, newHeight : int)
    deferred procedure SetPositionAndSize (newX, newY : int,
	newWidth, newHeight : int)
    deferred procedure SetUserValue (value : int)
    deferred function GetUserValue : int
    deferred procedure SetUserObject (object : ^anyclass)
    deferred function GetUserObject : ^anyclass
end GenericWidgetClass

