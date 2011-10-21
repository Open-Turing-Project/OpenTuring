/*
** Input module
**
** NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
**      IT WILL BE IMPLICITLY IMPORTED.
*/

unit
module Input
    export ~.*getch, ~.*getchar, ~.*hasch, Pause, KeyDown, Flush

%
% Get a single character without waiting for a newline.
%
    external "input_getch" procedure getch (var ch : string(1))

%
% A function returning char equivalent of getch
%
    function getchar: char
	var ch : string(1)
	getch (ch)
	result ch
    end getchar

%
% Determine whether a character has been typed at the keyboard
% but not yet read.
%
    external "input_hasch" function hasch : boolean

    procedure Pause
	var ch : string(1)
	getch (ch)
    end Pause

    external "input_keydown" 
	procedure KeyDown (var keys : array char of boolean)
	
    % Remove all keyboard input from the buffer	
    procedure Flush
	var ch : string(1)
        loop
            exit when not hasch
	    getch (ch)
	end loop
    end Flush            	
end Input
