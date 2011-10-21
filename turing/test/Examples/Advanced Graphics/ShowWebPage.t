% The "ShowWebPage" program.
if not Sys.Exec ("http://www.holtsoft.com") then
    % The call failed!
    put "Sys.Exec failed with message: ", Error.LastMsg
else
    put "Sys.Exec successful."
    put "Browser should now come up with Holt Software's web page..."
end if
