unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CanvasClass - Class that implements drawing canvases that are active
%               (i.e. respond to mouse presses)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of CanvasClass is as follows:
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of CanvasClass is as follows:
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
% The inheritable subprograms of CanvasClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of CanvasClass are as follows:
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of CanvasClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
%   function ConsiderButtonDown (x, y : int) : boolean
%   function ConsiderKeystroke (key : char) : boolean
%   procedure DrawWidget - Draw the widget.
%   procedure ComputeWidgetPosition - Compute the location of the label, etc.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class CanvasClass
    inherit GenericActiveWidgetClass

    import WidgetGlobals

    %
    % Exported routines for CanvasClass
    %
    export
    % The external interface
	Initialize, InitializeFull,
	DrawArc, DrawBox, DrawCls, DrawDot, DrawFill, DrawFillArc,
	DrawFillBox, DrawFillMapleLeaf, DrawFillOval, DrawFillPolygon,
	DrawFillStar, DrawLine, DrawMapleLeaf, DrawOval, DrawPolygon,
	DrawStar, DrawText, FontDraw, PicDraw, PicNew, PicScreenLoad,
	PicScreenSave, SetXOR


    %
    % The inheritable variables of CanvasClass
    %
    var border : int
    var canvasPicture : int := - 1
    var mouseDown : procedure x (mx, my : int)
    var mouseDrag : procedure x (mx, my : int)
    var mouseUp : procedure x (mx, my : int)
    var xorMode : boolean := false

    %
    % The inheritable subprograms of ButtonClass
    %
    procedure TakeCurrentPicture
	if canvasPicture not= - 1 then
	    Pic.Free (canvasPicture)
	end if
	canvasPicture := Pic.New (originalX, originalY,
	    originalX + originalWidth - 1,
	    originalY + originalHeight - 1)
    end TakeCurrentPicture


    procedure BeforeDraw
	View.ClipSet (originalX, originalY, originalX + originalWidth - 1,
	    originalY + originalHeight - 1)
	if xorMode then
	    View.Set ("xor")
	end if
    end BeforeDraw


    procedure AfterDraw
	if xorMode then
	    View.Set ("noxor")
	end if
	View.ClipOff
	TakeCurrentPicture
    end AfterDraw


    %
    % The external subprograms of CanvasClass
    %
    procedure InitializeFull (newX, newY, newWidth, newHeight : int,
	    newBorder : int, newMouseDown : procedure x (mx, my : int),
	    newMouseDrag : procedure x (mx, my : int),
	    newMouseUp : procedure x (mx, my : int))
	SetPositionAndSize (newX, newY, newWidth, newHeight)
	mouseDown := newMouseDown
	mouseDrag := newMouseDrag
	mouseUp := newMouseUp
	border := TranslateBorder (newBorder)

	GeneralInitialize
    end InitializeFull


    procedure Initialize (newX, newY, newWidth, newHeight : int)
	InitializeFull (newX, newY, newWidth, newHeight, WidgetGlobals.LINE,
	    WidgetGlobals.DefaultMouseRoutine,
	    WidgetGlobals.DefaultMouseRoutine,
	    WidgetGlobals.DefaultMouseRoutine)
    end Initialize


    procedure DrawArc (x, y, xRadius, yRadius, initAngle, finalAngle : int,
	    clr : int)
	BeforeDraw
	Draw.Arc (x + originalX, y + originalY, xRadius, yRadius,
	    initAngle, finalAngle, clr)
	AfterDraw
    end DrawArc


    procedure DrawBox (x1, y1, x2, y2, clr : int)
	BeforeDraw
	Draw.Box (x1 + originalX, y1 + originalY, x2 + originalX,
	    y2 + originalY, clr)
	AfterDraw
    end DrawBox


    procedure DrawCls
	Draw.FillBox (originalX, originalY,
	    originalX + originalWidth - 1,
	    originalY + originalHeight - 1, 0)
	TakeCurrentPicture
    end DrawCls


    procedure DrawDot (x, y, clr : int)
	BeforeDraw
	Draw.Dot (x + originalX, y + originalY, clr)
	AfterDraw
    end DrawDot


    procedure DrawFill (x, y, fillColour, borderColour : int)
	View.ClipSet (originalX, originalY,
	    originalX + originalWidth - 1,
	    originalY + originalHeight - 1)
	Draw.Fill (x + originalX, y + originalY, fillColour,
	    borderColour)
	View.ClipOff
	TakeCurrentPicture
    end DrawFill

    procedure DrawFillArc (x, y, xRadius, yRadius, initAngle : int,
	    finalAngle, clr : int)
	BeforeDraw
	Draw.FillArc (x + originalX, y + originalY, xRadius, yRadius,
	    initAngle, finalAngle, clr)
	AfterDraw
    end DrawFillArc


    procedure DrawFillBox (x1, y1, x2, y2, clr : int)
	BeforeDraw
	Draw.FillBox (x1 + originalX, y1 + originalY, x2 + originalX,
	    y2 + originalY, clr)
	AfterDraw
    end DrawFillBox


    procedure DrawFillMapleLeaf (x1, y1, x2, y2, clr : int)
	BeforeDraw
	Draw.FillMapleLeaf (x1 + originalX, y1 + originalY,
	    x2 + originalX, y2 + originalY, clr)
	AfterDraw
    end DrawFillMapleLeaf


    procedure DrawFillOval (x, y, xRadius, yRadius, clr : int)
	BeforeDraw
	Draw.FillOval (x + originalX, y + originalY, xRadius, yRadius,
	    clr)
	AfterDraw
    end DrawFillOval


    procedure DrawFillPolygon (x, y : array 1 .. * of int, n, clr : int)
	var nx, ny : array 1 .. n of int
	for count : 1 .. n
	    nx (count) := x (count) + originalX
	    ny (count) := y (count) + originalY
	end for
	BeforeDraw
	Draw.FillPolygon (x, y, n, clr)
	AfterDraw
    end DrawFillPolygon


    procedure DrawFillStar (x1, y1, x2, y2, clr : int)
	BeforeDraw
	Draw.FillStar (x1 + originalX, y1 + originalY, x2 + originalX,
	    y2 + originalY, clr)
	AfterDraw
    end DrawFillStar



    procedure DrawLine (x1, y1, x2, y2, clr : int)
	BeforeDraw
	Draw.Line (x1 + originalX, y1 + originalY, x2 + originalX,
	    y2 + originalY, clr)
	AfterDraw
    end DrawLine


    procedure DrawMapleLeaf (x1, y1, x2, y2, clr : int)
	BeforeDraw
	Draw.MapleLeaf (x1 + originalX, y1 + originalY, x2 + originalX,
	    y2 + originalY, clr)
	AfterDraw
    end DrawMapleLeaf


    procedure DrawOval (x, y, xRadius, yRadius, clr : int)
	BeforeDraw
	Draw.Oval (x + originalX, y + originalY, xRadius, yRadius, clr)
	AfterDraw
    end DrawOval


    procedure DrawPolygon (x, y : array 1 .. * of int, n, clr : int)
	var nx, ny : array 1 .. n of int
	for count : 1 .. n
	    nx (count) := x (count) + originalX
	    ny (count) := y (count) + originalY
	end for
	BeforeDraw
	Draw.Polygon (x, y, n, clr)
	AfterDraw
    end DrawPolygon


    procedure DrawStar (x1, y1, x2, y2, clr : int)
	BeforeDraw
	Draw.Star (x1 + originalX, y1 + originalY, x2 + originalX,
	    y2 + originalY, clr)
	AfterDraw
    end DrawStar


    procedure DrawText (text : string, x, y, fontID, clr : int)
	BeforeDraw
	Draw.Text (text, x + originalX, y + originalY, fontID, clr)
	AfterDraw
    end DrawText


    procedure FontDraw (text : string, x, y, fontID, clr : int)
	BeforeDraw
	Font.Draw (text, x + originalX, y + originalY, fontID, clr)
	AfterDraw
    end FontDraw


    procedure PicDraw (picID, x, y, mode : int)
	View.ClipSet (originalX, originalY,
	    originalX + originalWidth - 1,
	    originalY + originalHeight - 1)
	Pic.Draw (picID, x + originalX, y + originalY, mode)
	View.ClipOff
	TakeCurrentPicture
    end PicDraw


    function PicNew (x1, y1, x2, y2 : int) : int
	result Pic.New (x1 + originalX, y1 + originalY,
	    x2 + originalX, y2 + originalY)
    end PicNew


    procedure PicScreenLoad (fileName : string, x, y, mode : int)
	View.ClipSet (originalX, originalY,
	    originalX + originalWidth - 1,
	    originalY + originalHeight - 1)
	Pic.ScreenLoad (fileName, x + originalX, y + originalY, mode)
	View.ClipOff
	TakeCurrentPicture
    end PicScreenLoad


    procedure PicScreenSave (x1, y1, x2, y2 : int, fileName : string)
	Pic.ScreenSave (x1 + originalX, y1 + originalY,
	    x2 + originalX, y2 + originalY, fileName)
    end PicScreenSave


    procedure SetXOR (newXor : boolean)
	xorMode := newXor
    end SetXOR


    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	if border = WidgetGlobals.LINE then
	    x := originalX - 1
	    y := originalY - 1
	    width := originalWidth + 2
	    height := originalHeight + 2
	elsif border = WidgetGlobals.INDENT then
	    x := originalX - riserSize
	    y := originalY - riserSize - 1
	    width := originalWidth + 2 * riserSize + 1
	    height := originalHeight + 2 * riserSize + 1
	else
	    x := originalX - riserSize - 1
	    y := originalY - riserSize
	    width := originalWidth + 2 * riserSize + 1
	    height := originalHeight + 2 * riserSize + 1
	end if
    end ComputeWidgetPosition


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if

	% Draw the border.
	if border = WidgetGlobals.LINE then
	    Draw.Box (x, y, x + width - 1, y + height - 1, foregroundColour)
	else
	    var topLeftColour, bottomRightColour : int
	    if border = WidgetGlobals.INDENT then
		topLeftColour := darkgrey
		bottomRightColour := illuminatedColor
	    else
		topLeftColour := illuminatedColor
		bottomRightColour := darkgrey
	    end if
	    var x2 : int := x + width - 1
	    var y2 : int := y + height - 1
	    for offset : 0 .. riserSize - 1
		Draw.Line (x + offset, y + offset, x2 - offset,
		    y + offset, bottomRightColour)
		Draw.Line (x2 - offset, y + offset, x2 - offset,
		    y2 - offset, bottomRightColour)
		Draw.Line (x + offset, y + offset, x + offset,
		    y2 - offset, topLeftColour)
		Draw.Line (x + offset, y2 - offset, x2 - offset,
		    y2 - offset, topLeftColour)
	    end for
	    if border = WidgetGlobals.INDENT then
		Draw.Line (x + riserSize, y + riserSize, x2 - riserSize,
		    y + riserSize, grey)
		Draw.Line (x2 - riserSize, y + riserSize, x2 - riserSize,
		    y2 - riserSize, grey)
	    elsif border = WidgetGlobals.EXDENT then
		Draw.Line (x + riserSize, y + riserSize, x + riserSize,
		    y2 - riserSize, grey)
		Draw.Line (x + riserSize, y2 - riserSize, x2 - riserSize,
		    y2 - riserSize, grey)
	    end if
	end if

	% Draw the canvas.
	if canvasPicture not= - 1 then
	    Pic.Draw (canvasPicture, originalX, originalY, picCopy)
	else
	    if colourbg = black and backgroundColour not= colourbg then
		Draw.FillBox (originalX, originalY,
		    originalX + originalWidth - 1,
		    originalY + originalHeight - 1, brightwhite)
	    else
		Draw.FillBox (originalX, originalY,
		    originalX + originalWidth - 1,
		    originalY + originalHeight - 1, colourbg)
	    end if
	end if
    end DrawWidget


    body procedure EraseWidget
	if not initialized then
	    AssertFailed ("The object has not been initialized yet")
	else
	    if not drawn then
		ComputeWidgetPosition
	    end if
	    TakeCurrentPicture
	    Draw.FillBox (x, y, x + width - 1, y + height - 1,
		backgroundColour)
	end if
    end EraseWidget


    body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	assert initialized
	if not enabled or not showing then
	    result false
	end if
	if originalX <= mouseX and
		mouseX < originalX + originalWidth and
		originalY <= mouseY and
		mouseY < originalY + originalHeight then

	    mouseDown (mouseX - originalX, mouseY - originalY)

	    var ox, oy, mx, my, b : int
	    ox := mouseX
	    oy := mouseY
	    loop
		Mouse.Where (mx, my, b)
		if (ox not= mx) or (oy not= my) then
		    mouseDrag (mx - originalX, my - originalY)
		    ox := mx
		    oy := my
		end if
		exit when b = 0
	    end loop
	    mouseUp (mx - originalX, my - originalY)
	    result true
	else
	    result false
	end if
    end ConsiderButtonDown


    body function ConsiderKeystroke (key : char) : boolean
	result false
    end ConsiderKeystroke
end CanvasClass

