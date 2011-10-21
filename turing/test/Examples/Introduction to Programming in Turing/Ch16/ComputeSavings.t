% The "ComputeSavings" program
% Computes bank interest to nearest cent
var balance, interestRate, interest : real

function roundCent (amount : real) : real
    % Round amount to nearest cent
    result round (amount * 100) / 100
end roundCent

procedure banker (var balance, interest : real, interestRate : real)
    interest := roundCent (balance * interestRate / 100)
    balance := balance + interest
end banker

put "Enter balance " ..
get balance
put "Enter current interest rate " ..
get interestRate
banker (balance, interest, interestRate)
put "New balance = ", balance
put "Interest = ", interest
