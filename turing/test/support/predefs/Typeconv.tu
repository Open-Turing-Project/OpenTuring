/*
 * Typeconv module - type conversions and tests
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module TypeConv
    export ~.*intreal, ~.*natreal, ~.*round, ~.*floor, ~.*ceil, ~.*realstr,
	~.*erealstr, ~.*frealstr, ~.*strreal, ~.*strrealok

    /* Type conversions */

    %
    % Convert the given integer value to a real.
    %
    external "typeconv_intreal" function intreal (i : int) : real

    %
    % Convert the given integer value to a real.
    %
    external "typeconv_natreal" function natreal (n : nat) : real

    %
    % Determine the nearest integer to the given value.
    %
    external "typeconv_round" function round (r : real) : int

    %
    % Determine the largest integer less than or equal to the given real value.
    %
    external "typeconv_floor" function floor (r : real) : int

    %
    % Determine the smallest integer greater than or equal to the given real value.
    %
    external "typeconv_ceil" function ceil (r : real) : int

    %
    % Convert the given real value to a string in the format specified.
    %
    external "typeconv_realstr"
	function realstr (r : real, width : int) : string

    %
    % Convert the given real value to a string in the format specified.
    %
    external "typeconv_erealstr" function erealstr (r : real,
	width, fractionWidth, exponentWidth : int) : string

    %
    % Convert the given real value to a string in the format specified.
    %
    external "typeconv_frealstr"
	function frealstr (r : real, width, fractionWidth : int) : string

    %
    % Convert the given string to a real value.
    %
    external "typeconv_strreal" function strreal (s : string) : real


    /* Type conversion tests */

    %
    % Return true if strint would succeed on string
    %
    %external "typeconv_strintok" function strintok (str: string): boolean

    %
    % Return true if strnat would succeed on string
    %
    %external "typeconv_strnatok" function strnatok (str: string): boolean

    %
    % Return true if strreal would succeed on string
    %
    function strrealok (s : string) : boolean
	handler (e)
	    result false
	end handler
	var r : real := strreal (s)
	result true
    end strrealok
end TypeConv
