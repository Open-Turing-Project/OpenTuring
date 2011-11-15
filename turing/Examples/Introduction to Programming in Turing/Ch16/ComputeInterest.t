% The "ComputeInterest" program
% Computes bank interest to nearest cent

function roundCent (amount : real) : real
    % Round amount to nearest cent
    result round (amount * 100) / 100
end roundCent

var balance, interestRate : real
put "Enter balance: " ..
get balance
put "Enter current interest rate in percent: " ..
get interestRate
const interest := roundCent (balance * interestRate / 100)
balance := balance + interest
put "New balance = ", balance
put "Interest = ", interest
