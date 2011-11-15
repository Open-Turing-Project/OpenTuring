% The "Fibonacci" program
% This outputs a fibonacci number
var number : int

put "Enter the element in the Fibonacci sequence to calculate: " ..
get number

if number <= 0 then
    put "The number should be positive"
else
    var element1 : int := 1
    var element2 : int := 1
    for i : 3 .. number
	var temp := element1 + element2
	element1 := element2
	element2 := temp
    end for
    put "Element number ", number, " is ", element2
end if
