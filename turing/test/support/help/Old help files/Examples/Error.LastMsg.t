% The "Error.LastMsg" program.
Dir.Create ("testdata/information")
if Error.Last = eNoError then
    put "Directory created"
else
    put "Did not create the directory."
    put "Error Number: ", Error.Last
    put "Error Message: ", Error.LastMsg
end if
