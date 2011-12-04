/*
 * View module - routines that concern display area
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive View
    export ClipSet, ClipAdd, ClipAddOval, ClipOff, WhatDotColor,
	WhatDotColour, Set, Scroll, SetTransparentColour,
	SetTransparentColor, Update, UpdateArea,
	~.*maxx, ~.*maxy

    % This procedure causes a run-time error indicating that the feature
    % is obsolete.
    external procedure obsolete (routine : string)

    external "view_clipset" procedure ClipSet (x1, y1, x2, y2 : int)

    external "view_clipadd" procedure ClipAdd (x1, y1, x2, y2 : int)

    external "view_clipaddoval" procedure ClipAddOval (x, y, xr, yr : int)

    external "view_clipoff" procedure ClipOff

    external "view_whatdotcolour" function WhatDotColor (x, y : int) : int
    external "view_whatdotcolour" function WhatDotColour (x, y : int) : int

    external "view_set" procedure Set (setStr : string)

    external "view_maxx" function maxx : int
    external "view_maxy" function maxy : int

    external "view_settransclr" procedure SetTransparentColour (clrNum : int)

    external "view_settransclr" procedure SetTransparentColor (clrNum : int)

    external "view_update" procedure Update

    external "view_updatearea" procedure UpdateArea (x1, y1, x2, y2 : int)

    function GetActivePage : int
	obsolete ("View.GetActivePage")
	result 0
    end GetActivePage

    function GetObsoletePage : int
	obsolete ("View.GetObsoletePage")
	result 0
    end GetObsoletePage

    procedure SetActivePage (page : int)
	obsolete ("View.SetActivePage")
    end SetActivePage

    procedure SetVisiblePage (page : int)
	obsolete ("View.SetVisiblePage")
    end SetVisiblePage

    /* EXPERIMENTALS */

    external "view_scroll" procedure Scroll (dx, dy : int, clear : boolean)

end View

