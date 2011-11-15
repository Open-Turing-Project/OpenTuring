% The "ComputeAverages" program
% Reads marks and computes average
var mark : int
var sum : int := 0
put "Enter marks"
for count : 1 .. 5
    put count
    get mark
    sum := sum + mark
end for
put "Average is ", round (sum / 5)
