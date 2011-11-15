% The "ComputeInputAverage" program
% Computes the average of a series of grades
% stored in a disk file
var grade : int
var count, sum : int := 0
loop
    % Skip over any white space to next grade or to eof
    get skip
    exit when eof
    get grade
    sum := sum + grade
    count := count + 1
end loop
put "Average mark is ", round (sum / count)
