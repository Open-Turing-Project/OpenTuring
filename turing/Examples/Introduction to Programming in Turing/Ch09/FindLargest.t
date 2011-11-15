% The "FindLargest" program
% Read 10 positive integers and find the largest
put "Enter 10 positive integers"
var number : int
var largest := - 1
for i : 1 .. 10
    get number
    assert number >= 0
    if number > largest then
	largest := number
    end if
end for
put "Largest integer of ten is ", largest
