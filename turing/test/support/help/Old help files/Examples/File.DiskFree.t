% The "File.DiskFree" program.
var bytesFree : int
bytesFree := File.DiskFree ("A:\\")
if bytesFree = -1 then
    put "Can't get free space on drive A:."
    put "Error: ", Error.LastMsg
else
    put "There are ", bytesFree, " bytes free on drive A:"
end if

bytesFree := File.DiskFree (".")
if bytesFree = -1 then
    put "Can't get free space on default directory."
    put "Error: ", Error.LastMsg
else
    put "There are ", bytesFree, " bytes free on the default directory"
end if
