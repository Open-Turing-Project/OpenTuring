% The "AlarmSystem" program.
% Alarm system is activated.
parallelput (1)
put "The system is now armed."
% Continuously checks for interruptions.
loop
    % If the beam is complete, then I0 is low because
    % current is flowing through the transistor.
    % Determine if I0 is low from the value returned by parallelget.
    if (parallelget div 64) mod 2 = 0 then
	% Continuously displays that the path is complete.
	put "The system is secure."
    else
	% I0 is high indicating that the beam is broken.
	put "The beam has been broken."
	% Leave the loop when the path is broken.
	exit
    end if
end loop
% When the light path is broken, the alarm will sound.
put "Activating alarm."
parallelput (2)
