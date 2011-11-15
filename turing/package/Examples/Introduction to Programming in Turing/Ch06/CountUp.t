% The "CountUp" program
% Counts from 1 to 10 but has 25 percent
% chance of stopping
var number : real
for count : 1 .. 10
    rand (number)
    exit when number <= .25
    put count
end for
