% The "LoopExample1" program.
for counter : 1 .. 20
    % The for is a reserved word to start the counting structure.
    % counter is any variable used to do the counting.
    % The : goes before the starting number.
    % The .. goes after the starting number and before the ending           
    % number. counter will equal one initially.
    % Each time through the for/end for will increase counter by one.
    put counter : 5
    % The put will output the numbers one at a time to the screen.
    % The :5 will allow five spaces for each number that is output.
end for
% All for repetition structures must end with an end for.
% The for/end for loop will repeat until counter passes 20.
% The first time the body of the loop is executed, counter has
% value of 1. The second time the body of the loop is executed, counter
% has a value of 2, and so on. The loop will continue to execute repeatedly
% 20 times, at which point end for ends the loop.
