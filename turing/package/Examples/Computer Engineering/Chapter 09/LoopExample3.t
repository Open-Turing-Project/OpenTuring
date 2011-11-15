% The "LoopExample3" program.
% This places the titles at the top of the columns.
put "  NUMBER  SQUARE CUBE"
for counter : 1 .. 20
    put counter : 6, counter ** 2 : 7, counter ** 3 : 8
end for
% The above three lines are the same as in Example 2.
