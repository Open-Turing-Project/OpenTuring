% The "LoopExample2" program.
for counter : 1 .. 20
    % The for starts a repetition structure to count.
    % The counter will start with a value of one.
    % Each time the for loop is executed, counter is increased by one.
    % The counter will be two on the second time through the loop.
    put counter : 6, counter ** 2 : 7, counter ** 3 : 8
    % The put outputs the numbers to the screen.
    % The counter is the variable doing the counting.
    %  The :6 allows six spaces for the output of the value of counter.
    % The **2 squares the variable counter.
    % The :7 allows seven spaces for the squared value.
    % The **3 cubes the variable counter.
end for
% end for always ends the counted loop structure.
% The for loop will repeat until counter is 30.
