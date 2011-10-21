% The "IntruderDetector" program.
% The emitter connected to D0 is turned on.
parallelput (1)
loop
    % If the beam is complete, then I0 is low because
    % current is flowing through the transistor.
    % Determine if I0 is low from the value returned by parallelget.
    if (parallelget div 64) mod 2 = 0 then
	put "The light beam is complete."
    else
	put "The light beam is broken."
    end if
end loop
