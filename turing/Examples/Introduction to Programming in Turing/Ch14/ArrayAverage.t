% The "ArrayAverage" program
% Reads a list of marks and computes their average
var count : int
put "How many students in class? " ..
get count
var mark : array 1 .. count of int
var sum := 0
put "Enter the marks for the students"
for i : 1 .. count
    get mark (i)
end for
for i : 1 .. count
    sum := sum + mark (i)
end for
const average := sum / count
% Output the average to two decimal places
put "Average mark is ", average : 6 : 2
