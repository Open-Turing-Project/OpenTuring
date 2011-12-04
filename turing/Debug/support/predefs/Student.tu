
/*
 ** module Student - Old Turing routines
 **
 ** NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 **      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Student
    export ~.*buttonchoose, ~.*buttonmoved, ~.*buttonwait,
	~.*cls, ~.*clock, ~.*color, ~.*colorback, ~.*colour, ~.*colourback, 
	~.*date, ~.*delay, ~.*drawarc, ~.*drawbox, ~.*drawdot, ~.*drawfill, 
	~.*drawfillarc, ~.*drawfillbox, ~.*drawfillmapleleaf, ~.*drawfilloval, 
	~.*drawfillpolygon, ~.*drawfillstar, ~.*drawline, ~.*drawmapleleaf, 
	~.*drawoval, ~.*drawpic, ~.*drawpolygon, ~.*drawstar, ~.*fetcharg,
	~.*locate, ~.*locatexy, ~.*mousehide, ~.*mouseshow, ~.*mousewhere,
	~.*nargs, ~.*parallelget, ~.*parallelput, ~.*play, ~.*playdone,  
	~.*rand, ~.*randint, ~.*randomize, ~.*setscreen, ~.*sizepic, ~.*sound,
	~.*sysclock, ~.*system, ~.*takepic, ~.*time, ~.*wallclock, ~.*whatcol,
	~.*whatcolor, ~.*whatcolorback, ~.*whatcolour, ~.*whatcolourback, 
	~.*whatdotcolor, ~.*whatdotcolour, ~.*whatrow

    /********************************************************************/
    /* Graphics                                                         */
    /********************************************************************/

    %
    % Draw an arc on the screen.
    %
    external "draw_arc" procedure drawarc (x, y, xRadius, yRadius : int,
	initialAngle, finalAngle : int, drawColour : int)

    %
    % Draw a box with the given bottom-left and top-right corners on the screen.
    %
    external "draw_box" procedure drawbox (x1, y1, x2, y2 : int, drawColour : int)

    %
    % Draw a dot (pixel) at the given location.
    %
    external "draw_dot" procedure drawdot (x, y : int, drawColour : int)

    %
    % Colour in a figure on the screen.
    %
    external "draw_fill" procedure drawfill (x, y : int, fillColour, borderColour : int)

    %
    % Draw a filled arc on the screen.
    %
    external "draw_fillarc" procedure drawfillarc (x, y, xRadius, yRadius : int,
	initialAngle, finalAngle : int, drawColour : int)

    %
    % Draw a filled box with the given bottom-left and top-right corners
    % on the screen.
    %
    external "draw_fillbox" procedure drawfillbox (x1, y1, x2, y2 : int, drawColour : int)

    %
    % Draw a filled maple leaf bounded by the past rectangle.
    %
    external "draw_fillmapleleaf" procedure drawfillmapleleaf (x1, y1, x2, y2 : int, drawColour : int)

    %
    % Draw a filled oval on the screen.
    %
    external "draw_filloval" procedure drawfilloval (x, y, xRadius, yRadius : int,
	drawColour : int)

    %
    % Draw the specified filled polygon on the screen.
    %
    external "draw_fillpolygon" procedure drawfillpolygon (x, y : array 1 .. * of int, n : int,
	drawColour : int)

    %
    % Draw a filled star bounded by the past rectangle.
    %
    external "draw_fillstar" procedure drawfillstar (x1, y1, x2, y2 : int, drawColour : int)

    %
    % Draw a line on the screen.
    %
    external "draw_line" procedure drawline (x1, y1, x2, y2 : int, drawColour : int)

    %
    % Draw a maple leaf bounded by the past rectangle.
    %
    external "draw_mapleleaf" procedure drawmapleleaf (x1, y1, x2, y2 : int, drawColour : int)

    %
    % Draw an oval on the screen.
    %
    external "draw_oval" procedure drawoval (x, y, xRadius, yRadius : int, drawColour : int)


    %
    % Draw the specified polygon on the screen.
    %
    external "draw_polygon" procedure drawpolygon (x, y : array 1 .. * of int, n : int,
	drawColour : int)

    %
    % Draw a star bounded by the past rectangle.
    %
    external "draw_star" procedure drawstar (x1, y1, x2, y2 : int, drawColour : int)


    %
    % Draw the buffered figure on the screen.
    %
    external "old_drawpic"
	procedure drawpic (x, y : int, buffer : array 1 .. * of int,
	drawMode : int)

    %
    % Determine the buffer size required to hold the rectangular figure
    % enclosed by the box with the specified bottom-left and top-right corners.
    %
    external "old_sizepic" function sizepic (x1, y1, x2, y2 : int) : int

    %
    % Save the rectangular figure enclosed by the box with the
    % specified bottom-left and top-right corners.
    %
    external "old_takepic" procedure takepic (x1, y1, x2, y2 : int,
	var buffer : array 1 .. * of int)

    /********************************************************************/
    /* Text                                                             */
    /********************************************************************/

    %
    % Clear the screen.
    %
    external "text_cls" procedure cls

    %
    % Set the colour to be used when printing text.
    %
    external "text_colour" procedure color (newColour : int)
    external "text_colour" procedure colour (newColour : int)

    %
    % Set the currently active background colour.
    %
    external "text_colourback" procedure colorback (newColour : int)
    external "text_colourback" procedure colourback (newColour : int)

    %
    % Move the cursor to the specified location on the screen.
    %
    external "text_locate" procedure locate (row, column : int)

    %
    % Move the cursor to the specified location on the screen.
    %
    external "text_locatexy" procedure locatexy (x, y : int)

    %
    % Determine the colour used when printing text.
    %
    external "text_whatcolour" function whatcolor : int
    external "text_whatcolour" function whatcolour : int

    %
    % Determine the current background colour.
    %
    external "text_whatcolourback" function whatcolorback : int
    external "text_whatcolourback" function whatcolourback : int

    %
    % Determine the current cursor column position.
    %
    external "text_whatcol" function whatcol : int


    %
    % Determine the current cursor row position.
    %
    external "text_whatrow" function whatrow : int

    %
    % Determine the character and its colours on the screen at the
    % current cursor position.
    %
    external procedure text_whatchar (row, col : int, var ch : char,
	foreColor, backColor : int)

    /********************************************************************/
    /* View                                                             */
    /********************************************************************/

    %
    % Determine the colour of the specified dot (pixel).
    %
    external "view_whatdotcolour" function whatdotcolor (x, y : int) : int
    external "view_whatdotcolour" function whatdotcolour (x, y : int) : int


    %
    % Set the current mode of the screen.
    %
    external "view_set" procedure setscreen (modes : string)

    /********************************************************************/
    /* Random Numbers                                                   */
    /********************************************************************/

    %
    % Generate a pseudo-random number from zero to one.
    %
    external "rand_real" procedure rand (var r : real)

    %
    % Generate a pseudo-random integer in the given range.
    %
    external "rand_int" procedure randint (var i : int, low, high : int)

    %
    % Reset the pseudo-random number sequences to a pseudo-random state.
    %
    external "rand_randomize" procedure randomize

    /********************************************************************/
    /* System Routines                                                  */
    /********************************************************************/

    %
    % Return the number of arguments on the command line.
    % Eg. for command "t.x y", nargs returns 1.
    %
    external "sys_nargs" function nargs : int

    %
    % Return the i'th argument from the command line.  The 0'th
    % argument is the command name.  0 <= i <= nargs.  The argument may be
    % truncated to fit.
    %
    external "sys_fetcharg" function fetcharg (i : int) : string

    %
    % Executes the shell command str, as if it was typed at the terminal.
    % ret is assigned the return code from the command.  ret := 127 if
    % the shell cannot be executed.  /bin/sh is used for the execution
    %
    procedure system (str : string, var ret : int)
	external "sys_exec" function sysexec (str : string) : boolean

	if sysexec (str) then
	    ret := 0
	else
	    if Error.Last = eSysNotEnoughSystemMemory then
		ret := -2
	    elsif Error.Last = eSysSysExecNotAllowed then
		ret := -6
	    else
		ret := -7
	    end if
	end if
    end system

    /********************************************************************/
    /* Time Routines                                                    */
    /********************************************************************/

    %
    % Assigns the current date to the variable d in the format
    % "dd mmm yy", where mmm is the first 3 characters of the month,
    % ie. "Apr".
    %
    procedure date (var d : string)
	external "time_date" function datetime : string

	var dtStr : string := datetime

	d := dtStr (1 .. 9)
    end date

    %
    % Assigns the current time to the variable t in the format "hh:mm:ss".
    %
    procedure time (var t : string)
	external "time_date" function datetime : string

	var dtStr : string := datetime

	t := dtStr (11 .. *)
    end time


    %
    % Assigns the number of central processor milliseconds of user time
    % used by this process to the variable c.
    %
    external "time_elapsed" procedure clock (var c : int)

    %
    % Assigns the number of central processor milliseconds of system time
    % used by this process to the variable c.
    %
    external "time_elapsedcpu" procedure sysclock (var c : int)

    %
    % Assigns the time in seconds since 00:00:00 GMT January 1, 1970 to
    % the variable c.
    %
    external "time_sec" procedure wallclock (var c : int)

    %
    % Pause the program for a given amount of real-time.
    % The duration argument is in milliseconds.
    %
    external "time_delay" procedure delay (duration : int)

    /********************************************************************/
    /* Sound and Music                                                  */
    /********************************************************************/

    %
    % Sound the specified musical notes.
    %
    external "music_play" procedure play (music : string)

    %
    % Determine when the notes played (asynchronously) by play, above,
    % have finished sounding.
    %
    external "music_playdone" function playdone : boolean

    %
    % Sound a note of the given frequency for a given time.
    % The frequency is specified in Hz and the duration in milliseconds.
    %
    external "music_sound" procedure sound (frequency, duration : int)

    /********************************************************************/
    /* Mouse                                                            */
    /********************************************************************/

    external "mouse_buttonchoose"
	procedure buttonchoose (chooseStr : string)

    external "mouse_buttonmoved"
	function buttonmoved (motionStr : string) : boolean

    external "mouse_buttonwait"
	procedure buttonwait (motionStr : string, var x, y : int,
	var btnNumber, buttonUpDown : int)
    external "mouse_where"
	procedure mousewhere (var x, y, buttonUpDown : int)

    external "mouse_hide" procedure mousehide

    external "mouse_show" procedure mouseshow

    /********************************************************************/
    /* Parallel Port                                                    */
    /********************************************************************/

    external procedure pc_parallelput (portNumber, value : int)
    external function pc_parallelget (portNumber : int) : int

    procedure parallelput (x : int)
	pc_parallelput (1, x)
    end parallelput

    function parallelget : int
	result pc_parallelget (1)
    end parallelget
end Student

