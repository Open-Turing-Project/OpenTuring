% The "Grade" program
% Read mark and find corresponding letter grade
put "Enter marks, end with 999"
var mark : int
loop
    put "Enter mark: " ..
    get mark
    exit when mark = 999
    if mark >= 0 and mark <= 100 then
	if mark >= 80 then
	    put "A"
	elsif mark >= 70 then
	    put "B"
	elsif mark >= 60 then
	    put "C"
	elsif mark >= 50 then
	    put "D"
	else
	    put "Fail"
	end if
    else
	put "Improper mark"
    end if
end loop
