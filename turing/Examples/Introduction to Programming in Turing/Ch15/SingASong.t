% The "SingASong" program
% Plays songs for a sing-a-long
% Read song from disk
var numLines : int
get numLines
var lineSyll : array 1 .. numLines, 1 .. 7 of string (7)
var lineNote : array 1 .. numLines, 1 .. 7 of string (7)
var numberOfSyll : array 1 .. numLines of int
for line : 1 .. numLines
    % Read number of syllables in line
    get numberOfSyll (line)
    % Read syllables in line
    for syllNo : 1 .. numberOfSyll (line)
	get lineSyll (line, syllNo)
    end for
    % Read notes in line
    for noteNo : 1 .. numberOfSyll (line)
	get lineNote (line, noteNo)
    end for
end for

cls
for line : 1 .. numLines
    % Display words of line on window row 10
    locate (10, 1)
    for syllNo : 1 .. numberOfSyll (line)
	put lineSyll (line, syllNo) : 7 ..
    end for
    % Plot ball over syllable
    for syllNo : 1 .. numberOfSyll (line)
	% Display ball over syllable in window row 9
	locate (9, 1 + (syllNo - 1) * 7)
	put "*" ..
	% Play corresponding note
	play (lineNote (line, syllNo))
	% When playdone is true, erase ball
	loop
	    exit when playdone
	end loop
	locate (9, 1 + (syllNo - 1) * 7)
	put " " ..
    end for
    % Erase words of line
    locate (10, 1)
    put " "
end for
