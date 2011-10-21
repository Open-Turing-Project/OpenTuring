/*
 ** Str module - string routines
 **
 ** NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 **      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Str
    export ~.*index, ~.*length, ~.*repeat, Lower, Upper, Trim, Right, Left,
	StartsWith, EndsWith, Pad, Centre, GetToken, GetTokenComma,
	GetTokenWithSep, ReadLineFromCharArray, WriteLineToCharArray,
	WriteToCharArray, CharArrayCopy

    %
    % Find the position of the given pattern in the given string.
    % The result is zero if the pattern does not exist in the string.
    %
    external "string_index" function index (s, patt : string) : int

    %
    % Determine the length of the given string.
    %
    external "string_length" function length (s : string) : int

    %
    % Return the given number of copies of the given string concatenated together.
    %
    external "string_repeat" function repeat (s : string, i : int) : string

    %
    % Return a string with all upper case letters made lower case
    %
    function Lower (s : string) : string
	var res : string := ""
	for i : 1 .. length (s)
	    if "A" <= s (i) and s (i) <= "Z" then
		res := res + chr (ord (s (i)) + ord ("a") - ord ("A"))
	    else
		res := res + s (i)
	    end if
	end for
	result res
    end Lower

    %
    % Return a string with all lower case letters made upper case
    %
    function Upper (s : string) : string
	var res : string := ""
	for i : 1 .. length (s)
	    if "a" <= s (i) and s (i) <= "z" then
		res := res + chr (ord (s (i)) + ord ("A") - ord ("a"))
	    else
		res := res + s (i)
	    end if
	end for
	result res
    end Upper

    %
    % Return a string with all the space before and after removed
    %
    function Trim (s : string) : string
	var len : int := length (s)
	var start : int := 1
	var finish : int := len
	loop
	    exit when start > len
	    exit when s (start) not= " " and s (start) not= "\t" and
		s (start) not= "\n"
	    start += 1
	end loop
	if start > len then
	    result ""
	end if
	loop
	    exit when finish < 1
	    exit when s (finish) not= " " and s (finish) not= "\t" and
		s (finish) not= "\n"
	    finish -= 1
	end loop
	result s (start .. finish)
    end Trim

    %
    % Return the right-most len characters
    %
    function Right (s : string, len : int) : string
	if length (s) < len then
	    result s
	end if
	result s (* - (len - 1) .. *)
    end Right

    %
    % Return the left-most len characters
    %
    function Left (s : string, len : int) : string
	if length (s) < len then
	    result s
	end if
	result s (1 .. len)
    end Left

    %
    % Return true if str starts with searchStr
    %
    function StartsWith (str, searchStr : string) : boolean
	const len : int := length (str)
	const searchLen : int := length (searchStr)

	if searchLen <= len and str (1 .. searchLen) = searchStr then
	    result true
	end if
	result false
    end StartsWith

    %
    % Return true if str ends with searchStr
    %
    function EndsWith (str, searchStr : string) : boolean
	const len : int := length (str)
	const searchLen : int := length (searchStr)

	if searchLen <= len and str (len - searchLen + 1 .. *) = searchStr then
	    result true
	end if
	result false
    end EndsWith

    %
    % Return s padded to length len with spaces
    %
    function Pad (s : string, len : int) : string
	result s + repeat (" ", len - length (s))
    end Pad

    %
    % Return s centred in a string of length len in spaces
    %
    function Centre (s : string, len : int) : string
	var startSpaces := (len - length (s)) div 2
	result repeat (" ", startSpaces) + s +
	    repeat (" ", len - length (s) - startSpaces)
    end Centre

    %
    % Return s centred in a string of length len in spaces
    %
    function Center (s : string, len : int) : string
	result Centre (s, len)
    end Center

    %
    % Return the next whitespace delimited token, while removing it
    % from the line
    %
    function GetToken (var line : string) : string
	var pos, pos1, pos2 : int
	var res : string

	if line (1) = " " or line (1) = "\t" then
	    line := Str.Trim (line)
	end if
	pos1 := index (line, " ")
	pos2 := index (line, "\t")
	if pos1 = 0 and pos2 = 0 then
	    res := line
	    line := ""
	else
	    if pos1 = 0 then
		pos := pos2
	    elsif pos2 = 0 then
		pos := pos1
	    else
		pos := min (pos1, pos2)
	    end if
	    res := line (1 .. pos - 1)
	    line := Str.Trim (line (pos + 1 .. *))
	end if
	result res
    end GetToken

    %
    % Return the next comma delimited token, while removing it
    % from the line
    %
    function GetTokenComma (var line : string) : string
	var pos : int
	var res : string

	pos := index (line, ",")
	if pos = 0 then
	    res := Trim (line)
	    line := ""
	else
	    res := Trim (line (1 .. pos - 1))
	    line := line (pos + 1 .. *)
	end if
	result res
    end GetTokenComma

    %
    % Return the next comma delimited token, while removing it
    % from the line
    %
    function GetTokenWithSep (var line : string, sep : string) : string
	var pos : int := maxint
	var res : string

	if sep = "" then
	    % Abort error message here
	    result ""
	elsif length (sep) > 8 then
	    % Abort error message here
	    result ""
	end if

	for i : 1 .. length (sep)
	    pos := min (pos, index (line, sep (i)))
	end for
	if pos = 0 then
	    res := Trim (line)
	    line := ""
	else
	    res := Trim (line (1 .. pos - 1))
	    line := line (pos + 1 .. *)
	end if
	result res
    end GetTokenWithSep

    %
    % Read a line (ending with '\n' or '\r\n') from an array of chars starting
    % at position arrayPos (which is updated)
    %
    external "string_readlinefromchararray"
	procedure ReadLineFromCharArray (chars : char (*), arraySize : int,
	var line : string, var arrayPos : int)

    %
    % Write a line (ending with '\r\n') to an array of chars starting
    % at position arrayPos (which is updated)
    %
    external "string_writelinetochararray"
	procedure WriteLineToCharArray (chars : char (*), line : string,
	var arrayPos : int)

    %
    % Write a string (no newline added) to an array of chars starting
    % at position arrayPos (which is updated)
    %
    external "string_writetochararray"
	procedure WriteToCharArray (chars : char (*), line : string,
	var arrayPos : int)

    %
    % Copy from one char array to another
    %
    external "string_chararraycopy"
	procedure CharArrayCopy (srcChars : char (*), srcArrayPos : int,
	destChars : char (*), destArrayPos : int, numCharsToCopy : int)
end Str

