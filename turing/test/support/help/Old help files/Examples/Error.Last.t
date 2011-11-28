% The "Error.Last" program.
var f : int
open : f, "testdata/information.txt", get
if f > 0 then
    put "File opened"
else
    put "Could not open file."
    put "Error Number: ", Error.Last        % Last error number
    put "Error Message: ", Error.LastMsg    % Last error message
    put "Error Constant: ", Error.LastStr   % last error constant as string
end if
