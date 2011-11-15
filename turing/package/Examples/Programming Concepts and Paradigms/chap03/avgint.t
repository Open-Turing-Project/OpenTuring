% File "avgint.t".
% Input a sequence of integers terminated by -1
% from a file and find their average.
var fileName : string
var number : int
var sum, count := 0
put "What is the name of the file of integers? "
get fileName
var streamIn : int
open : streamIn, fileName, get
assert streamIn > 0 % Checks to see that the file exists.
loop
    get : streamIn, number
    exit when number = - 1
    count := count + 1
    sum := sum + number
end loop
put "Average of the ", count, " integers is ", sum / count : 7 : 2
