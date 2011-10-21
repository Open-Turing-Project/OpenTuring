% The "StepperMotor" program.
% Repeats the sequence one hundred times.
for count : 1 .. 100
    % Counts 0, 1, 2, and 3 which activates D0, D1, D2,
    % and D3 successively.
    for counter : 0 .. 3
	% Outputs to the parallel port.
	parallelput (2 ** counter)
	% Delays for one-tenth of a second.
	delay (100)
    end for
end for

% Delays for two seconds.
delay (2000)

% Again, repeats pattern one hundred times.
for count : 1 .. 100
    % Counts 3, 2, 1, and 0 which activates D3, D2, D1,
    % and D0 successively.
    for decreasing counter : 3 .. 0
	% Outputs to the parallel port.
	parallelput (2 ** counter)
	% Delays for one-tenth of a second.
	delay (100)
    end for
end for
