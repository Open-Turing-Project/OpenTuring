/*
 * Time module - time routines
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Time
    export Sec, Date, SecDate, DateSec, SecParts, PartsSec,
	Elapsed, ElapsedCPU, Delay, SecStr, DelaySinceLast

    % return turingTime (the number of seconds since Jan 1, 1970  00:00:00 GMT)
    external "time_sec" function Sec : int

    % return a date/time string in format "dd mmm yy hh:mm:ss"
    external "time_date" function Date : string

    % convert an ootTime into a date/time string
    external "time_secdate" function SecDate (turingTime : int) : string

    % convert a date and/or time string into ootTime
    external "time_datesec" function DateSec (dateTimeStr : string) : int

    % convert ootTime into components
    external "time_secparts"
	procedure SecParts (turingTime : int, var year, month, day : int,
	var dayofweek, hour, minute, second : int)

    % convert components into ootTime
    external "time_partssec"
	function PartsSec (year, month, day, hour, minute, second : int) : int

    % return elapsed time program has been running in milliseconds
    external "time_elapsed" function Elapsed : int

    % return elapsed CPU time used by program in milliseconds
    external "time_elapsedcpu" function ElapsedCPU : int

    % delay execution for a number of milliseconds
    external "time_delay" procedure Delay (duration : int)

    % Use ANSI strftime conversion
    external function time_secstr (turingTime : int,
	formatStr : string) : string

    function SecStr (turingTime : int, formatStr : string) : string
	var newFormatStr : string := ""
	type letterSet : set of char
	var legalFormatLetters : letterSet :=
	    letterSet ('a', 'A', 'b', 'B', 'c', 'd', 'H', 'I', 'j', 'm', 'M',
	    'p', 'S', 'U', 'w', 'W', 'x', 'X', 'y', 'Y', 'z', 'Z')
	for i : 1 .. length (formatStr)
	    var ch : char := formatStr (i)
	    if ch = ' ' or ch = ',' then
		newFormatStr += ch
	    elsif ch in legalFormatLetters then
		newFormatStr += "%" + ch
	    else
		Error.Halt ("Illegal character '" + ch + "' in format string")
	    end if
	end for
	result time_secstr (turingTime, newFormatStr)
    end SecStr

    var timeOfLastCall := 0
    procedure DelaySinceLast (delayTime : int)
	if Time.Elapsed < timeOfLastCall + delayTime then
	    Time.Delay (timeOfLastCall + delayTime - Time.Elapsed)
	end if
	timeOfLastCall := Time.Elapsed
    end DelaySinceLast
end Time
