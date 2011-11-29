unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% VerticalScrollBarClass - Class that that implements scroll bars much as
%                          they appear in MS Windows 3.1.  The original
%                          SliderWidget was the work of teacher Greg Clarke.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of VerticalScrollBarClass is as follows:
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
%   procedure InitializeFull (newX, newY, newLength, newMin, newMax : int,
%           startValue : int, newActionProc : proc x (sliderValue : int),
%           arrowIncrement, pageIncrement, thumbSize : int)
%                       x, y - The location of the start of the slide
%                       length - The length ofthe slide
%                       min, max - The minimum and maximum value of the slider.
%                       startValue - The initial value of the thumb.
%                       arrowIncrement - The change in the scroll bar value
%                                        when an arrow is pressed.
%                       pageIncrement - The change in the scroll bar value
%                                       when clikcing in above/below the thumb.
%                       thumbSize - The size of the thumb (w.r.t min/max).
%   function GetSliderValue : int - Return the current value of the slider.
%   procedure SetSliderValue (value : int) - Set the value of the slider.
%   procedure SetSliderMinMax (min, max : int) - Set the sliders min/max
%                                                values.
%   procedure SetScrollAmount (arrowIncrement, pageIncrement,
%                              thumbSize : int) - Set the increments for
%                                                 pressing the arrow, page up/
%                                                 down and the thumb size.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The package interface of VerticalScrollBarClass is as follows:
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
% The inheritable subprograms of VerticalScrollBarClass are as follows:
%   procedure AssertFailed (msg : string) - Quit program with error message.
%   procedure DrawPressedBorder - Draws a pressed raised button border.
%   procedure DrawUnpressedBorder - Draws a raised button border.
%   procedure DrawWidget - Draw the widget.
%   procedure EraseWidget - Erase the widget.
%   procedure DrawWidget - Draw the widget.
%   procedure ComputeWidgetPosition - Compute the location of widget, etc.
%   procedure DrawScrollThumb - Draw the thumb
%   function InThumb (mx, my : int) : boolean - returns whether the
%       point (mx, my) is located in the thumb of the slider
%   function InBounds (x, y : int) : boolean - returns whether the
%       point (x, y) is located in the slider
%   function InBoundsMoreButton (mx, my : int) : boolean - returns whether the
%       point (x, y) is located in the down/right arrow of the slider
%   function InBoundsLessButton (mx, my : int) : boolean - returns whether the
%       point (x, y) is located in the up/left arrow of the slider
%   function InBoundsPageUp (mx, my : int) : boolean - returns whether the
%       point (x, y) is located in the page up/left section of the slider
%   function InBoundsPageDown (mx, my : int) : boolean - returns whether the
%       point (x, y) is located in the page down/right section of the slider
%   function InBoundsOnThumb (mx, my : int) : boolean - returns whether the
%       point (x, y) is located in the thumb of the slider
%   procedure DrawMoreButton (drawPressed : boolean) - draw the more button
%       hilighted or unhighlighted
%   procedure DrawLessButton (drawPressed : boolean) - draw the less button
%       hilighted or unhighlighted
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
% The inheritable variables of VerticalScrollBarClass are as follows:
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
%   sliderLength - The length that the thumb can slide.
%   thumbPicture - Actually picture of area behind the thumb.
%   thumbX, thumbY - The location of lower left corner of thumb.
%   arrowIncrement - The amount to increment when arrows pressed.
%   pageIncrement -  The amount to increment when page up/down pressed.
%   thumbSize - The "size" of the thumb, w.r.t. of the slider value.
%   {less/more}Arrow{Unpressed/Pressed}{X,Y} - The arrays of coordinates for
%            drawing the arrows in the line up/down.
%   thumbMovementMouseDY - The displacement of the mouse down from the lower
%   minThumbPosition - Y coordinate of the bottom most position of the thumb.
%   scrollBarLength - The total length of the scroll bar including arrows.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of VerticalScrollBarClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   slideWidth - The width of the actual slide.
%   markLength - The length of the marks on the sides of the slide.
%   thumbWidth - The size of the thumb transverse to direction of sliding.
%   thumbHeight - The size of the thumb in the direction of sliding.
%   drawPressed - Argument to DrawScrollThumb - Draw the thumb pressed.
%   drawUnpressed - Argument to DrawScrollThumb - Draw the thumb unpressed.
%   delayBetweenPageScrolling - Delay in msecs between page up/downs.
%   delayBetweenArrowScrolling - Delay in msecs between line up/downs.
%   WidgetGlobals.SCROLL_BAR_WIDTH - The width of the scroll bar.
%   INVALID_VALUE - Temporary value used to force thumb redraw.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Internal Variables available for inheriting classes
%   x, y - The location of the lower left corner of the (individual) widget
%   width, height - The width and height of the (individual) widget
%   showing - Is the (individual) widget visible
%   buttonColor - The colour of the top of a raised button
%   illuminatedColor - The color of the hilight on the raised button
%   riserSize - The size in pixels of the border on a raised button
%   INVALID_VALUE - The value of an integer when not initialized
%   sliderValue - The current value of the slider
%   minValue - The minimum value of the slider
%   maxValue - The minimum value of the slider
%   arrowIncrement - The amount to increment when arrows pressed
%   pageIncrement - The amount to increment when page up/down pressed
%   feedBackProcSet - Whether the user feedback procedure is set
%   UserFeedBackProcedure - The current user feedback procedure
%   scrollBarLength - The length of the entire slider in pixels
%   thumbPosition - The position of the lower left corner of thumb in pixels
%   delayBetweenPageScrolling - Delay in msecs between page downs
%   WidgetGlobals.SCROLL_BAR_WIDTH - Width of a slider in pixels (also size of arrow boxes)
%   minThumbPosition - The lowest possible location of the thumb in pixels
%   thumbSliderLength - The number of pixel positions possible for the thumb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class VerticalScrollBarClass
    inherit GenericScrollBarClass

    %
    % The inheritable variables of VerticalScrollBarClass
    %
    var lessArrowUnpressedX, lessArrowUnpressedY,
	lessArrowPressedX, lessArrowPressedY,
	moreArrowUnpressedX, moreArrowUnpressedY,
	moreArrowPressedX, moreArrowPressedY : array 1 .. 8 of int
    var thumbMovementMouseDY : int
    var minThumbPosition, scrollBarLength : int


    %
    % Overridden subprograms
    %
    body procedure ComputeWidgetPosition
	x := originalX
	y := originalY
	scrollBarLength := originalHeight
	width := WidgetGlobals.SCROLL_BAR_WIDTH + 1
	height := originalHeight + 1

	minThumbPosition := y + WidgetGlobals.SCROLL_BAR_WIDTH + 1
	sliderLength := scrollBarLength -
	    3 * WidgetGlobals.SCROLL_BAR_WIDTH - 2

	% Create the arrow polygons
	lessArrowUnpressedX (1) := x + 8
	lessArrowUnpressedY (1) := y + scrollBarLength - 4
	lessArrowUnpressedX (2) := x + 4
	lessArrowUnpressedY (2) := y + scrollBarLength - 8
	lessArrowUnpressedX (3) := x + 6
	lessArrowUnpressedY (3) := y + scrollBarLength - 8
	lessArrowUnpressedX (4) := x + 6
	lessArrowUnpressedY (4) := y + scrollBarLength - 12
	lessArrowUnpressedX (5) := x + 10
	lessArrowUnpressedY (5) := y + scrollBarLength - 12
	lessArrowUnpressedX (6) := x + 10
	lessArrowUnpressedY (6) := y + scrollBarLength - 8
	lessArrowUnpressedX (7) := x + 12
	lessArrowUnpressedY (7) := y + scrollBarLength - 8
	lessArrowUnpressedX (8) := x + 8
	lessArrowUnpressedY (8) := y + scrollBarLength - 4

	moreArrowUnpressedX (1) := x + 8
	moreArrowUnpressedY (1) := y + 4
	moreArrowUnpressedX (2) := x + 4
	moreArrowUnpressedY (2) := y + 8
	moreArrowUnpressedX (3) := x + 6
	moreArrowUnpressedY (3) := y + 8
	moreArrowUnpressedX (4) := x + 6
	moreArrowUnpressedY (4) := y + 12
	moreArrowUnpressedX (5) := x + 10
	moreArrowUnpressedY (5) := y + 12
	moreArrowUnpressedX (6) := x + 10
	moreArrowUnpressedY (6) := y + 8
	moreArrowUnpressedX (7) := x + 12
	moreArrowUnpressedY (7) := y + 8
	moreArrowUnpressedX (8) := x + 8
	moreArrowUnpressedY (8) := y + 4

	for cnt : 1 .. 8
	    lessArrowPressedX (cnt) := lessArrowUnpressedX (cnt) + 1
	    lessArrowPressedY (cnt) := lessArrowUnpressedY (cnt) - 1
	    moreArrowPressedX (cnt) := moreArrowUnpressedX (cnt) + 1
	    moreArrowPressedY (cnt) := moreArrowUnpressedY (cnt) - 1
	end for

	drawn := true
    end ComputeWidgetPosition


    body procedure DrawMoreButton (drawPressed : boolean)
	if drawPressed then
	    DrawPressedBox (x, y + scrollBarLength -
		WidgetGlobals.SCROLL_BAR_WIDTH)
	    Draw.FillPolygon (lessArrowPressedX, lessArrowPressedY,
		8, black)
	else
	    DrawUnpressedBox (x, y + scrollBarLength -
		WidgetGlobals.SCROLL_BAR_WIDTH)
	    if enabled and minValue not= maxValue then
		Draw.FillPolygon (lessArrowUnpressedX, lessArrowUnpressedY,
		    8, black)
	    else
		Draw.FillPolygon (lessArrowUnpressedX, lessArrowUnpressedY,
		    8, darkgrey)
	    end if
	end if
    end DrawMoreButton


    body procedure DrawLessButton (drawPressed : boolean)
	if drawPressed then
	    DrawPressedBox (x, y)
	    Draw.FillPolygon (moreArrowPressedX, moreArrowPressedY,
		8, black)
	else
	    DrawUnpressedBox (x, y)
	    if enabled and minValue not= maxValue then
		Draw.FillPolygon (moreArrowUnpressedX, moreArrowUnpressedY,
		    8, black)
	    else
		Draw.FillPolygon (moreArrowUnpressedX, moreArrowUnpressedY,
		    8, darkgrey)
	    end if
	end if
    end DrawLessButton


    body proc DrawScrollThumb (drawPressed : boolean)
	if drawPressed then
	    DrawPressedBox (x, minThumbPosition + thumbPosition)
	else
	    DrawUnpressedBox (x, minThumbPosition + thumbPosition)
	end if
    end DrawScrollThumb


    body procedure DrawThumb
	DrawScrollThumb (drawUnpressed)
    end DrawThumb


    body procedure DrawWidget
	if not drawn then
	    ComputeWidgetPosition
	end if

	% Draw the surrounding box
	Draw.Box (x, y, x + WidgetGlobals.SCROLL_BAR_WIDTH,
	    y + scrollBarLength, black)

	% Draw the arrows
	DrawLessButton (drawUnpressed)
	DrawMoreButton (drawUnpressed)

	if enabled and minValue not= maxValue then
	    % Draw the shaft
	    Draw.FillBox (x + 1, y + WidgetGlobals.SCROLL_BAR_WIDTH + 1,
		x + WidgetGlobals.SCROLL_BAR_WIDTH - 1,
		y + scrollBarLength - WidgetGlobals.SCROLL_BAR_WIDTH - 1,
		shaftColour)

	    % Draw the thumb
	    CalculateThumbPosition
	    DrawScrollThumb (drawUnpressed)
	else
	    Draw.FillBox (x + 1, y + WidgetGlobals.SCROLL_BAR_WIDTH + 1,
		x + WidgetGlobals.SCROLL_BAR_WIDTH - 1,
		y + scrollBarLength - WidgetGlobals.SCROLL_BAR_WIDTH - 1,
		disabledShaftColour)
	end if
    end DrawWidget


    body proc EraseThumb
	Draw.FillBox (x + 1, minThumbPosition + thumbPosition,
	    x + WidgetGlobals.SCROLL_BAR_WIDTH - 1,
	    minThumbPosition + thumbPosition +
	    WidgetGlobals.SCROLL_BAR_WIDTH, shaftColour)
    end EraseThumb


    body function InBounds (mx, my : int) : boolean
	result x <= mx and mx <= x + WidgetGlobals.SCROLL_BAR_WIDTH and
	    y <= my and my <= y + scrollBarLength
    end InBounds


    body function InBoundsMoreButton (mx, my : int) : boolean
	result x <= mx and mx <= x + WidgetGlobals.SCROLL_BAR_WIDTH and
	    y + scrollBarLength - WidgetGlobals.SCROLL_BAR_WIDTH <= my and
	    my <= y + scrollBarLength
    end InBoundsMoreButton


    body function InBoundsLessButton (mx, my : int) : boolean
	result x <= mx and mx <= x + WidgetGlobals.SCROLL_BAR_WIDTH and
	    y <= my and my <= y + WidgetGlobals.SCROLL_BAR_WIDTH
    end InBoundsLessButton


    body function InBoundsPageUp (mx, my : int) : boolean
	result x <= mx and mx <= x + WidgetGlobals.SCROLL_BAR_WIDTH and
	    y + WidgetGlobals.SCROLL_BAR_WIDTH < my and my <
	    minThumbPosition + thumbPosition
    end InBoundsPageUp


    body function InBoundsPageDown (mx, my : int) : boolean
	result x <= mx and mx <= x + WidgetGlobals.SCROLL_BAR_WIDTH and
	    minThumbPosition + thumbPosition +
	    WidgetGlobals.SCROLL_BAR_WIDTH < my and
	    my < y + scrollBarLength - WidgetGlobals.SCROLL_BAR_WIDTH
    end InBoundsPageDown


    body function InBoundsOnThumb (mx, my : int) : boolean
	result x <= mx and mx <= x + WidgetGlobals.SCROLL_BAR_WIDTH and
	    minThumbPosition + thumbPosition <= my and
	    my <= minThumbPosition + thumbPosition +
	    WidgetGlobals.SCROLL_BAR_WIDTH
    end InBoundsOnThumb


    body function CloseToSlider (mx, my : int) : boolean
	result x - WidgetGlobals.SCROLL_BAR_WIDTH <= mx and
	    mx <= x + 2 * WidgetGlobals.SCROLL_BAR_WIDTH
    end CloseToSlider


    body function GetSliderValueFromMouse (mx, my : int) : int
	const newThumbPosition : int := my - thumbMovementMouseDY
	var temp := minValue +
	    round ( (newThumbPosition - minThumbPosition) /
	    sliderLength * (maxValue - minValue))
	result min (maxValue, max (minValue, temp))
    end GetSliderValueFromMouse


    % This procedure is a little bit obscure because the mapping
    % from mouse position to slider value is relative to where the
    % mouse button down occurred.
    body procedure SetOriginalMouseDownForThumb (mx, my : int)
	thumbMovementMouseDY := my - (minThumbPosition + thumbPosition)
    end SetOriginalMouseDownForThumb
end VerticalScrollBarClass

