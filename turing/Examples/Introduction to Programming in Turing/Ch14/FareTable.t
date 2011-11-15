% The "FareTable" program
% Reads and stores an airfare table
put "Enter number of cities in table: " ..
var cityCount : int
get cityCount
var airfare : array 1 .. cityCount, 1 .. cityCount of int
% Read table into two-dimensional array
for i : 1 .. cityCount
    put "Enter next row of table"
    for j : 1 .. cityCount
	get airfare (i, j)
    end for
end for
% Output two-dimensional array to disk
% Assign a stream number to fare
var fare : int
open : fare, "Fare", put
put : fare, cityCount
for i : 1 .. cityCount
    for j : 1 .. cityCount
	put : fare, airfare (i, j) : 8 ..
    end for
    put : fare, ""
end for
