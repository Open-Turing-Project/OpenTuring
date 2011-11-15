% The "LightActivation" program.
var value, i0, i1 : int
% Infinite loop.
loop
    value := parallelget
    % i0 = 1 if I0 pin is high, i0 = 0 is I0 pin is low.
    i0 := (value div 64) mod 2
    % i1 = 1 if I1 pin is high, i1 = 0 is I1 pin is low.
    i1 := (value div 128) mod 2
    % If neither or both receivers get light the motor is stopped.
    if (i0 = 0 and i1 = 0) or (i0 = 1 and i1 = 1) then
	parallelput (0)
    elsif i0 = 0 then
	% I0 pin is low (i.e. light is shining in IR receiver 1).
	% Counts the four steps of the motor.
	for counter : 0 .. 3
	    % Counts through the four steps.
	    parallelput (2 ** counter)
	    % Waits one tenth of a second.
	    delay (100)
	end for
	% If IR receiver 2 is closed the motor is rotated in
	% the other direction.
    elsif i1 = 0 then
	% I1 pin is low (i.e. light is shining in IR receiver 2).
	% Counts in the opposite direction.
	for decreasing counter : 3 .. 0
	    % Counts through the four steps.
	    parallelput (2 ** counter)
	    % Waits one tenth of a second.
	    delay (100)
	end for
    end if
    % Endlessly repeats until program is stopped.
end loop
