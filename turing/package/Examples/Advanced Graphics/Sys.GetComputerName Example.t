% The "Sys.GetComputerName Example" program.
var computerName, userName : string
computerName := Sys.GetComputerName
userName := Sys.GetUserName
put "Hello ", userName, " on ", computerName
