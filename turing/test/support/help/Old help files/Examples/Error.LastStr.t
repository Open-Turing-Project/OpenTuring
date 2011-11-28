% The "Error.LastStr" program.
Dir.Create ("testdata/information")
if Error.Last = eNoError then
    put "Directory created"
else
    put "Did not create the directory."
    put "Error Number: ", Error.Last        % last error number
    put "Error Constant: ", Error.LastStr   % last error constant as string
end if
