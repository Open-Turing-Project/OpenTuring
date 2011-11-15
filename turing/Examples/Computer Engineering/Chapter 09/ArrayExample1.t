% The "ArrayExample1" program.
var number : array 1 .. 5 of int
% This variable declaration informs the computer that the variable
% number will store five separate numbers.
for counter : 1 .. 5
    put "Enter number ", counter, ": " ..
    get number (counter)
    % The variable number, since it has a bracket after it and was declared
    % as an array, will store the first number entered under number(1), the
    % second under number(2) through to number(5), the last number entered.
end for
% The five numbers stored above will now be output.
put "The five numbers in order of entry are"
for counter : 1 .. 5
    put number (counter)
end for
