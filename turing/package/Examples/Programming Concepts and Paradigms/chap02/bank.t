% File "bank.t".
% This program computes the interest on a bank balance.
var interestRate : real := 3.5
var bankBalance := 215.12
% Output new bank balance at end of year.
var interest := interestRate * bankBalance /100
bankBalance := bankBalance + interest
put "New balance ", bankBalance : 10 : 2
% The new balance is rounded off to the nearest cent on output
% but the values stored in the variables interest or bankBalance
% are not rounded off.

