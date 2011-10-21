unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericActiveWidgetClass - Abstract class that implements widgets that are 
%                            active (i.e. respond to mouse presses, key 
%                            strokes.)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of GenericActiveWidgetClass is as follows:
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of GenericActiveWidgetClass is as follows:
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
% The inheritable subprograms of GenericActiveWidgetClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of GenericActiveWidgetClass are as follows:
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of GenericActiveWidgetClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
%   function ConsiderButtonDown (x, y : int) : boolean
%   function ConsiderKeystroke (key : char) : boolean
%   procedure DrawWidget - Draw the widget.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericActiveWidgetClass
    inherit GenericWidgetBodyClass

    %
    % Exported routines for GenericActiveWidgetClass
    %
    export
    % The external interface
	Enable, Disable,

    % The package interface
	ConsiderButtonDown, ConsiderKeystroke, GeneralActionProc

    %
    % The inheritable variables of GenericActiveWidgetClass
    %
    % Is the widget enabled (activated)
    var enabled : boolean


    %
    % The inheritable subprograms of GenericActiveWidgetClass
    %
    deferred procedure ActionProc
    
    procedure GeneralActionProc
	WidgetGlobals.selectedWidget := self
	ActionProc
	WidgetGlobals.selectedWidget := nil
    end GeneralActionProc
    
    
    %
    % The package subprograms of GenericActiveWidgetClass
    %
    deferred function ConsiderButtonDown (mouseX, mouseY : int) : boolean
    deferred function ConsiderKeystroke (key : char) : boolean


    %
    % The external subprograms of the GenericActiveWidgetClass
    %
    procedure Disable
	if showing then
	    EraseWidget
	    enabled := false
	    DrawWidget
	else
	    enabled := false
	end if
    end Disable


    procedure Enable
	if showing then
	    EraseWidget
	    enabled := true
	    DrawWidget
	else
	    enabled := true
	end if
    end Enable
    
    
    % 
    % Overridden subprograms
    %
    body procedure GeneralInitialize
	enabled := true
	GenericWidgetBodyClass.GeneralInitialize
    end GeneralInitialize
end GenericActiveWidgetClass

