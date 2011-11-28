% The "simutime" program.
process p (t : int)
    loop
	pause t
	put simutime, " " ..
	exit when simutime > 25
    end loop
end p

% One process is pausing for 3 simulation time units at a time,
% the other for 5 simulation time units at a time.
fork p (3)
fork p (5)
