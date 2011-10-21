% The "ShowMovie" program.
if not Sys.Exec ("skate.avi") then
    % The call failed!
    put "Sys.Exec failed with message: ", Error.LastMsg
else
    put "Sys.Exec successful.  Movie should now be showing..."
end if
