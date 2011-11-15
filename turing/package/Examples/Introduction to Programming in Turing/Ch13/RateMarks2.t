% The "RateMarks2" program
% Read in a series of marks, ending with -1
% Output comments about performance
var mark : int
put "Enter -1 to signal end of series of marks"
loop
    put "Enter mark: " ..
    get mark
    exit when mark = - 1
    if mark >= 80 then
	put "Excellent"
    elsif mark >= 70 then
	put "Good"
    elsif mark >= 50 then
	put "Satisfactory"
    else
	put "Poor"
    end if
end loop
