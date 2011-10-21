% File "avgten.t".
% Compute the average of 10 marks.
var mark : int
var sum := 0
put "Enter 10 marks one after the other"
for count : 1 .. 10
    get mark
    sum := sum + mark
end for
put "Average of 10 marks is ", sum / 10 : 5 : 1
