% The "DecisionExample1" program.
var answer : string
% Always prompt the user for the entry.
put "Please enter in the capital of Ontario: " ..
% The user will input their guess into the variable answer.
get answer : *
if answer = "Toronto" then
    % When the condition is true, execution transfers here.
    put "Good"
else
    % When the answer to the condition is not true, the execution
    % transfers here.
    put "Try Again"
end if
