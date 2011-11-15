% File "passfail.t".
% Read a sequence of exam marks and indicate whether a
% mark is a pass or failure. The sequence of marks is terminated by -1.
var mark : int
const sentinel := - 1
put "Enter marks of exams, end with ", sentinel
loop
    get mark
    exit when mark = sentinel
    if mark >= 50 then
        put "The mark ", mark, " is a pass"
    else
        put "The mark ", mark, " is a failure"
    end if
end loop
