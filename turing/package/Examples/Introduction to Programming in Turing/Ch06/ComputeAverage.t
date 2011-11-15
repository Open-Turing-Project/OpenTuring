% The "ComputeAverage" program
% Compute the average of a series of marks
% Give average to the nearest integer
put "Enter a series of marks"
const sentinel := - 1
put "End with ", sentinel
var mark : int
var count, sum : int := 0
loop
    get mark
    exit when mark = sentinel
    count := count + 1
    sum := sum + mark
end loop
put "Average mark is ", round (sum / count)
