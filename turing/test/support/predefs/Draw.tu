/*
 * Draw module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED
 */

unit
module pervasive Draw
    export Arc, Box, Cls, DashedLine, Dot, Fill, FillArc, FillBox,
	FillMapleLeaf, FillOval, FillPolygon, FillStar, Line, MapleLeaf,
	Oval, Polygon, Star, Text, ThickLine,
	~.*drawSolid, ~.*drawDash, ~.*drawDot, ~.*drawDashDot,
	~.*drawDashDotDot

    %
    % Draw an arc on the screen.
    %
    external "draw_arc"
	procedure Arc (x, y, xRadius, yRadius : int,
	initialAngle, finalAngle : int, colourNum : int)

    %
    % Draw a box with the given bottom-left and top-right corners
    %
    external "draw_box" procedure Box (x1, y1, x2, y2 : int, colourNum : int)

    %
    % Clear the screen with colourbg
    %
    external "draw_cls" procedure Cls

    %
    % Draw a dot (pixel) at the given location.
    %
    external "draw_dot" procedure Dot (x, y : int, colourNum : int)

    %
    % Colour in a figure on the screen.
    %
    external "draw_fill"
	procedure Fill (x, y : int, fillColourNum, borderColourNum : int)

    %
    % Draw a filled arc on the screen.
    %
    external "draw_fillarc"
	procedure FillArc (x, y, xRadius, yRadius : int,
	initialAngle, finalAngle : int,
	colourNum : int)

    %
    % Draw a filled box with the given bottom-left and top-right corners
    % on the screen.
    %
    external "draw_fillbox"
	procedure FillBox (x1, y1, x2, y2 : int, colourNum : int)

    %
    % Draw a filled maple leaf bounded by the passed rectangle.
    %
    external "draw_fillmapleleaf"
	procedure FillMapleLeaf (x1, y1, x2, y2 : int, colourNum : int)

    %
    % Draw a filled oval on the screen.
    %
    external "draw_filloval"
	procedure FillOval (x, y, xRadius, yRadius : int, colourNum : int)

    %
    % Draw the specified filled polygon on the screen.
    %
    external "draw_fillpolygon"
	procedure FillPolygon (x, y : array 1 .. * of int, n : int,
	colourNum : int)

    %
    % Draw a filled star bounded by the passed rectangle.
    %
    external "draw_fillstar"
	procedure FillStar (x1, y1, x2, y2 : int, colourNum : int)

    %
    % Draw a line on the screen.
    %
    external "draw_line"
	procedure Line (x1, y1, x2, y2 : int, colourNum : int)

    %
    % Draw a (dashed/dotted) line on the screen.
    %
    external "draw_line_dashed"
	procedure DashedLine (x1, y1, x2, y2 : int, style, colourNum : int)

    const *drawSolid : int := 0
    const *drawDash : int := 1
    const *drawDot : int := 2
    const *drawDashDot : int := 3
    const *drawDashDotDot : int := 4

    %
    % Draw a thick line on the screen.
    %
    external "draw_line_thick"
	procedure ThickLine (x1, y1, x2, y2 : int, width, colourNum : int)

    %
    % Draw a maple leaf bounded by the passed rectangle.
    %
    external "draw_mapleleaf"
	procedure MapleLeaf (x1, y1, x2, y2 : int, colourNum : int)

    %
    % Draw an oval on the screen.
    %
    external "draw_oval"
	procedure Oval (x, y, xRadius, yRadius : int, colourNum : int)


    %
    % Draw the specified polygon on the screen.
    %
    external "draw_polygon"
	procedure Polygon (x, y : array 1 .. * of int, n : int,
	colourNum : int)

    %
    % Draw a star bounded by the passed rectangle.
    %
    external "draw_star"
	procedure Star (x1, y1, x2, y2 : int, colourNum : int)


    %
    % Same as Font.Draw - Here to handle different mental models
    %

    external "draw_text"
	procedure Text (str : string, x, y : int, fontId, colourNum : int)
end Draw
