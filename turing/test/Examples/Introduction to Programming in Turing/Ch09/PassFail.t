% The "PassFail" program
% Read a mark and decide whether it is a pass or failure
put "Enter a mark " ..
var mark : int
get mark
if mark >= 50 then
    put mark, " is a pass"
else
    put mark, " is a failure"
end if
