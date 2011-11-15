% The "ShowUserName" program.
var userName := Sys.GetUserName
if Error.Last not= 0 then
    put "Sys.GetUserName failed: ", Error.LastMsg
else
    put "User Name: ", userName
end if

var computerName := Sys.GetComputerName
if Error.Last not= 0 then
    put "Sys.GetComputerName failed: ", Error.LastMsg
else
    put "Computer Name: ", computerName
end if
