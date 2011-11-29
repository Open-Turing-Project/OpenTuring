/*
 * Limits module - limitations on the OOT types
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Limits
    export DefaultFW, DefaultEW, Radix, NumDigits, MinExp, MaxExp,
	GetExp, SetExp, Rreb, ~.*minint, ~.*maxint, ~.*minnat, ~.*maxnat

    % Default widths for printing using the "put" statement

    const DefaultFW := 6        % Default fraction width
    const DefaultEW := 2        % Default exponent width

    % Range of supported int values
    %    Note that the value -2 ** 31 = -2147483648 is the "uninitialized" int

    const minint := -2147483647
    const maxint := 2147483647          %  2**31 - 1


    % Range of supported nat values

    const minnat := 0
    const maxnat := 4294967294


    % The representation of a "real" number as floating point is:
    %   f * (radix ** e)    or     0
    % where, for non-zero f:
    %   (1/radix) <= abs(f)  and  abs(f) < 1.0
    %   minexp <= e  and  e <= maxexp

    % Numdigits is the number of radix digits in f.

    const Radix := 2
    const NumDigits := 53
    const MinExp := -1021
    const MaxExp := +1023

    % Functions to access and modify exponents

    external "limits_getexp" function GetExp (a : real)
    e : int
    % post (a=0 & e=0) or (a~=0 & e=e0)
    %   where
    %      a = f0 * (radix ** e0)
    %   and
    %      (1/radix) <= abs(f0) < 1
    %
    % assert getexp(0) = 0

    external "limits_setexp" function SetExp (a : real, e : int)
    r : real
    % post (a ~= 0 -> r = f0 * (radix ** e)) and (a = 0 -> r = 0)
    %   where
    %      a = f0 * (radix ** e0)
    %   and
    %      (1/radix) <= abs(f0) < 1

    % The relative roundoff error bound

    external "limits_rreb" function Rreb : real
    % const rreb := setexp(1.0, 1 - numdigits)

    % Conformance to recommendations of the Turing Language Report
    %       The following assertions demonstrate conformance:

    % assert minint <= -(2**30 - 1 + 2**30)      % -(2**31 -1)
    % assert maxstr >= 255
    % assert minexp <= -38 * ln(10) / ln(radix)
    % assert maxexp >= +38 * ln(10) / ln(radix)
    % assert rreb   <= 1e-14
end Limits
