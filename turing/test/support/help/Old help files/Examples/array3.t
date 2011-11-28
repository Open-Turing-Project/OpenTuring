% The "array3" program.
function total (a : array 1 .. * of real) : real
    var sum : real := 0
    for i : 1 .. upper (a)
	sum := sum + a (i)
    end for
    result sum
end total

var howMany : int
put "How many heights? " ..
get howMany
var height : array 1 .. howMany of real
% Read in all the elements of this array
for i : 1 .. howMany
    put "Enter height #", i, ": " ..
    get height (i)
end for

% Output the sum of the heights
put "Sum of the heights is ", total (height)
