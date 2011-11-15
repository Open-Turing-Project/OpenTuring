% File "sum100.t".
% A program to sum the first 100 integers using a conditional loop.
var sum : int := 0
var number : int := 1
loop
    sum := sum + number
    exit when number = 100
    number := number + 1
end loop
put "Sum of first 100 integers is ", sum
