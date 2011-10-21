% The "ComputeInterest2" program
% Computes bank interest to nearest cent

function roundCent (amount : real) : real
    % Round amount to nearest cent
    result round (amount * 100) / 100
end roundCent

function calcInterest (amount, interestRate : real) : real
    result roundCent (amount * interestRate / 100)
end calcInterest

var balance, interestRate : real
put "Enter balance: " ..
get balance
put "Enter current interest rate in percent: " ..
get interestRate
const interest := calcInterest (balance, interestRate)
balance := balance + interest
put "New balance = ", balance
put "Interest = ", interest
