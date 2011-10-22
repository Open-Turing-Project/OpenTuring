const its := 2

var sTime := Time.Elapsed
for i : 1..its
    Pic.ScreenLoad("phot3.jpg",0,0,picCopy)
end for
put "with cache: ",Time.Elapsed-sTime

sTime := Time.Elapsed
for i : 1..its
    Pic.ScreenLoadNoCache("phot3.jpg",0,0,picCopy)
end for
put "no cache: ",Time.Elapsed-sTime
