unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericScrollBarClass - Abstract class that that implements scroll bars
%                         much as they appear in MS Windows 3.1.  The original
%                          SliderWidget was the work of teacher Greg Clarke.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of GenericScrollBarClass is as follows:
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
% The package interface of GenericScrollBarClass is as follows:
%   function ConsiderButtonDown (x, y : int) : boolean
%                       x, y - The location of of the mouse when the button
%                              was pressed.
%                       Handle the mouse button being pressed.  If it occured
%                       in the widget, handle the event.  Return true if the
%                       event is handled by this function.  Return false
%                       otherwise.  If the widget was pressed, call the
%                       userActionProc for this widget.
%   function ConsiderKeystroke (key : char) : boolean
%                       key - The key pressed.
%                       Handle a keystroke. If the keystroke is the
%                       shortcut for the widget (or the user pressed ENTER and
%                       this is the default widget), the userActionProc for the
%                       widget is called and the function returns true,
%                       otherwise it returns false.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of GenericScrollBarClass are as follows:
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
% The inheritable variables of GenericScrollBarClass are as follows:
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
%   userActionProc - The current user feedback procedure.
%   sliderValue - The current value of the slider.
%   minValue - The minimum value of the slider.
%   maxValue - The maximum value of the slider.
%   sliderLength - The length of the slider.
%   thumbPicture - Actually picture of area behind the thumb.
%   thumbX, thumbY - The location of lower left corner of thumb.
%   thumbPosition - The number of pixels from origin for thumb LL corner.
%   thumbSize - The "size" of the thumb, w.r.t. of the slider value.
%   arrowIncrement - The amount to increment when arrows pressed.
%   pageIncrement -  The amount to increment when page up/down pressed.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of GenericScrollBarClass are as follows:
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
%   INVALID_VALUE - Temporary value used to force thumb redraw.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
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
%   function CloseToSlider (mx, my : int) : boolean - returns whether the
%       point (x, y) is located near enough to the slider
%   function GetSliderValueFromMouse (mx, my : int) : int - returns the slider
%       value corresponding to the change in mouse position
%   body procedure SetOriginalMouseDownForThumb (mx, my : int) - called when
%       the user first puts the mouse down in the thumb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericScrollBarClass
    inherit GenericSliderClass

    %
    % Exported routines for SliderClass
    %
    export
    % The external interface
	SetScrollAmount, InitializeFull

    %
    % The inheritable variables of GenericScrollBarClass
    %
    var arrowIncrement : int := 0
    var pageIncrement : int := 0
    var shaftColour, disabledShaftColour : int
    if colourbg = black then
	shaftColour := white
	disabledShaftColour := brightwhite
    else
	shaftColour := grey
	disabledShaftColour := white
    end if

    %
    % The inheritable constants of GenericScrollBarClass
    %
    const drawPressed : boolean := true
    const drawUnpressed : boolean := false
    const delayBetweenPageScrolling : int := 133
    const delayBetweenArrowScrolling : int := 33
    const INVALID_VALUE : int := - 999


    %
    % The inheritable subprograms of GenericScrollBarClass
    %
    deferred function InBounds (mx, my : int) : boolean
    deferred function InBoundsMoreButton (mx, my : int) : boolean
    deferred function InBoundsLessButton (mx, my : int) : boolean
    deferred function InBoundsPageUp (mx, my : int) : boolean
    deferred function InBoundsPageDown (mx, my : int) : boolean
    deferred function InBoundsOnThumb (mx, my : int) : boolean
    deferred procedure DrawMoreButton (drawPressed : boolean)
    deferred procedure DrawLessButton (drawPressed : boolean)
    deferred procedure DrawScrollThumb (drawPressed : boolean)


    procedure SetValueSpecifyScrollThumb (newValue : int, drawThumbPressed :
	    boolean)
	% Erase the thumb in the old location
	if enabled and showing and minValue not= maxValue then
	    EraseThumb
	end if

	% Make certain minValue and maxValue are set
	if not initialized then
	    AssertFailed ("slider not initialized")
	end if

	% Make certain slider value falls between minimum and maximum
	sliderValue := min (maxValue, max (minValue, newValue))

	% Show it in the new location
	if enabled and showing and minValue not= maxValue then
	    CalculateThumbPosition
	    DrawScrollThumb (drawThumbPressed)
	end if

	% Call the feedback function
	GeneralActionProc
    end SetValueSpecifyScrollThumb


    procedure DrawUnpressedBox (x, y : int)
	const x1 : int := x
	const y1 : int := y
	const x2 : int := x + WidgetGlobals.SCROLL_BAR_WIDTH
	const y2 : int := y + WidgetGlobals.SCROLL_BAR_WIDTH
	const rise : int := riserSize + 1

	Draw.Box (x1, y1, x2, y2, black)

	for offset : 1 .. riserSize
	    Draw.Line (x1 + offset, y1 + offset, x2 - offset, y1 + offset,
		darkgrey)
	    Draw.Line (x2 - offset, y1 + offset, x2 - offset, y2 - offset,
		darkgrey)
	    Draw.Line (x1 + offset, y1 + offset, x1 + offset, y2 - offset,
		illuminatedColor)
	    Draw.Line (x1 + offset, y2 - offset, x2 - offset, y2 - offset,
		illuminatedColor)
	end for
	Draw.FillBox (x1 + rise, y1 + rise, x2 - rise, y2 - rise,
	    buttonColor)
    end DrawUnpressedBox


    procedure DrawPressedBox (x, y : int)
	const x1 : int := x
	const y1 : int := y
	const x2 : int := x + WidgetGlobals.SCROLL_BAR_WIDTH
	const y2 : int := y + WidgetGlobals.SCROLL_BAR_WIDTH
	const rise : int := riserSize + 1

	Draw.Box (x1, y1, x2, y2, black)

	Draw.FillBox (x1 + 1, y1 + 1, x2 - 1, y2 - 1, buttonColor)

	for offset : 1 .. rise - 1
	    Draw.Line (x1 + offset, y1 + offset, x1 + offset, y2 - offset,
		darkgrey)
	    Draw.Line (x1 + offset, y2 - offset, x2 - offset, y2 - offset,
		darkgrey)
	end for
    end DrawPressedBox


    %
    % The external subprograms of GenericScrollBarClass
    %
    procedure InitializeFull (newX, newY, newLength, newMin, newMax : int,
	    startValue : int, newActionProc : proc x (sliderValue : int),
	    newArrowIncrement, newPageIncrement, newThumbSize : int)
	thumbSize := max (newThumbSize, 1)
	arrowIncrement := newArrowIncrement
	pageIncrement := newPageIncrement
	GenericSliderClass.Initialize (newX, newY, newLength, newMin, newMax,
	    startValue, newActionProc)
    end InitializeFull


    body procedure Initialize (newX, newY, newLength, newMin : int,
	    newMax : int, startValue : int,
	    newActionProc : proc x (sliderValue : int))
	    InitializeFull (newX, newY, newLength, newMin, newMax,
		startValue, newActionProc, 1, (newMax - newMin) div 4, 1)
	end Initialize


	% Set the scrolling amounts and thumb size of the scroll bar.
	% arrowInc is the change in the value when an arrow is pressed,
	% pageInc is the change in the slider when the mouse is pressed
	% in the page up, page down section.
	procedure SetScrollAmount (newArrowIncrement, newPageIncrement,
		newThumbSize : int)

	    arrowIncrement := newArrowIncrement
	    pageIncrement := newPageIncrement

	    if showing and thumbSize not= newThumbSize then
		EraseWidget
		% Make certain slider value falls between minimum and maximum
		maxValue := max (minValue, origMax - newThumbSize)
		thumbSize := newThumbSize
		sliderValue := max (minValue,
		    min (maxValue - thumbSize, sliderValue))
		DrawWidget
	    else
		maxValue := max (minValue, origMax - newThumbSize)
		thumbSize := newThumbSize
		% Make certain slider value falls between minimum and maximum
		sliderValue := max (minValue,
		    min (maxValue - thumbSize, sliderValue))
	    end if
	end SetScrollAmount


	%
	% Overridden subprograms
	%
	% Set the minimum and maximum values of the sliders.
	body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	    assert initialized
	    if not enabled or not showing or minValue = maxValue then
		result false
	    end if

	    % If the click occurred outside of the slider, immediately return false.
	    if not InBounds (mouseX, mouseY) then
		result false
	    end if

	    var mx, my, b : int
	    var pressed : boolean := true
	    var delayMultiplier : real

	    if InBoundsMoreButton (mouseX, mouseY) then
		% Check for clicking on the more arrow (bottom, right of the slider)

		DrawMoreButton (drawPressed)
		pressed := true
		delayMultiplier := 5.0

		loop
		    if pressed and sliderValue not= maxValue then
			SetValueSpecifyScrollThumb (sliderValue +
			    arrowIncrement, drawUnpressed)
		    end if
		    Time.Delay (round ( (delayMultiplier + 1) *
			delayBetweenArrowScrolling))
		    delayMultiplier *= 0.75
		    Mouse.Where (mx, my, b)
		    if InBoundsMoreButton (mx, my) and not pressed then
			pressed := true
			DrawMoreButton (drawPressed)
		    elsif not InBoundsMoreButton (mx, my) and pressed then
			pressed := false
			DrawMoreButton (drawUnpressed)
		    end if
		    exit when b = 0
		end loop

		DrawMoreButton (drawUnpressed)
		result true

	    elsif InBoundsLessButton (mouseX, mouseY) then
		% Check for clicking on the less arrow (top, left of the slider)

		DrawLessButton (drawPressed)
		pressed := true
		delayMultiplier := 5.0

		loop
		    if pressed and sliderValue not= minValue then
			SetValueSpecifyScrollThumb (sliderValue -
			    arrowIncrement, drawUnpressed)
		    end if
		    Time.Delay (round ( (delayMultiplier + 1) *
			delayBetweenArrowScrolling))
		    delayMultiplier *= 0.75
		    Mouse.Where (mx, my, b)
		    if InBoundsLessButton (mx, my) and not pressed then
			pressed := true
			DrawLessButton (drawPressed)
		    elsif not InBoundsLessButton (mx, my) and pressed then
			pressed := false
			DrawLessButton (drawUnpressed)
		    end if
		    exit when b = 0
		end loop

		DrawLessButton (drawUnpressed)
		result true

	    elsif InBoundsPageDown (mouseX, mouseY) then
		% Check for clicking on the page down (below thumb)

		pressed := true

		loop
		    if pressed then
			SetValueSpecifyScrollThumb (sliderValue +
			    pageIncrement,
			    drawUnpressed)
		    end if
		    Time.Delay (delayBetweenPageScrolling)
		    Mouse.Where (mx, my, b)
		    if InBoundsPageDown (mx, my) and not pressed then
			pressed := true
		    elsif not InBoundsPageDown (mx, my) and pressed then
			pressed := false
		    end if
		    exit when b = 0
		end loop

		result true

	    elsif InBoundsPageUp (mouseX, mouseY) then
		% Check for clicking on the page up (above thumb)

		pressed := true

		loop
		    if pressed then
			SetValueSpecifyScrollThumb (sliderValue -
			    pageIncrement,
			    drawUnpressed)
		    end if
		    Time.Delay (delayBetweenPageScrolling)
		    Mouse.Where (mx, my, b)
		    if InBoundsPageUp (mx, my) and not pressed then
			pressed := true
		    elsif not InBoundsPageUp (mx, my) and pressed then
			pressed := false
		    end if
		    exit when b = 0
		end loop

		result true

	    elsif InBoundsOnThumb (mouseX, mouseY) then
		% Check for clicking on the thumb

		const originalSliderValue := sliderValue
		pressed := true

		DrawScrollThumb (drawPressed)
		SetOriginalMouseDownForThumb (mouseX, mouseY)
		loop
		    Mouse.Where (mx, my, b)
		    if (CloseToSlider (mx, my)) and not pressed then
			pressed := true
			% To make certain thumb gets drawn down below.
			sliderValue := INVALID_VALUE
			DrawScrollThumb (drawPressed)
		    elsif (not CloseToSlider (mx, my)) and pressed then
			pressed := false
			SetValueSpecifyScrollThumb (originalSliderValue,
			    drawUnpressed)
		    end if

		    if pressed then
			const newValue := GetSliderValueFromMouse (mx, my)
			if newValue not= sliderValue then
			    SetValueSpecifyScrollThumb (newValue,
				drawPressed)
			end if
		    end if

		    exit when b = 0
		end loop

		DrawScrollThumb (drawUnpressed)

		result true
	    end if
	end ConsiderButtonDown

	body function ConsiderKeystroke (key : char) : boolean
	    result false
	end ConsiderKeystroke
    end GenericScrollBarClass

