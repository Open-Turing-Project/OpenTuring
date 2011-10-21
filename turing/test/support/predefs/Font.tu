/*
 * Font module
 *
 * NB:  DO NOT INCLUDE OR IMPORT THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Font
    export Draw, Free, GetName, GetSize, GetStyle, Name, New, Sizes,
	StartName, StartSize, Width,
	~.*defFontID, ~.*defFontId

    external "font_new" function New (fontSelectStr : string) : int

    external "font_free" procedure Free (fontId : int)

    external "draw_text"
	procedure Draw (str : string, x, y : int, fontId, colourNum : int)

    external "font_width" function Width (str : string, fontId : int) : int

    external "font_sizes"
	procedure Sizes (fontId : int, var height, ascent, descent : int,
	var internalLeading : int)

    external "font_name" function Name (fontId : int) : string

    %
    % Font enumeration routines
    %
    external "font_startname" procedure StartName
    external "font_getname" function GetName : string
    external "font_getstyle"
	procedure GetStyle (name : string, var bold, italic, underline : boolean)
    external "font_startsize" procedure StartSize (name, styles : string)
    external "font_getsize" function GetSize : int

    % Constants
    const *defFontID := -10
    const *defFontId := defFontID
end Font
