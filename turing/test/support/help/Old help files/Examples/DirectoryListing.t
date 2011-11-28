% The "DirectoryListing" program.
setscreen ("text")
var streamNumber : int
var fileName : string
streamNumber := Dir.Open (".")
assert streamNumber > 0
loop
    fileName := Dir.Get (streamNumber)
    exit when fileName = ""
    put fileName
end loop
Dir.Close (streamNumber)
