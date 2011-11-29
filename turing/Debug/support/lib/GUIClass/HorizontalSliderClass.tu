unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% VerticalSliderClass - Class that that implements vertical sliders.  The
%                       original SliderWidget was the work of teacher Greg
%                       Clarke.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of VerticalSliderClass is as follows:
%   procedure Show - Display the widget.
%   procedure Hide - Hide the widget.
%   procedure Dispose - Hide the widget and free any data structures allocated
%                       by the class.  To be called before the Widget is freed.
%   procedure SetPosition (x, y : int) - Move the widget to (x, y).
%   procedure SetSize (width, height : int) - Resize the widget.
%   procedure SetPositionAndSize (x, y, width, height : int) - Move and resize
%                                                              the widget.
%   procedure Enable - Allow slider to be slid by user.
%   procedure Disable - Stop user from changing slider value.
%   procedure Initialize (x, y, length, min, max, startValue : int,
%                         newActionProc : proc x (sliderValue : int))
%                       x, y - The location of the start of the slide
%                       length - The length ofthe slide
%                       min, max - The minimum and maximum value of the slider.
%                       startValue - The initial value of the thumb.
%   function GetSliderValue : int - Return the current value of the slider.
%   procedure SetSliderValue (value : int) - Set the value of the slider.
%   procedure SetSliderMinMax (min, max : int) - Set the sliders min/max
%                                                values.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of VerticalSliderClass is as follows:
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
% The inheritable subprograms of VerticalSliderClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%   procedure DrawWidget - Draw the widget.
%   procedure ComputeWidgetPosition - Compute the location of widget, etc.
%   procedure DrawThumb - Draw the thumb
%   procedure CalculateThumbPosition - Calculate the thumb position from the
%       current scroll value.
%   function InThumb (mx, my : int) : boolean - returns whether the
%       point (mx, my) is located in the thumb of the slider
%   function CloseToSlider (mx, my : int) : boolean - returns whether the
%       point (x, y) is located near enough to the slider
%   function GetSliderValueFromMouse (mx, my : int) : int - returns the slider
%       value corresponding to the change in mouse position
%   body procedure SetOriginalMouseDownForThumb (mx, my : int) - called when
%       the user first puts the mouse down in the thumb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable variables of VerticalSliderClass are as follows:
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
%   actionProc - The current user feedback procedure.
%   sliderValue - The current value of the slider.
%   minValue - The minimum value of the slider.
%   maxValue - The maximum value of the slider.
%   sliderLength - The length of the slider.
%   thumbPicture - Actually picture of area behind the thumb.
%   thumbX, thumbY - The location of lower left corner of thumb
%   thumbMovementMouseDX - the displacement of the mouse down from the lower
%                          left corner
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of VerticalSliderClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   slideWidth - The width of the actual slide.
%   markLength - The length of the marks on the sides of the slide.
%   thumbWidth - The size of the thumb transverse to direction of sliding.
%   thumbHeight - The size of the thumb in the direction of sliding.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class HorizontalSliderClass
    inherit GenericSliderClass

    %
    % The inheritable variables of SliderClass
    %
    var thumbMovementMouseDX : int := 0

    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	x := originalX - max (slideWidth, thumbHeight) div 2
	y := originalY - markLength - slideWidth div 2
	sliderLength := originalWidth
	width := originalWidth + max (thumbWidth, thumbHeight)
	height := 2 * markLength + slideWidth + 1
	drawn := true
    end ComputeWidgetPosition


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if

	CalculateThumbPosition

	const radius := slideWidth div 2

	% Draw left edge of slider
	Draw.FillArc (originalX, originalY, radius, radius, 90, 270,
	    trackColour)
	Draw.Arc (originalX, originalY, radius, radius, 90, 270,
	    foregroundColour)

	% Draw body of slider
	Draw.FillBox (originalX, originalY - radius,
	    originalX + sliderLength, originalY + radius, trackColour)
	Draw.Line (originalX, originalY - radius, originalX + sliderLength,
	    originalY - radius, foregroundColour)
	Draw.Line (originalX, originalY + radius, originalX + sliderLength,
	    originalY + radius, foregroundColour)

	% Draw the right edge of the slider
	Draw.FillArc (originalX + sliderLength, originalY, radius, radius,
	    270, 90, trackColour)
	Draw.Arc (originalX + sliderLength, originalY, radius, radius,
	    270, 90, foregroundColour)

	% Draw the marks
	for mark : 0 .. 4
	    Draw.Line (originalX + mark * sliderLength div 4,
		originalY - radius, originalX + mark * sliderLength div 4,
		originalY - radius - markLength, markColour)
	    Draw.Line (originalX + mark * sliderLength div 4,
		originalY + radius, originalX + mark * sliderLength div 4,
		originalY + radius + markLength, markColour)
	end for

	DrawThumb
    end DrawWidget


    body procedure DrawThumb
	var whiteColour : int
	if colourbg = black then
	    whiteColour := brightwhite
	else
	    whiteColour := white
	end if

	thumbX := originalX + thumbPosition - 8
	thumbY := originalY - 10

	var x1 : int := thumbX
	var y1 : int := thumbY
	var x2 : int := thumbX + thumbHeight
	var y2 : int := thumbY + thumbWidth

	if thumbPicture not= - 1 then
	    % About the only time execution gets here is when there's
	    % a Refresh
	    Pic.Free (thumbPicture)
	end if
	thumbPicture := Pic.New (x1, y1, x2, y2)

	Draw.Box (x1, y1, x2, y2, black)
	if colourbg = black then
	    Draw.FillBox (x1 + 2, y1 + 2, x2 - 2, y2 - 2, white)
	else
	    Draw.FillBox (x1 + 2, y1 + 2, x2 - 2, y2 - 2, grey)
	end if


	% Border
	Draw.Line (x1 + 1, y1 + 1, x1 + 1, y2 - 1, whiteColour)
	Draw.Line (x1 + 1, y2 - 1, x2 - 1, y2 - 1, whiteColour)
	Draw.Line (x1 + 2, y1 + 1, x2 - 1, y1 + 1, darkgrey)
	Draw.Line (x2 - 1, y1 + 1, x2 - 1, y2 - 2, darkgrey)

	% Draw the ridges
	const midx : int := x1 + (x2 - x1) div 2
	Draw.Line (midx - 4, y1 + 4, midx - 4, y2 - 4, whiteColour)
	Draw.Line (midx - 3, y1 + 4, midx - 3, y2 - 4, darkgrey)
	Draw.Line (midx, y1 + 4, midx, y2 - 4, whiteColour)
	Draw.Line (midx + 1, y1 + 4, midx + 1, y2 - 4, darkgrey)
	Draw.Line (midx + 4, y1 + 4, midx + 4, y2 - 4, whiteColour)
	Draw.Line (midx + 5, y1 + 4, midx + 5, y2 - 4, darkgrey)
    end DrawThumb


    body function InThumb (mx, my : int) : boolean
	result thumbX <= mx and mx <= thumbX + thumbHeight and
	    thumbY <= my and my <= thumbY + thumbWidth
    end InThumb


    body function CloseToSlider (mx, my : int) : boolean
	result originalY - markLength - slideWidth <= my and
	    my <= originalY + markLength + slideWidth
    end CloseToSlider


    body function GetSliderValueFromMouse (mx, my : int) : int
	const newThumbPosition : int := mx - thumbMovementMouseDX
	var temp := minValue +
	    round ( (newThumbPosition - originalX) /
	    sliderLength * (maxValue - minValue))
	result min (maxValue, max (minValue, temp))
    end GetSliderValueFromMouse


    body procedure SetOriginalMouseDownForThumb (mx, my : int)
	thumbMovementMouseDX := mx - (originalX + thumbPosition)
    end SetOriginalMouseDownForThumb
end HorizontalSliderClass

