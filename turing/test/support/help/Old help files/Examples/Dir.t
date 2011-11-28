% The "Dir" program.
setscreen ("text")

% Get the current directory
put "Current directory: ", Dir.Current

% Change to a new directory
put "We attempt to change to directory 'xyzzy' " ..
Dir.Change ("xyzzy")
if Error.Last = 0 then
    put "and succeed"
else
    put "but we get the following error: ", Error.LastMsg
end if

% Create the directory
put "Now we create the directory 'xyzzy' " ..
Dir.Create ("xyzzy")
if Error.Last = 0 then
    put "and succeed"
else
    put "but we get the following error: ", Error.LastMsg
end if

% Change to the new directory
put "We once again attempt to change to directory 'xyzzy' " ..
Dir.Change ("xyzzy")
if Error.Last = 0 then
    put "and succeed"
else
    put "but we get the following error: ", Error.LastMsg
end if

% Get the current directory
put "New Current directory: ", Dir.Current

% Create a file in the new directory
put "We now create a file in the current directory"
var f : int
open : f, "junk.dat", put
put : f, "This is a test"
close : f
put "File created"

% Change to the parent directory
put "We move to the parent directory " ..
Dir.Change ("..")
if Error.Last = 0 then
    put "and succeed"
else
    put "but we get the following error: ", Error.LastMsg
end if

% Delete the directory (which should failed because there's a file in it).
put "We try  to delete the directory 'xyzzy' " ..
Dir.Delete ("xyzzy")
if Error.Last = 0 then
    put "and succeed"
else
    put "but we get the following error: ", Error.LastMsg
end if

% Get the current directory
put "New Current directory: ", Dir.Current

% Delete the file in the directory
put "We try to delete the file 'xyzzy/junk.dat' " ..
File.Delete ("xyzzy/junk.dat")
if Error.Last = 0 then
    put "and succeed"
else
    put "but we get the following error: ", Error.LastMsg
end if

% Delete the directory (which should failed because there's a file in it).
put "We once again try to delete the directory 'xyzzy' " ..
Dir.Delete ("xyzzy")
if Error.Last = 0 then
    put "and succeed"
else
    put "but we get the following error: ", Error.LastMsg
end if
