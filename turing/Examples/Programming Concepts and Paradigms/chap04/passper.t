% File "passper.t".
% Compute the percentage of pass marks in a sequence of exam marks.
var mark : int
const sentinel := - 1
put "Enter marks of exams, end with ", sentinel
var countOfPasses, countOfMarks : int := 0
loop
    get mark
    exit when mark = sentinel
    countOfMarks += 1
    if mark >= 50 then
        countOfPasses += 1
    end if
end loop
put "Percentage passed is ", (countOfPasses / countOfMarks) * 100 : 6 : 1, 
    "%"
