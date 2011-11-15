% File "avgmarks.t".
% Compute the average of a sequence of marks.
var howMany, mark, sum : int
sum := 0
put "How many marks are there? " ..
get howMany
put "Enter ", howMany, " marks one after the other"
for count : 1 .. howMany
    get mark % Read next mark.
    sum += mark % Process the mark.
end for
put "Average of ", howMany, " marks is ", sum / howMany : 5 : 1
