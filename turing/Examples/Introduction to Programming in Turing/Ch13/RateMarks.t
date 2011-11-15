% The "RateMarks" program
% Reads in a series of marks, ending with -1
% Outputs comments about performance
var mark : int
put "Enter -1 to signal end of series of marks"
loop
    put "Enter mark: " ..
    get mark
    exit when mark = - 1
    if mark >= 80 then
	put "Excellent"
    else
	if mark >= 70 then
	    put "Good"
	else
	    if mark >= 50 then
		put "Satisfactory"
	    else
		put "Poor"
	    end if
	end if
    end if
end loop
