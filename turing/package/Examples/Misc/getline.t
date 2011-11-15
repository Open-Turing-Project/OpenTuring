%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                               GetLine                                 %
%                                                                       %
% This functions get a line of input with a maximum specified length    %
% It will beep when                                                     %
%    (1) The user tries to put more data in than allowed                %
%    (2) The user tries to erase character when there are no characters %
%        to erase                                                       %
%    (3) The user enters in a "non-printable" character.  i.e. Any      %
%        graphics character or character that is not between a space    %
%        and a tilde ("~") in the ASCII sequence.                       %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function GetLine (maxLength : int) : string
    var str : string := ""      % The string you're building
    var ch : string (1)         % The character just read in

    setscreen ("noecho") % Turn off echoing

    loop
	getch (ch) % Get the character

	exit when ord (ch) = 10 % Exit the loop when you get a RETURN

	if ord (ch) = 8 then
	    % We just entered a backspace character.  We will first check to
	    % make certain that there are characters to erase.  If there
	    % aren't then we beep.  If there are, we put a "backspace"
	    % character which erases the previous character. Lastly,
	    % we cut the last character off the string

	    if str = "" then % Check to see if any chars to erase
		sound (700, 200)
	    else
		put "\b" .. % Erase the previous character

		% And now cut the last character off the string
		str := str (1 .. * -1)
	    end if
	else
	    % Check to see if the character is a "printable"
	    if (ch < " ") or ("~" < ch) then
		% The character is a graphics character, beep
		sound (600, 200)
	    else
		% The character entered is a "printable", so called
		% because it's a non-graphics character.  We now check
		% to see if there are already too many characters in
		% the string.  If there are, then we beep, otherwise
		% we add the character to the string

		if length (str) >= maxLength then
		    % The string is already as long as allowed, beep
		    sound (400, 200)
		else
		    put ch .. % Echo the character
		    str := str + ch % Add it to the string
		end if
	    end if
	end if
    end loop

    if whatrow = maxrow then
	locate (maxrow, maxcol)
	put ""
    else
	locate (whatrow + 1, 1)
    end if

    setscreen ("echo") % Return the screen to echo mode

    result str
end GetLine


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                               GetInt                                  %
%                                                                       %
% This functions get a positive integer with a maximum specified length %
% It will beep when                                                     %
%    (1) The user tries to put more data in than allowed                %
%    (2) The user tries to erase character when there are no characters %
%        to erase                                                       %
%    (3) The user enters in a non-numeric character.  i.e. Any          %
%        character that is not between "0" and "9"                      %
% The function returns -1 if the number is larger than the maximum size %
% size of a positive integer (2147483647).                              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function GetInt (maxLength : int) : int
    var str : string := "" % The string you're building
    var ch : string (1) % The character just read in

    setscreen ("noecho") % Turn off echoing

    loop
	getch (ch) % Get the character

	exit when ord (ch) = 10 % Exit the loop when you get a RETURN

	if ord (ch) = 8 then
	    % We just entered a backspace character.  We will first check to
	    % make certain that there are characters to erase.  If there
	    % aren't then we beep.  If there are, we put a "backspace"
	    % character which erases the previous character.  Lastly,
	    % we cut the last character off the string

	    if str = "" then % Check to see if any chars to erase
		sound (700, 200)
	    else
		put "\b" .. % Erase the previous character

		% And now cut the last character off the string
		str := str (1 .. * -1)
	    end if
	else
	    % Check to see if the character is a "printable"
	    if (ch < "0") or ("9" < ch) then
		% The character is a non-numeric character, beep
		sound (600, 200)
	    else
		% The character entered is a number.  We now check
		% to see if there are already too many characters in
		% the string.  If there are, then we beep, otherwise
		% we add the character to the string

		if length (str) >= maxLength then
		    % The string is already as long as allowed, beep
		    sound (400, 200)
		else
		    put ch .. % Echo the character
		    str := str + ch % Add it to the string
		end if
	    end if
	end if
    end loop

    if whatrow = maxrow then
	locate (maxrow, maxcol)
	put ""
    else
	locate (whatrow + 1, 1)
    end if

    setscreen ("echo") % Return the screen to echo mode

    % We check to make certain that the number is not larger than
    % the maximum positive integer
    if (length (str) > 10) or
	    ((length (str) = 10) and str > ("2147483647")) then
	result - 1
    else
	result strint (str)
    end if
end GetInt


% Some samples to show it in action
var name : string (30)
put "Name: " ..
name := GetLine (30)
put "Name = \"", name, "\""

var address : string (40)
put "Address: " ..
address := GetLine (40)
put "Address = \"", address, "\""

var phone : string (8)
put "Phone: " ..
phone := GetLine (8)
put "Phone = \"", phone, "\""

var num : int
put "Positive Number: " ..
num := GetInt (11)
put "Number = \"", num, "\""
