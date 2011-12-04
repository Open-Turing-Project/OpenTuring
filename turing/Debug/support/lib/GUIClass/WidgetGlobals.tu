unit
module WidgetGlobals
    import GenericWidgetClass

    export LEFT, RIGHT, CENTRE, TOP, MIDDLE, BOTTOM,
	HORIZONTAL_ALIGNMENTS, VERTICAL_ALIGNMENTS, ALL_ALIGNMENTS,
	LINE, INDENT, EXDENT, ANY, INTEGER, REAL,
	SCROLL_BAR_WIDTH,
	systemFontID, var showWhenInitialized,
	var firstWidget, var lastWidget, var selectedWidget,
	menuFontID, menuFontDescent, 
	X_OFFSET, MENU_BAR_HEIGHT, ITEM_HEIGHT,
	MENU_COLOUR, MENU_TEXT_COLOUR, MENU_DISABLED_TEXT_COLOUR,
	MENU_HILITE_TEXT_COLOUR,
	var ~.backgroundColour, charNames,
	DefaultMouseRoutine, AddWindowToArray, RemoveWindowFromArray,
	SetDisplayWhenInitialized,
	numWindows, var window, var blinkingTextField

    const LEFT := 1
    const CENTRE := 2
    const RIGHT := 4
    const HORIZONTAL_ALIGNMENTS := LEFT + CENTRE + RIGHT

    const TOP := 8
    const MIDDLE := 16
    const BOTTOM := 32
    const VERTICAL_ALIGNMENTS := TOP + MIDDLE + BOTTOM

    const ALL_ALIGNMENTS := VERTICAL_ALIGNMENTS + HORIZONTAL_ALIGNMENTS

    const LINE := 1
    const INDENT := 2
    const EXDENT := 3

    const ANY := 1
    const INTEGER := 2
    const REAL := 3

    const SCROLL_BAR_WIDTH : int := 16

    % Alternative : "system:12"
    #if _DOS_ then
	const systemFontID : int := Font.New ("System:16")
    #else
	const systemFontID : int := Font.New ("serif:9:bold")
    #end if

    var showWhenInitialized : boolean := true

    var backgroundColour : int := colourbg

    var firstWidget, lastWidget : ^GenericWidgetClass := nil
    var selectedWidget : ^GenericWidgetClass := nil
    var blinkingTextField : ^GenericWidgetClass := nil

    % The array of open windows and the number of open windows.
    var window : array 1 .. 250 of
	record
	    windowID : int
	    menuBar : ^anyclass
	end record
    var numWindows : int := 0

    % Menu constants
    #if _MACOOT_ then
	const menuFontID : int := Font.New ("chicago:12")
    #elsif _DOS_ then
	const menuFontID : int := Font.New ("System:16")
    #else
	const menuFontID : int := Font.New ("system:9:bold")
    #end if
    var d1, d2, d3 : int
    var menuFontDescent : int
    Font.Sizes (menuFontID, d1, d2, menuFontDescent, d3)
    const X_OFFSET : int := 7
    const MENU_BAR_HEIGHT : int := 19
    const ITEM_HEIGHT : int := 18
    #if _DOS_ then
	const MENU_COLOUR : int := brightwhite
	const MENU_DISABLED_TEXT_COLOUR : int := white
    #else
	const MENU_COLOUR : int := white
	const MENU_DISABLED_TEXT_COLOUR : int := gray
    #end if
    const MENU_TEXT_COLOUR : int := black
    const MENU_HILITE_TEXT_COLOUR : int := white

    const charNames : array char of string (16) := init (
	"", "Ctrl+A", "Ctrl+B", "", "Ctrl+D", "Ctrl+E", "Ctrl+F", % 0-6
	"Ctrl+G", "Ctrl+H", "Ctrl+I", "Ctrl+J", "Ctrl+K", "Ctrl+L", % 7-12
	"Ctrl+M", "Ctrl+N", "Ctrl+O", "Ctrl+P", "Ctrl+Q", "Ctrl+R", % 13-18
	"Ctrl+S", "Ctrl+T", "Ctrl+U", "Ctrl+V", "Ctrl+W", "Ctrl+X", % 19-24
	"Ctrl+Y", "Ctrl+Z", "ESC", "Ctrl+\\", "Ctrl+]", "Ctrl+\^", % 25-30
	"Ctrl+_", "(space)", "!", "\"", "#", "$", "%", "&", "'", % 31-39
	"(", ")", "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", % 40-51
	"4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", % 52-62
	"?", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", % 63-74
	"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", % 75-86
	"W", "X", "Y", "Z", "[", "\\", "]", "\^", "_", "`", "a", % 87-97
	"b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", % 98-109
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", % 110-121
	"z", "{", "|", "}", "~", "Ctrl+BS", "Alt+9", "Alt+0", % 122-129
	"Alt+-", "Alt+=", "Ctrl+PgUp", "F11", "F12", "Shift+F11", % 130-136
	"Shift+F12", % 137
	"Ctrl+F11", "Ctrl+F12", "Alt+F11", "Alt+F12", % 137-140
	"Ctrl+Up Arrow", "", "Shift+TAB", "Alt+Q", "Ctrl+Dn Arrow", % 141-145
	"Ctrl+Insert", "Ctrl+Del", "Alt+T", "Alt+Y", "Alt+U", % 146-150
	"Alt+I", "Alt+O", "Alt+P", "", "", "", "", "Alt+A", % 151-158
	"Alt+S", "Alt+D", "Alt+F", "Alt+G", "Alt+H", "Alt+J", % 159-164
	"Alt+K", "Alt+L", "", "", "", "", "", "Alt+Z", "Alt+X", % 165-173
	"Alt+C", "Alt+V", "Alt+B", "Alt+N", "Alt+M", "", "", "", % 174-181
	"", "", "", "", "", "F1", "F2", "F3", "F4", "F5", "F6", % 182-192
	"F7", "F8", "F9", "F10", "", "", "Home", "Up Arrow", % 193-200
	"PgUp", "", "Left Arrow", "", "Right Arrow", "", "End", % 201-207
	"Down Arrow", "PgDn", "Insert", "Del", "Shift+F1", % 208-212
	"Shift+F2", "Shift+F3", "Shift+F4", "Shift+F5", "Shift+F6", % 213-217
	"Shift+F7", "Shift+F8", "Shift+F9", "Shift+F10", "Ctrl+F1", % 218-222
	"Ctrl+F2", "Ctrl+F3", "Ctrl+F4", "Ctrl+F5", "Ctrl+F6", % 223-227
	"Ctrl+F7", "Ctrl+F8", "Ctrl+F9", "Ctrl+F10", "Alt+F1", % 228-232
	"Alt+F2", "Alt+F3", "Alt+F4", "Alt+F5", "Alt+F6", % 233-237
	"Alt+F7", "Alt+F8", "Alt+F9", "Alt+F10", "", % 238-242
	"Ctrl+Left Arrow", "Ctrl+Right Arrow", "Ctrl+End", % 243-245
	"Ctrl+PgDn", "Ctrl+Home", "Alt+1", "Alt+2", "Alt+3", % 246-250
	"Alt+4", "Alt+5", "Alt+6", "Alt+7", "Alt+8") % 251-255

    procedure SetDisplayWhenInitialized (display : boolean)
	showWhenInitialized := display
    end SetDisplayWhenInitialized


    procedure DefaultMouseRoutine (mx, my : int)
    end DefaultMouseRoutine


    procedure AddWindowToArray (win : int)
	for i : 1 .. numWindows
	    if win = window (i).windowID then
		return
	    end if
	end for
	#if _WINOOT_ then
	    % Really should be Window.Set (win...
	    Window.Set (win, "nocursor,noecho,rawinput")
	#else
	    % Really should be Window.Set (win...
	    Window.Set (win, "nocursor,noecho")
	#end if
	numWindows += 1
	window (numWindows).windowID := win
	window (numWindows).menuBar := nil
    end AddWindowToArray


    procedure RemoveWindowFromArray (win : int)
	for i : 1 .. numWindows
	    if win = window (i).windowID then
		for j : i + 1 .. numWindows
		    window (j - 1) := window (j)
		end for
		numWindows -= 1
		return
	    end if
	end for
    end RemoveWindowFromArray
end WidgetGlobals

