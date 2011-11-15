% The "DecisionExample2" program.
var number1, number2, total : real
% Prompt for first number.
put "Please enter the first number: " ..
% The user enters any number.
get number1
% Prompt for second number.
put "Please enter the second number: " ..
% The user enters the second number.
get number2
% The user is prompted for the sum.
put "Enter the sum of ", number1, " and ", number2, ": " ..
% The user's guess for the sum is entered.
get total
if total = number1 + number2 then
    % This section is executed when the guess is correct.
    put "Congratulations."
else
    % This section is executed when the guess is incorrect.
    put "Check your answer."
end if
% The end if always goes at the end of the decision structure.
