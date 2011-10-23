unit

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% GenericSliderClass - Abstract class that that implements sliders.  The
%                      original SliderWidget was the work of teacher
%                      Greg Clarke.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The external interface of GenericSliderClass is as follows:
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
% The package interface of GenericSliderClass is as follows:
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
%                       this is the default widget), the userActionProc for
%                       the widget is called and the function returns true,
%                       otherwise it returns false.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable subprograms of GenericSliderClass are as follows:
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
% The inheritable variables of GenericSliderClass are as follows:
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
%   thumbX, thumbY - The location of lower left corner of thumb
%   thumbPosition - The number of pixels from origin from thumb centre.
%   thumbSize - The "size" of the thumb, w.r.t. of the slider value.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The inheritable constants of GenericSliderClass are as follows:
%   buttonColor - The colour of the top of a raised button.
%   illuminatedColor - The color of the hilight on the raised button.
%   riserSize - The size in pixels of the border on a raised button.
%   slideWidth - The width of the actual slide.
%   markLength - The length of the marks on the sides of the slide.
%   thumbWidth - The size of the thumb transverse to direction of sliding.
%   thumbHeight - The size of the thumb in the direction of sliding.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The deferred subprograms are as follows:
%   procedure DrawWidget - Draw the widget.
%   procedure ComputeWidgetPosition - Compute the location of widget, etc.
%   procedure DrawThumb - Draw the thumb
%   function InThumb (mx, my : int) : boolean - returns whether the
%       point (mx, my) is located in the thumb of the slider
%   function CloseToSlider (mx, my : int) : boolean - returns whether the
%       point (x, y) is located near enough to the slider
%   function GetSliderValueFromMouse (mx, my : int) : int - returns the slider
%       value corresponding to the change in mouse position
%   body procedure SetOriginalMouseDownForThumb (mx, my : int) - called when
%       the user first puts the mouse down in the thumb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class GenericSliderClass
    inherit GenericActiveWidgetClass

    %
    % Exported routines for GenericSliderClass
    %
    export
    % The external interface
	GetSliderValue, Initialize, SetMinMax, SetSliderValue,
	SetSliderSize, SetSliderReverse


    %
    % The inheritable variables of GenericSliderClass
    %
    var userActionProc : procedure userActionProc (sliderValue : int)
    var sliderValue : int := 0
    var minValue, origMin : int
    var maxValue, origMax : int
    var sliderLength : int % The length of the slider.
    var thumbPicture : int := -1
    % Actually picture of area behind the thumb.
    var thumbX, thumbY : int % The location of lower left corner of thumb.
    % The number of pixels from origin from thumb centre.
    var thumbPosition : int
    var thumbSize : int := 1
    var reversed : boolean := false
    var trackColour : int
    var markColour : int
    if colourbg = black then
	if backgroundColour = colourbg then
	    trackColour := brightwhite
	    markColour := white
	else
	    trackColour := white
	    markColour := darkgrey
	end if
    else
	trackColour := grey
	markColour := darkgrey
    end if


    %
    % The inheritable constants of GenericSliderClass
    %
    const slideWidth : int := 8
    const markLength : int := 10
    const thumbWidth : int := 20 % Transverse to slider
    const thumbHeight : int := 17 % In same direction as slider


    %
    % The inheritable subprograms of GenericSliderClass
    %
    deferred procedure DrawThumb
    deferred procedure EraseThumb
    deferred function InThumb (mx, my : int) : boolean
    deferred function CloseToSlider (mx, my : int) : boolean
    deferred function GetSliderValueFromMouse (mx, my : int) : int
    deferred procedure SetOriginalMouseDownForThumb (mx, my : int)


    procedure CalculateThumbPosition
	% Make certain minValue and maxValue are set
	if not initialized then
	    AssertFailed ("Slider not initialized.")
	end if

	if minValue = maxValue then
	    thumbPosition := 0
	else
	    thumbPosition := round (((sliderValue - minValue) /
		(maxValue - minValue)) * sliderLength)
	end if
    end CalculateThumbPosition


    body procedure EraseThumb
	if thumbPicture = -1 then
	    assert false
	end if
	Pic.Draw (thumbPicture, thumbX, thumbY, picCopy)
	Pic.Free (thumbPicture)
	thumbPicture := -1
    end EraseThumb


    body procedure ActionProc
	if reversed then
	    userActionProc (maxValue - sliderValue + minValue)
	else
	    userActionProc (sliderValue)
	end if
    end ActionProc


    procedure SetValueSpecifyThumb (newValue : int)
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
	    DrawThumb
	end if

	% Call the feedback function
	GeneralActionProc
    end SetValueSpecifyThumb


    procedure SetSliderReverse
	reversed := not reversed

	% Erase the thumb in the old location
	if enabled and showing and minValue not= maxValue then
	    EraseThumb
	end if

	% Make certain slider value falls between minimum and maximum
	sliderValue := maxValue - sliderValue + minValue

	% Show it in the new location
	if enabled and showing and minValue not= maxValue then
	    CalculateThumbPosition
	    DrawThumb
	end if
    end SetSliderReverse


    %
    % The external subprograms of GenericSliderClass
    %
    function GetSliderValue : int
	if reversed then
	    result maxValue - sliderValue + minValue
	else
	    result sliderValue
	end if
    end GetSliderValue


    procedure Initialize (newX, newY, newLength, newMin, newMax : int,
	    startValue : int, newActionProc : proc x (sliderValue : int))
	% Note that the height will be ignored for horizontal sliders
	% and the width ignored for vertical sliders.
	origMin := newMin
	origMax := newMax
	SetPositionAndSize (newX, newY, newLength, newLength)

	minValue := newMin
	% Note that thumbsize = 0 for sliders, possible non-zero for scrollbars
	maxValue := max (minValue, newMax - thumbSize + 1)
	sliderValue := min (maxValue, max (minValue, startValue))
	userActionProc := newActionProc

	GeneralInitialize
    end Initialize


    % Set the minimum and maximum values of the sliders.
    procedure SetMinMax (newMinValue, newMaxValue : int)
	var wasEnabled : boolean := false

	if newMinValue > newMaxValue then
	    AssertFailed ("Minimum value must be less than maximum!")
	end if

	origMin := newMinValue
	origMax := newMaxValue

	% Erase the thumb in the old location
	if enabled and showing and minValue not= maxValue then
	    wasEnabled := true
	    EraseThumb
	end if
	minValue := newMinValue
	maxValue := max (minValue, newMaxValue - thumbSize + 1)

	% Make certain slider value falls between minimum and maximum
	if reversed then
	    sliderValue := min (maxValue, max (minValue,
		sliderValue + (origMax - maxValue)))
	else
	    sliderValue := min (maxValue, max (minValue, sliderValue))
	end if

	% Show it in the new location
	if enabled and showing and minValue not= maxValue then
	    CalculateThumbPosition
	    if not wasEnabled then
		DrawWidget
	    else
		DrawThumb
	    end if
	else
	    if wasEnabled then
		DrawWidget
	    end if
	end if
    end SetMinMax


    procedure SetSliderValue (newValue : int)
	if reversed then
	    SetValueSpecifyThumb (maxValue - newValue + minValue)
	else
	    SetValueSpecifyThumb (newValue)
	end if
    end SetSliderValue


    procedure SetSliderSize (size : int)
	SetSize (size, size)
    end SetSliderSize


    %
    % Overridden subprograms
    %
    body function ConsiderButtonDown (mouseX, mouseY : int) : boolean
	assert initialized
	if not enabled or not showing then
	    result false
	end if

	var mx, my, b : int
	var pressed : boolean := true

	if InThumb (mouseX, mouseY) then
	    % Check for clicking on the thumb

	    const originalSliderValue := sliderValue
	    pressed := true

	    SetOriginalMouseDownForThumb (mouseX, mouseY)
	    loop
		Mouse.Where (mx, my, b)
		if (CloseToSlider (mx, my)) and not pressed then
		    pressed := true
		elsif (not CloseToSlider (mx, my)) and pressed then
		    pressed := false
		    SetValueSpecifyThumb (originalSliderValue)
		end if

		if pressed then
		    const newValue := GetSliderValueFromMouse (mx, my)
		    if newValue not= sliderValue then
			SetValueSpecifyThumb (newValue)
		    end if
		end if

		exit when b = 0
	    end loop

	    result true
	else
	    result false
	end if
    end ConsiderButtonDown


    body function ConsiderKeystroke (key : char) : boolean
	result false
    end ConsiderKeystroke


    body procedure EraseWidget
	if not initialized then
	    AssertFailed ("The object has not been initialized yet")
	else
	    if not drawn then
		ComputeWidgetPosition
	    end if
	    Draw.FillBox (x, y, x + width - 1, y + height - 1,
		backgroundColour)
	    if thumbPicture not= -1 then
		Pic.Free (thumbPicture)
		thumbPicture := -1
	    end if
	end if
    end EraseWidget
end GenericSliderClass

