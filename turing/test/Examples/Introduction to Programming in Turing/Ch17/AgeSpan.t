% The "AgeSpan" program
% Computes the difference between the largest
% and smallest element in an array of ages
type ageRange : 1 .. 120
put "Enter number of elements in array: " ..
var number : int
get number
var age : array 1 .. number of ageRange
% Read in array with number elements
put "Enter ", number, " ages"
for i : 1 .. number
    get age (i)
end for

procedure minmax (list : array 1 .. * of ageRange,
	var minimum, maximum : ageRange, number : int)
    minimum := list (1)
    maximum := list (1)
    for i : 2 .. number
	if list (i) > maximum then
	    maximum := list (i)
	elsif list (i) < minimum then
	    minimum := list (i)
	end if
    end for
end minmax

var largest, smallest : ageRange
minmax (age, smallest, largest, number)
put "Age span in group is ", largest - smallest
