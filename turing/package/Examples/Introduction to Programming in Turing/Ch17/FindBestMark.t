% The "FindBestMark" program
% Finds the highest mark in a class of five

function maxArray (list : array 1 .. 5 of int) : int
    var biggest := list (1)
    for i : 2 .. 5
	if list (i) > biggest then
	    biggest := list (i)
	end if
    end for
    result biggest
end maxArray

put "Enter five marks"
var mark : array 1 .. 5 of int
for i : 1 .. 5
    get mark (i)
end for
put "The best mark is ", maxArray (mark)
