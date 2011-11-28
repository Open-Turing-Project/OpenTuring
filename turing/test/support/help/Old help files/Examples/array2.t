% The "array2" program.
var price : array 1988 .. 1990, 1 .. 12 of int

% Create a random value for each year and month
for year : 1988 .. 1990             % For each year
    for month : 1 .. 12             % For each month
	price (year, month) := Rand.Int (5000, 10000)
    end for
end for

% Find the sum of all the prices
var sum : int := 0
for year : 1988 .. 1990             % For each year
    for month : 1 .. 12             % For each month
	sum := sum + price (year, month)
    end for
end for
put "The sum is ", sum

% Output the table with labels
put "    " ..
for month : 1 .. 12                 % For each month
    put month : 6 ..
end for
put ""
for year : 1988 .. 1990             % For each year
    put year, " " ..
    for month : 1 .. 12             % For each month
	put price (year, month) : 6 ..
    end for
    put ""
end for
