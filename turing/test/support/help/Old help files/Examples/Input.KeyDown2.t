% The "Input.KeyDown2" program.

% Note that depending on the keyboard's hardware, there may be a maximum
% of two keys + Shift + Control + Alt detectable at any one time.  This 
% should be remembered in multiplayer games where one player holding down
% two keys could prevent another player's keys from being detected.

% Note that many ASCII values are never generated.  Shift+A produces
% two characters: lower case a and Shift.

% The list of keys and their associated characters starting the characters
% with no visible representation.
var keys : array 0 .. * of
    record
	character : char
	name : string
    end record := init (
    init (chr (27), "Escape key"),
    init (chr (187), "F1 key"),
    init (chr (188), "F2 key"),
    init (chr (189), "F3 key"),
    init (chr (190), "F4 key"),
    init (chr (191), "F5 key"),
    init (chr (192), "F6 key"),
    init (chr (193), "F7 key"),
    init (chr (194), "F8 key"),
    init (chr (195), "F9 key"),
    init (chr (196), "F10 key"),
    init (chr (133), "F11 key"),
    init (chr (134), "F12 key"),
    init (chr (10), "Enter key"),
    init (chr (180), "Shift key"),
    init (chr (181), "Control key"),
    init (chr (182), "Alt key"),
    init (chr (8), "Backspace key"),
    init (chr (9), "Tab key"),
    init (chr (203), "Left Arrow"),
    init (chr (200), "Up Arrow"),
    init (chr (205), "Right Arrow"),
    init (chr (208), "Down Arrow"),
    init (chr (199), "Home"),
    init (chr (201), "Page Up"),
    init (chr (207), "End"),
    init (chr (209), "Page Down"),
    init (chr (210), "Insert"),
    init (chr (211), "Delete"),
    init (chr (183), "Keypad 5"),

    init ('`', "Left apostrophe key"),
    init ('1', "1 key"),
    init ('2', "2 key"),
    init ('3', "3 key"),
    init ('4', "4 key"),
    init ('5', "5 key"),
    init ('6', "6 key"),
    init ('7', "7 key"),
    init ('8', "8 key"),
    init ('9', "9 key"),
    init ('0', "0 key"),
    init ('-', "- key"),
    init ('=', "= key"),
    init ('a', "a key"),
    init ('b', "b key"),
    init ('c', "c key"),
    init ('d', "d key"),
    init ('e', "e key"),
    init ('f', "f key"),
    init ('g', "g key"),
    init ('h', "h key"),
    init ('i', "i key"),
    init ('j', "j key"),
    init ('k', "k key"),
    init ('l', "l key"),
    init ('m', "m key"),
    init ('n', "n key"),
    init ('o', "o key"),
    init ('p', "p key"),
    init ('q', "q key"),
    init ('r', "r key"),
    init ('s', "s key"),
    init ('t', "t key"),
    init ('u', "u key"),
    init ('v', "v key"),
    init ('w', "w key"),
    init ('x', "x key"),
    init ('y', "y key"),
    init ('z', "z key"),
    init ('[', "[ key"),
    init (']', "] key"),
    init ('\\', "\\ key"),
    init (';', "; key"),
    init ('\'', "' key"),
    init (',', ", key"),
    init ('.', ". key"),
    init ('/', "/ key"),
    init (' ', "Space key"),
    init ('*', "* key (Keypad)"),
    init ('+', "+ key (Keypad)")
    )
var a : array char of boolean
setscreen ("noecho")
loop
    Input.KeyDown (a)

    locate (1, 1)
    for key : 0 .. upper (keys)
	if a (keys (key).character) then
	    put ord (keys (key).character), ": ", keys (key).name, "  " ..
	end if
    end for
    put ""
end loop
