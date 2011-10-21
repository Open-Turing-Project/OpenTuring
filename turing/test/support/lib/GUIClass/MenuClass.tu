unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MenuClass - Implements a single menu in OOT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of MenuClass is as follows:
%   procedure AddMenuItem (menuItem, afterMenuItem : ^MenuItemClass)
%                       menuItem - The menu item to be added to the menu.
%                       afterMenuItem - The menu item will be added after this
%                                       item.
%                       Adds a menu item to the menu.
%   procedure AppendMenuItem (menuItem : ^MenuItemClass)
%                       menuItem - The menu item to be appended to the menu.
%                       Adds a menu item to the menu after the last menu item.
%   procedure Initialize (newName : string) - Sets the menu's name.
%   procedure Enable - Enable (activate) the menu.
%   procedure Disable - Disable (deactivate) the menu.
%   procedure Show - Does nothing.
%   procedure Hide - Does nothing.
%   procedure Dispose - Does nothing.
%   procedure SetPosition (x, y : int) - Does nothing.
%   procedure SetSize (width, height : int) - Does nothing.
%   procedure SetPositionAndSize (x, y, width, height : int) - Does nothing.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of MenuClass is as follows:
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
%   procedure DrawMenu (draw : boolean) -
%   procedure DrawMenuTitle -
%   procedure Hilite (hilite : boolean) -
%   function InMenu (x : int) : boolean -
%   procedure SetMenuBarLine (newMenuLine : int) -
%   function SetWidth (offset : int) : int -
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of MenuClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of MenuClass are as follows:
%   enabled - Is the widget enabled (activated).
%   name - The menu name.
%   start, finish -
%   menuHeight -
%   maxItemWidth -
%   maxShortFormWidth -
%   menuLine -
%   selectedMenuItem -
%   firstMenuItemNode -
%   menuPic, menuPicX, menuPicY - Pic ID and location of screen under menu.
%   menuX1, menuY1, menuX2, menuY2 - Menu coordinates.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class MenuClass
    inherit GenericActiveWidgetClass

    import MenuItemClass, WidgetGlobals

    export
    % The external interface
	AddMenuItem, AppendMenuItem, Initialize,
    % The package interface
	DrawMenu, DrawMenuTitle, Hilite, InMenu,
	SetMenuBarLine, SetWidth

    %
    % The inheritable types of MenuClass
    %
    type MenuItemNode :
	record
	    menuItem : ^MenuItemClass
	    next : ^MenuItemNode
	end record


    %
    % The inheritable variables of MenuClass
    %
    var name : string (40)
    var start, finish : int
    var menuHeight : int := 0
    var maxItemWidth : int := 0
    var maxShortFormWidth : int := 0
    var menuLine : int
    %    var selectedMenuItem : ^MenuItemClass
    var firstMenuItemNode : ^MenuItemNode := nil
    % Pic ID and location of screen under menu
    var menuPic, menuPicX, menuPicY : int
    % Menu Coordinates
    var menuX1, menuY1, menuX2, menuY2 : int := -1


    %
    % The package subprograms of MenuClass
    %
    procedure DrawMenu (draw : boolean)
	var menuItemNode : ^MenuItemNode
	if firstMenuItemNode = nil then
	    return
	end if

	if not draw then
	    Pic.Draw (menuPic, menuPicX, menuPicY, picCopy)
	    menuPic := 0
	    return
	end if

	menuX1 := start
	menuY1 := menuLine - menuHeight
	menuX2 := menuX1 + maxItemWidth + 31
	if maxShortFormWidth > 0 then
	    menuX2 += 10 + maxShortFormWidth
	end if
	menuY2 := menuLine - 1
	menuPic := Pic.New (menuX1, menuY1 - 1, menuX2 + 1, menuY2)
	menuPicX := menuX1
	menuPicY := menuY1 - 1

	Draw.Line (menuX1, menuY1, menuX2, menuY1, black)
	Draw.Line (menuX1, menuY1, menuX1, menuY2, black)
	Draw.Line (menuX2, menuY1, menuX2, menuY2, black)
	Draw.Line (menuX2 + 1, menuY1 - 1, menuX2 + 1, menuY2, darkgrey)
	Draw.Line (menuX1, menuY1 - 1, menuX2 + 1, menuY1 - 1, darkgrey)
	Draw.FillBox (menuX1 + 1, menuY1 + 1, menuX2 - 1, menuY2,
	    WidgetGlobals.MENU_COLOUR)

	menuItemNode := firstMenuItemNode
	loop
	    exit when menuItemNode = nil
	    menuItemNode -> menuItem -> Display (menuX1 + 1, menuX2 - 1,
		maxItemWidth, enabled)
	    menuItemNode := menuItemNode -> next
	end loop
    end DrawMenu


    procedure DrawMenuTitle
	var menuColour : int
	if enabled then
	    menuColour := WidgetGlobals.MENU_TEXT_COLOUR
	else
	    menuColour := WidgetGlobals.MENU_DISABLED_TEXT_COLOUR
	end if
	Font.Draw (name, start + WidgetGlobals.X_OFFSET - 2,
	    menuLine + 2 + WidgetGlobals.menuFontDescent,
	    WidgetGlobals.menuFontID, menuColour)
    end DrawMenuTitle


    procedure Hilite (hilite : boolean)
	if hilite then
	    % Hilite the menu
	    var menuColour : int
	    if enabled then
		menuColour := white
	    else
		menuColour := WidgetGlobals.MENU_DISABLED_TEXT_COLOUR
	    end if
	    Draw.FillBox (start, menuLine + 1, finish, maxy - 1, blue)
	    Font.Draw (name, start + WidgetGlobals.X_OFFSET - 2,
		menuLine + 2 + WidgetGlobals.menuFontDescent, 
		WidgetGlobals.menuFontID, menuColour)
	else
	    % Unhilite the menu
	    Draw.FillBox (start, menuLine + 1, finish,
		maxy - 1, WidgetGlobals.MENU_COLOUR)
	    DrawMenuTitle
	end if
    end Hilite


    function InMenu (x : int) : boolean
	if x <= finish then
	    result true
	else
	    result false
	end if
    end InMenu


    procedure SetMenuBarLine (newMenuLine : int)
	menuLine := newMenuLine
    end SetMenuBarLine


    function SetWidth (offset : int) : int
	start := offset
	finish := offset + 2 * WidgetGlobals.X_OFFSET +
	    Font.Width (name, WidgetGlobals.menuFontID)
	result finish + 2
    end SetWidth


    %
    % The external subprograms of MenuClass
    %
    procedure AddMenuItem (menuItem, afterMenuItem : ^MenuItemClass)
	var newMenuItemNode, menuItemNode : ^MenuItemNode
	var baseLine : int
	var menuItemHeight, menuItemWidth, menuItemShortFormWidth : int

	% Create a menu node and place it in the linked list
	new newMenuItemNode
	newMenuItemNode -> menuItem := menuItem

	if afterMenuItem = nil then
	    newMenuItemNode -> next := firstMenuItemNode
	    firstMenuItemNode := newMenuItemNode
	else
	    menuItemNode := firstMenuItemNode
	    loop
		exit when menuItemNode = nil or
		    menuItemNode -> menuItem = afterMenuItem
		menuItemNode := menuItemNode -> next
	    end loop
	    if menuItemNode = nil then
		AssertFailed ("Menu Item ID not found")
	    end if
	    newMenuItemNode -> next := menuItemNode -> next
	    menuItemNode -> next := newMenuItemNode
	end if

	menuItemNode := firstMenuItemNode
	baseLine := menuLine - WidgetGlobals.ITEM_HEIGHT + 1
	menuHeight := 0
	maxItemWidth := 0
	maxShortFormWidth := 0
	loop
	    exit when menuItemNode = nil
	    menuItemNode -> menuItem -> GetSize (baseLine, menuItemHeight,
		menuItemWidth, menuItemShortFormWidth)
	    menuHeight += menuItemHeight
	    maxItemWidth := max (maxItemWidth, menuItemWidth)
	    maxShortFormWidth := max (maxShortFormWidth,
		menuItemShortFormWidth)
	    menuItemNode := menuItemNode -> next
	end loop
    end AddMenuItem


    procedure AppendMenuItem (menuItem : ^MenuItemClass)
	if firstMenuItemNode = nil then
	    AddMenuItem (menuItem, nil)
	else
	    var menuItemNode : ^MenuItemNode := firstMenuItemNode
	    loop
		exit when menuItemNode -> next = nil
		menuItemNode := menuItemNode -> next
	    end loop
	    AddMenuItem (menuItem, menuItemNode -> menuItem)
	end if
    end AppendMenuItem


    procedure Initialize (newName : string)
	name := newName

	GeneralInitialize
    end Initialize


    %
    % Overridden subprograms
    %
    body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	var menuItemNode : ^MenuItemNode
	var menuItem, selectedMenuItem : ^MenuItemClass := nil
	var mx, my, b : int

	Hilite (true)
	DrawMenu (true)

	selectedMenuItem := nil

	loop
	    Mouse.Where (mx, my, b)
	    exit when my >= maxy - WidgetGlobals.MENU_BAR_HEIGHT and
		(mx < start or finish < mx)

	    if menuX1 <= mx and mx <= menuX2 and
		    menuY1 <= my and my <= menuY2 then
		menuItemNode := firstMenuItemNode
		menuItem := nil
		loop
		    exit when menuItemNode = nil
		    if menuItemNode -> menuItem -> InMenuItem (my) and
			    enabled then
			menuItem := menuItemNode -> menuItem
			exit
		    end if
		    menuItemNode := menuItemNode -> next
		end loop
	    else
		menuItem := nil
	    end if
	    if menuItem not= selectedMenuItem then
		if selectedMenuItem not= nil then
		    selectedMenuItem -> Hilite (false, menuX1 + 1,
			menuX2 - 1, maxItemWidth)
		end if
		selectedMenuItem := menuItem
		if selectedMenuItem not= nil then
		    selectedMenuItem -> Hilite (true, menuX1 + 1, menuX2 - 1,
			maxItemWidth)
		end if
	    end if
	    exit when b = 0
	end loop

	Hilite (false)
	DrawMenu (false)

	if b = 0 then
	    if selectedMenuItem not= nil then
		selectedMenuItem -> GeneralActionProc
	    end if
	    result true
	end if

	result false
    end ConsiderButtonDown


    body function ConsiderKeystroke (key : char) : boolean
	var menuItemPtr : ^MenuItemNode := firstMenuItemNode

	if not enabled then
	    result false
	end if

	% Check each menu item for the keystroke
	loop
	    exit when menuItemPtr = nil
	    if menuItemPtr -> menuItem -> ConsiderKeystroke (key) then
		result true
	    end if
	    menuItemPtr := menuItemPtr -> next
	end loop

	result false
    end ConsiderKeystroke


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


    body procedure Disable
	enabled := false
	DrawMenuTitle
    end Disable


    body procedure Enable
	enabled := true
	DrawMenuTitle
    end Enable
end MenuClass

