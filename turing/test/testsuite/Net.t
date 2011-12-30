var url : string := "http://tristan.hume.ca/"

var netStream : int
var line : string

netStream := Net.OpenURLConnection (url)
if netStream <= 0 then
    put "Unable to connect to ", url
    return
end if
loop
    exit when eof (netStream)
    get : netStream, line
    put line
end loop
Net.CloseConnection (netStream)
