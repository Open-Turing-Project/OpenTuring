% The "fetcharg" program.
% Run this program with "Run with Args" and enter some command line arguments.
put "The name of this program is : ", fetcharg (0)
for i : 1 .. nargs
    put "Argument ", i, " is ", fetcharg (i)
end for
