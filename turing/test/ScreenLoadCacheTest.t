var sTime := Time.Elapsed
for i : 1..10
    Pic.ScreenLoad("phot3.jpg",0,0,picCopy)
end for
put "with cache: ",Time.Elapsed-sTime
