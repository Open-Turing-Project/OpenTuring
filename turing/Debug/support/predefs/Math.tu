/*
 * Math module - various mathematical routines
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Math
    export E, PI, Distance, DistancePointLine, ~.*arccos, ~.*arccosd, ~.*arcsin, ~.*arcsind,
	~.*arctan, ~.*arctand, ~.*cos, ~.*cosd, ~.*exp,
	~.*ln, ~.*sign, ~.*sin, ~.*sind, ~.*sqrt, ~.*tan, ~.*tand

    % Constants
    const E : real := 2.71828182845904523536
    const PI : real := 3.14159265358979323846

    %
    % Determine the arc tangent of the given value, with result in radians.
    %
    external "math_arctan" function arctan (r : real) : real

    %
    % Determine the arc tangent of the given value, with result in degrees.
    %
    external "math_arctand" function arctand (r : real) : real

    %
    % Determine the cosine of the given angle in radians.
    %
    external "math_cos" function cos (r : real) : real

    %
    % Determine the cosine of the given angle in degrees.
    %
    external "math_cosd" function cosd (r : real) : real

    %
    % Determine the natural exponent (e ** r) of the given value.
    %
    external "math_exp" function exp (r : real) : real

    %
    % Determine the natural logarithm of the given value.
    %
    external "math_ln" function ln (r : real) : real

    %
    % Determine whether the given value is positive, zero, or negative.
    %
    external "math_sign" function sign (r : real) : -1 .. 1

    %
    % Determine the sine of the given angle in radians.
    %
    external "math_sin" function sin (r : real) : real

    %
    % Determine the sine of the given angle in degrees.
    %
    external "math_sind" function sind (r : real) : real

    %
    % Determine the square root of the given value.
    %
    external "math_sqrt" function sqrt (r : real) : real

    %
    % Determine the arc sine of the given value, with result in radians.
    %
    function arcsin (r : real) : real
	if r = 1 then
	    result PI / 2
	elsif r = 0 then
	    result 0
	elsif r = -1 then
	    result - PI / 2
	elsif r < -1 or r > 1 then
	    quit < : excpTrigArgumentTooLarge
	elsif r < 0 then
	    result - arctan (sqrt ((r * r) / (1 - r * r)))
	else
	    result arctan (sqrt ((r * r) / (1 - r * r)))
	end if
    end arcsin

    %
    % Determine the arc sine of the given value, with result in degrees.
    %
    function arcsind (r : real) : real
	if r = 1 then
	    result 90
	elsif r = 0 then
	    result 0
	elsif r = -1 then
	    result - 90
	elsif r < -1 or r > 1 then
	    quit < : excpTrigArgumentTooLarge
	elsif r < 0 then
	    result - arctand (sqrt ((r * r) / (1 - r * r)))
	else
	    result arctand (sqrt ((r * r) / (1 - r * r)))
	end if
    end arcsind

    %
    % Determine the arc cosine of the given value, with result in radians.
    %
    function arccos (r : real) : real
	if r = 1 then
	    result 0
	elsif r = 0 then
	    result PI / 2
	elsif r = -1 then
	    result PI
	elsif r < -1 or r > 1 then
	    quit < : excpTrigArgumentTooLarge
	elsif r > 0 then
	    result arctan (sqrt ((1 - r * r) / (r * r)))
	else
	    result PI - arctan (sqrt ((1 - r * r) / (r * r)))
	end if
    end arccos

    %
    % Determine the arc cosine of the given value, with result in degrees.
    %
    function arccosd (r : real) : real
	if r = 1 then
	    result 0
	elsif r = 0 then
	    result 90
	elsif r = -1 then
	    result 180
	elsif r < -1 or r > 1 then
	    quit < : excpTrigArgumentTooLarge
	elsif r > 0 then
	    result arctand (sqrt ((1 - r * r) / (r * r)))
	else
	    result 180 - arctand (sqrt ((1 - r * r) / (r * r)))
	end if
    end arccosd

    %
    % Determine the tangent of a given angle in radians
    %
    function tan (r : real) : real
	if cos (r) = 0 then
	    quit < : excpRealOverflow
	else
	    result sin (r) / cos (r)
	end if
    end tan

    %
    % Determine the tangent of a given angle in degrees
    %
    function tand (r : real) : real
	if r mod 180 = 90 then
	    quit < : excpRealOverflow
	else
	    result sind (r) / cosd (r)
	end if
    end tand

    %
    % Calculate the distance between two points
    function Distance (x1, y1, x2, y2 : real) : real
	var dx : real := x1 - x2
	var dy : real := y1 - y2
	result sqrt (dx * dx + dy * dy)
    end Distance

    %
    % Calculate the distance between a point and a line segment
    %
    function DistancePointLine (px, py, x1, y1, x2, y2 : real) : real
	var lineSize : real := Distance (x1, y1, x2, y2)
	if lineSize = 0 then
	    result Distance (px, py, x1, y1)
	end if

	var u : real := ((px - x1) * (x2 - x1) +
	    (py - y1) * (y2 - y1)) / (lineSize * lineSize)

	if u < 0.0 then
	    result Distance (px, py, x1, y1)
	elsif u > 1.0 then
	    result Distance (px, py, x2, y2)
	else
	    var ix : real := x1 + u * (x2 - x1)
	    var iy : real := y1 + u * (y2 - y1)
	    result Distance (px, py, ix, iy)
	end if
    end DistancePointLine
end Math
