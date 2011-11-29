unit 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericRadioButtonClass - Abstract class that implements radio buttons.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of GenericRadioButtonClass is as follows:
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
%   procedurre Select (isSelected : boolean)
%                       isDefault - Whether to set the button to default or
%                                   not.
%   procedure GetSpecs (var x, y, alignment : int)
%                       x, y - The returned position of the radio button.
%                       alignment - The returned alignment of the radio button.
%   function GetVerticalSeparation : int - Return the vertical separation of
%                       two radio buttons.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of GenericRadioButtonClass is as follows:
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
% The inheritable subprograms of GenericRadioButtonClass are as follows:
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
%   procedure SetProcs (shortCut : char, actionProc : procedure x ())
%                       selectActionProc : procedure x (widget : ^Widget))
%                       shortCut - The keystroke to use as the short cut.
%                                  Use '\0' if no short cut wanted.
%                       actionProc - The procedure to be called when the radio
%                                    button is pressed.
%                       selectActionProc - The procedure to be called when the
%                                          radio button is selected.  This
%                                          procedure should deselect all other
%                                          buttons in the group.
%                       Initialize the radio button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of GenericRadioButtonClass are as follows:
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
%   userActionProc - The procedure to call when the button is pushed.
%   selectActionProc - The procedure call when the radio button is selected to
%                      deselect all other buttons in group.
%   shortCutChar - The keystroke shortcut to press the button.
%   selected - Whether the button is the currently select on in group.
%   alignment - Whether the circle appears to right or left of text.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of GenericRadioButtonClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   labelFontID - The font ID number for the label.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericRadioButtonClass
    inherit GenericButtonClass

    import WidgetGlobals

    %
    % Exported routines for ButtonClass
    %
    export
    % The external interface
	Select, GetSpecs,
    % The package interface
	GetFirstRadio, GetNextRadio, GetSelectedRadio,
	SetFirstRadio, SetNextRadio, SetSelectedRadio,
	DrawSelect


    %
    % The inheritable variables of GenericRadioButtonClass
    %
    var userActionProc : procedure actionProc ()
    var selectActionProc : procedure selectActionProc (
	widget : ^GenericWidgetClass)
    var selected : boolean := false
    var alignment : int
    var firstRadioButton : ^GenericRadioButtonClass
    var nextRadioButton : ^GenericRadioButtonClass := nil
    var selectedRadioButton : ^GenericRadioButtonClass


    forward procedure DrawSelect (isSelected : boolean)
    forward function GetFirstRadio : ^GenericRadioButtonClass
    forward function GetNextRadio : ^GenericRadioButtonClass
    forward function GetSelectedRadio : ^GenericRadioButtonClass
    forward procedure SetFirstRadio (newFirst : ^GenericRadioButtonClass)
    forward procedure SetNextRadio (newNext : ^GenericRadioButtonClass)


    %
    % The inheritable subprograms of GenericRadioButtonClass
    %
    procedure InitRadio (newActionProc : procedure x (),
	    shortCut : char, joinRadio : ^GenericRadioButtonClass)
	if joinRadio = nil then
	    firstRadioButton := self
	    selectedRadioButton := self
	    DrawSelect (true)
	else
	    if joinRadio -> GetNextRadio not= nil then
		AssertFailed ("Radio button to be joined already has a " +
		    "radio button joined to it.")
	    end if
	    firstRadioButton := joinRadio -> GetFirstRadio
	    selectedRadioButton := joinRadio -> GetSelectedRadio
	    joinRadio -> SetNextRadio (self)
	end if
	shortCutChar := shortCut
	userActionProc := newActionProc
    end InitRadio


    %
    % The package subprograms of GenericActiveWidgetClass
    %
    procedure SetRadioSet (newFirst, newNext, newSelect :
	    ^GenericRadioButtonClass)
	firstRadioButton := newFirst
	nextRadioButton := newNext
	selectedRadioButton := newSelect
    end SetRadioSet


    body function GetFirstRadio : ^GenericRadioButtonClass
	result firstRadioButton
    end GetFirstRadio


    body function GetNextRadio : ^GenericRadioButtonClass
	result nextRadioButton
    end GetNextRadio


    body function GetSelectedRadio : ^GenericRadioButtonClass
	result selectedRadioButton
    end GetSelectedRadio


    body procedure SetFirstRadio (newFirst : ^GenericRadioButtonClass)
	firstRadioButton := newFirst
    end SetFirstRadio


    body procedure SetNextRadio (newNext : ^GenericRadioButtonClass)
	nextRadioButton := newNext
    end SetNextRadio


    procedure SetSelectedRadio (newSelect : ^GenericRadioButtonClass)
	selectedRadioButton := newSelect
    end SetSelectedRadio


    %
    % The external subprograms of GenericRadioButtonClass
    %
    body procedure DrawSelect (isSelected : boolean)
	if showing then
	    EraseWidget
	    selected := isSelected
	    DrawWidget
	else
	    selected := isSelected
	end if
    end DrawSelect


    procedure GetSpecs (var returnX, returnY, returnDirection : int)
	returnX := originalX
	returnY := originalY
	returnDirection := alignment
    end GetSpecs


    procedure Select
	GeneralActionProc
    end Select


    %
    % Overridden subprograms
    %
    body procedure ActionProc
	if not selected then
	    selected := true
	    DrawUnpressedButton

	    % Deselect the currently selected radio button.
	    selectedRadioButton -> DrawSelect (false)

	    % Let all the other radio buttons in group know new selected button.
	    var radioButton : ^GenericRadioButtonClass := firstRadioButton
	    loop
		exit when radioButton = nil
		radioButton -> SetSelectedRadio (self)
		radioButton := radioButton -> GetNextRadio
	    end loop

	    userActionProc
	else
	    DrawUnpressedButton
	end if
    end ActionProc


    body procedure Dispose
	if firstRadioButton = self then
	    % Let all the other radio buttons in group know new first button.
	    var radioButton : ^GenericRadioButtonClass := firstRadioButton
	    loop
		exit when radioButton = nil
		radioButton -> SetFirstRadio (nextRadioButton)
		radioButton := radioButton -> GetNextRadio
	    end loop
	else
	    % Let all the other radio buttons in group know new first button.
	    var radioButton : ^GenericRadioButtonClass := firstRadioButton
	    var prevRadio : ^GenericRadioButtonClass
	    loop
		exit when radioButton = self
		prevRadio := radioButton
		radioButton := radioButton -> GetNextRadio
	    end loop
	    prevRadio -> SetNextRadio (nextRadioButton)
	end if

	if selected and firstRadioButton not= nil then
	    firstRadioButton -> DrawSelect (true)
	    % Let all the other radio buttons in group know new selected button.
	    var radioButton : ^GenericRadioButtonClass := firstRadioButton
	    loop
		exit when radioButton = nil
		radioButton -> SetSelectedRadio (firstRadioButton)
		radioButton := radioButton -> GetNextRadio
	    end loop
	end if
	GenericButtonClass.Dispose
    end Dispose
end GenericRadioButtonClass

