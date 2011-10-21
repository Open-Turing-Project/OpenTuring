% File "squares.t".
% This program computes the sum of the
% squares of numbers from 1 to 100.
var sum := 0
for number : 1 .. 100
    sum := sum + number ** 2
end for
put "Sum of squares of numbers from 1 to 100 is ", sum
