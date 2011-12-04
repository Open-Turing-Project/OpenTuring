unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MenuBarClass - Class that implements menus in OOT.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of MenuBarClass empty.  The MenuBarClass should not
% be used directly by external programs.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of MenuBarClass is as follows:
%   procedure AddMenu (menu, afterMenu : ^MenuClass) - Add a MenuClass object
%                                                      to the menu bar.  Place
%                                                      it after afterMenu.
%   procedure AppendMenu (menu : ^MenuClass) - Add a MenuClass object to the
%                                              menu bar after all other menus.
%   function ConsiderButtonDown (x, y : int) : boolean
%                       x, y - The location of of the mouse when the button
%                              was pressed.
%                       Handle the mouse button being pressed.  If it occured
%                       in the menu bar, handle the event.  Return true if the
%                       event is handled by this function.  Return false
%                       otherwise.  If a menu item was selected, call the
%                       actionProc for the menu item.
%   function ConsiderKeystroke (key : char) : boolean
%                       key - The key pressed.
%                       Handle a keystroke. If the keystroke is the
%                       shortcut for a menu item, the actionProc for the
%                       menu item is called and the function returns true,
%                       otherwise it returns false.
%   function GetLastMenu : ^MenuClass - Return the last menu added to the menu
%                                       bar.
%   function GetSelectedMenuItem : ^WidgetClass - Return the currently
%                                                 selected menu item.  Used
%                                                 by the GUI module to get the
%                                                 id of the menu item.
%   procedure Hide - Hide the menu bar.
%   procedure Show - Display the menu bar.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of MenuBarClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawBar - Draw the menu bar.
%   function FindMenu (x : int) : ^MenuClass - Return the menu which is found
%                                              at position (x, maxy) in the 
%                                              menu bar.
%   procedure Initialize - Initialize the menu bar.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of MenuBarClass are as follows:
%   initialized - Whether the menu bar has been initialized.
%   showing - Whether the menu bar is currently visible.
%   menuLine - The y coordinate of the bottom of the menu bar.
%   selectedMenu - The currently selected menu.
%   firstMenuNode - the first in the linked list of MenuNodes.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class MenuBarClass
    import MenuClass, GenericWidgetClass, WidgetGlobals

    export 
	% The package interface
	AddMenu, AppendMenu, ConsiderButtonDown, ConsiderKeystroke, 
	GetLastMenu, Hide, Show 

    %
    % The inheritable types of MenuBarClass
    %
    type MenuNode :
	record
	    menu : ^MenuClass
	    next : ^MenuNode
	end record


    %
    % The inhertiable variables of MenuBarClass
    %
    var initialized : boolean := false
    var showing : boolean := true
    var menuLine : int
    var selectedMenu : ^MenuClass
    var firstMenuNode : ^MenuNode


    %
    % The inheritable subprograms of MenuBarClass
    %
    procedure AssertFailed (msg : string)
	Window.Select (defWinId)
	locate (1, 1)
	put msg
	assert false
    end AssertFailed


    procedure DrawBar
	var menuNode : ^MenuNode := firstMenuNode

	if not initialized or not showing then
	    return
	end if

	Draw.Line (0, maxy, maxx, maxy, black)

	Draw.FillBox (0, menuLine + 1, maxx, maxy - 1,
	    WidgetGlobals.MENU_COLOUR)
	Draw.Line (0, menuLine, maxx, menuLine, black)

	loop
	    exit when menuNode = nil
	    menuNode -> menu -> DrawMenuTitle
	    menuNode := menuNode -> next
	end loop
    end DrawBar


    function FindMenu (x : int) : ^MenuClass
	var menuNode : ^MenuNode := firstMenuNode

	loop
	    exit when menuNode = nil
	    if menuNode -> menu -> InMenu (x) then
		result menuNode -> menu
	    end if
	    menuNode := menuNode -> next
	end loop

	result nil
    end FindMenu


    procedure Initialize
	if not initialized then
	    initialized := true
	    firstMenuNode := nil
	    menuLine := maxy - WidgetGlobals.MENU_BAR_HEIGHT
	end if
    end Initialize


    %
    % The package subprograms of MenuBarClass
    %
    procedure AddMenu (menu, afterMenu : ^MenuClass)
	var newMenuNode, menuNode : ^MenuNode
	var offset : int

	Initialize

	% Set the menu bar that the menu has been placed in
	menu -> SetMenuBarLine (menuLine)

	% Create a menu node and place it in the linked list
	new newMenuNode
	newMenuNode -> menu := menu

	if afterMenu = nil then
	    newMenuNode -> next := firstMenuNode
	    firstMenuNode := newMenuNode
	else
	    menuNode := firstMenuNode
	    loop
		exit when menuNode = nil or menuNode -> menu = afterMenu
		menuNode := menuNode -> next
	    end loop
	    if menuNode = nil then
		AssertFailed ("Menu ID not found")
	    end if
	    newMenuNode -> next := menuNode -> next
	    menuNode -> next := newMenuNode
	end if

	% Set the start/end positions
	menuNode := firstMenuNode
	offset := 0
	loop
	    exit when menuNode = nil
	    offset := menuNode -> menu -> SetWidth (offset)
	    menuNode := menuNode -> next
	end loop

	DrawBar
    end AddMenu


    procedure AppendMenu (menu : ^MenuClass)
	Initialize

	if firstMenuNode = nil then
	    AddMenu (menu, nil)
	else
	    var menuNode : ^MenuNode := firstMenuNode
	    loop
		exit when menuNode -> next = nil
		menuNode := menuNode -> next
	    end loop
	    AddMenu (menu, menuNode -> menu)
	end if
    end AppendMenu


    function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	var mx, my, b : int
	var menu : ^MenuClass

	if not initialized or not showing or mouseY < menuLine then
	    result false
	end if

	selectedMenu := FindMenu (mouseX)
	if selectedMenu = nil then
	    result true
	end if

	% Hilite the menu
	loop
	    if selectedMenu not= nil then
		exit when selectedMenu -> ConsiderButtonDown (0, 0)
	    end if
	    Mouse.Where (mx, my, b)
	    selectedMenu := FindMenu (mx)
	    exit when b = 0
	end loop

	result true
    end ConsiderButtonDown

    
    function ConsiderKeystroke (key : char) : boolean
	var menuNode : ^MenuNode := firstMenuNode

	if not initialized or not showing then
	    result false
	end if

	% Check each menu for the keystroke
	loop
	    exit when menuNode = nil
	    selectedMenu := menuNode -> menu
	    if menuNode -> menu -> ConsiderKeystroke (key) then
		result true
	    end if
	    menuNode := menuNode -> next
	end loop

	result false
    end ConsiderKeystroke


    function GetLastMenu : ^MenuClass
	if firstMenuNode = nil then
	    result nil
	else
	    var menuNode : ^MenuNode := firstMenuNode
	    loop
		exit when menuNode -> next = nil
		menuNode := menuNode -> next
	    end loop
	    result menuNode -> menu
	end if
    end GetLastMenu


    procedure Hide
	showing := false
	if initialized then
	    Draw.FillBox (0, menuLine, maxx, maxy,
		WidgetGlobals.backgroundColour)
	end if
    end Hide


    procedure Show
	showing := true
	DrawBar
    end Show
end MenuBarClass

