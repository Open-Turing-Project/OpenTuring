unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MenuItemClass - Implements a single menu item in OOT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of MenuItemClass is as follows:
%   procedure Initialize (menuItemName : string,
%           newActionProc : procedure x ()) -
%   procedure InitializeFull (menuItemName : string,
%           newActionProc : procedure x (), newShortFormChar : char) -
%   procedure Enable - Enable (activate) the menu.
%   procedure Disable - Disable (deactivate) the menu.
%   procedure Show - Does nothing.
%   procedure Hide - Does nothing.
%   procedure Dispose - Does nothing.
%   procedure SetPosition (x, y : int) - Does nothing.
%   procedure SetSize (width, height : int) - Does nothing.
%   procedure SetPositionAndSize (x, y, width, height : int) - Does nothing.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of MenuItemClass is as follows:
%   function ConsiderButtonDown (x, y : int) : boolean
%                       x, y - The location of of the mouse when the button
%                              was pressed.
%                       Handle the mouse button being in the menu.  If the
%                       mouse up occurs in a menu item, call the menu item's
%                       userActionProc and return true. If the mouse strays off
%                       the menu, return false.
%   function ConsiderKeystroke (key : char) : boolean
%                       key - The key pressed.
%                       Handle a keystroke. If the keystroke is the
%                       shortcut for a menu item in this menu, the userActionProc
%                       for the menu item is called and the function returns
%                       true, otherwise it returns false.
%   procedure DrawWidget - Does nothing.
%   procedure Display (left, right, maxItemWidth : int) -
%   procedure GetSize (var newBaseLine, menuItemHeight, menuItemWidth,
%           menuItemShortFormWidth : int) -
%   procedure Hilite (hilite : boolean, left, right, maxItemWidth : int) -
%   function InMenuItem (mouseY : int) : boolean -
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of MenuItemClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package variables of MenuItemClass are as follows:
%   userActionProc - The procedure to call if the menu item is selected.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of MenuItemClass are as follows:
%   enabled - Is the widget enabled (activated).
%   name - The name of the menu item.
%   shortFormString - The short cut as it appears on the menu.
%   shortFormChar - The actual character for the menu item's short cut.
%   baseLine - The y coordinate of any text drawn.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class MenuItemClass
    inherit GenericActiveWidgetClass

    import WidgetGlobals

    export
    % The external interface
	Initialize, InitializeFull,
    % The package interface
	Display, GetSize, Hilite, InMenuItem


    %
    % The inheritable variables of MenuClass
    %
    var name : string (40)
    var shortFormString : string (40)
    var shortFormChar : char
    var userActionProc : procedure userActionProc ()
    var baseLine : int


    procedure Display (left, right, maxItemWidth : int, menuEnabled : boolean)
	var menuColour : int
	if menuEnabled and enabled then
	    menuColour := WidgetGlobals.MENU_TEXT_COLOUR
	else
	    menuColour := WidgetGlobals.MENU_DISABLED_TEXT_COLOUR
	end if
	if name = "---" then
	    Draw.Line (left, baseLine, right, baseLine, menuColour)
	else
	    Font.Draw (name, left + 15,
		baseLine + WidgetGlobals.menuFontDescent,
		WidgetGlobals.menuFontID, menuColour)
	    if shortFormString not= "" then
		Font.Draw (shortFormString, left + maxItemWidth + 25,
		    baseLine + WidgetGlobals.menuFontDescent,
		    WidgetGlobals.menuFontID, menuColour)
	    end if
	end if
    end Display


    procedure GetSize (var newBaseLine, menuItemHeight, menuItemWidth,
	    menuItemShortFormWidth : int)
	if name = "---" then
	    menuItemHeight := 7
	    baseLine := newBaseLine + 13
	    newBaseLine -= 7
	    menuItemWidth := 0
	    menuItemShortFormWidth := 0
	else
	    menuItemHeight := WidgetGlobals.ITEM_HEIGHT
	    baseLine := newBaseLine
	    newBaseLine -= WidgetGlobals.ITEM_HEIGHT
	    menuItemWidth := Font.Width (name, WidgetGlobals.menuFontID)
	    if shortFormString = "" then
		menuItemShortFormWidth := 0
	    else
		menuItemShortFormWidth := Font.Width (shortFormString,
		    WidgetGlobals.menuFontID)
	    end if
	end if
    end GetSize


    procedure Hilite (hilite : boolean, left, right, maxItemWidth : int)
	if hilite then
	    % Hilite the menu item
	    Draw.FillBox (left + 1, baseLine,
		right - 1, baseLine + WidgetGlobals.ITEM_HEIGHT - 2, blue)
	    Font.Draw (name, left + 15,
		baseLine + WidgetGlobals.menuFontDescent,
		WidgetGlobals.menuFontID,
		WidgetGlobals.MENU_HILITE_TEXT_COLOUR)
	    if shortFormString not= "" then
		Font.Draw (shortFormString, left + maxItemWidth + 25,
		    baseLine + WidgetGlobals.menuFontDescent,
		    WidgetGlobals.menuFontID,
		    WidgetGlobals.MENU_HILITE_TEXT_COLOUR)
	    end if
	else
	    % Unhilite the menu
	    Draw.FillBox (left + 1, baseLine,
		right - 1, baseLine + WidgetGlobals.ITEM_HEIGHT - 2,
		WidgetGlobals.MENU_COLOUR)
	    Display (left, right, maxItemWidth, true)
	end if
    end Hilite


    function InMenuItem (mouseY : int) : boolean
	if name = "---" or not enabled then
	    result false
	end if

	if baseLine <= mouseY and
		mouseY <= baseLine + WidgetGlobals.ITEM_HEIGHT - 1 then
	    result true
	end if

	result false
    end InMenuItem


    %
    % The external subprograms of MenuItemClass
    %
    procedure InitializeFull (menuItemName : string,
	    newActionProc : procedure x (), newShortFormChar : char)
	name := menuItemName
	shortFormString := WidgetGlobals.charNames (newShortFormChar)
	shortFormChar := newShortFormChar
	userActionProc := newActionProc

	GeneralInitialize
    end InitializeFull


    procedure Initialize (menuItemName : string,
	    newActionProc : procedure x ())
	InitializeFull (menuItemName, newActionProc, '\0')
    end Initialize



    %
    % Overridden subprograms
    %
    body procedure ActionProc
	userActionProc
    end ActionProc


    body function ConsiderKeystroke (key : char) : boolean
	if key = shortFormChar then
	    GeneralActionProc
	    result true
	end if
	result false
    end ConsiderKeystroke


    body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	result false
    end ConsiderButtonDown


    body procedure DrawWidget
    end DrawWidget


    body procedure Show
    end Show


    body procedure Hide
    end Hide


    body procedure Dispose
    end Dispose


    body procedure SetPosition (newX, newY : int)
    end SetPosition


    body procedure SetSize (newWidth, newHeight : int)
    end SetSize


    body procedure SetPositionAndSize (newX, newY, newWidth, newHeight : int)
    end SetPositionAndSize


    body procedure DrawWidget
    end DrawWidget


    body procedure EraseWidget
    end EraseWidget
end MenuItemClass

