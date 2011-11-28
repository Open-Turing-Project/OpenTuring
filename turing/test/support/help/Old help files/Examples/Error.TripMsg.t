% The "Error.TripMsg" program.
Error.TripMsg (eFsysFileNotFound, "File *really* not found")
put "Error code = ", Error.Last
put "Error message = ", Error.LastMsg
