/*
 * Text module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Text
    export Cls, Color, Colour, ColorBack, ColourBack, Locate, LocateXY,
	WhatRow, WhatCol, WhatColor, WhatColour,
	WhatColorBack, WhatColourBack, WhatChar, ~.*maxcol, ~.*maxrow

    external procedure obsolete (routine : string)

    external "text_cls" procedure Cls

    external "text_colour" procedure Color (clrNum : int)
    external "text_colour" procedure Colour (clrNum : int)

    external "text_colourback" procedure ColorBack (clrNum : int)
    external "text_colourback" procedure ColourBack (clrNum : int)

    external "text_locate" procedure Locate (row, col : int)

    external "text_locatexy" procedure LocateXY (x, y : int)

    external "text_maxcol" function maxcol : int
    external "text_maxrow" function maxrow : int

    external "text_whatcol" function WhatCol : int
    external "text_whatrow" function WhatRow : int

    external "text_whatcolour" function WhatColor : int
    external "text_whatcolour" function WhatColour : int

    external "text_whatcolourback" function WhatColorBack : int
    external "text_whatcolourback" function WhatColourBack : int

    procedure WhatChar (row, col : int, var ch : char,
	    var foreColor, backColor : int)
	obsolete ("Text.WhatChar")
    end WhatChar
end Text
