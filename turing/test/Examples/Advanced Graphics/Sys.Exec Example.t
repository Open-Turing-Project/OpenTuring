% The "Sys.Exec Example" program.
if not Sys.Exec ("http://www.holtsoft.com/turing/support") then
    put "The Sys.Exec call failed"
    put "Error: ", Error.LastMsg
end if
if not Sys.Exec ("skate.avi") then
    put "The Sys.Exec call failed"
    put "Error: ", Error.LastMsg
end if
