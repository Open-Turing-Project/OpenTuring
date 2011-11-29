/*
 * Rand module - pseudo-random number routines
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Rand
    export Int, Next, Real, Reset, Seed, Set

    %
    % Generate a pseudo-random number from zero to one.
    %
    function Real : real
	external procedure rand_real (var r : real)
	var r : real

	rand_real (r)
	result r
    end Real

    %
    % Generate a pseudo-random integer in the given range.
    %
    function Int (low, high : int) : int
	external procedure rand_int (var i : int, low, high : int)
	var i : int

	rand_int (i, low, high)
	result i
    end Int

    %
    % Generate a pseudo-random number from zero to one in the given sequence.
    %
    function Next (seq : 1 .. 10) : real
	external procedure rand_next (var r : real, seq : 1 .. 10)
	var r : real

	rand_next (r, seq)
	result r
    end Next

    %
    % Reset the pseudo-random number sequences to a pseudo-random state.
    %
    external "rand_randomize" procedure Randomize

    %
    % Reset the specified pseudo-random number sequences to the given state.
    %
    external "rand_seed" procedure Seed (seed : int, seq : 1 .. 10)

    procedure Set (seed : int)
	external procedure rand_seed (seed : int, seq : 0 .. 10)

	rand_seed (seed, 0)
    end Set

    var reset : boolean := false
    procedure Reset
	if not reset then
	    Set (16#57657475)
	    reset := true
	end if
    end Reset

    Randomize
end Rand
