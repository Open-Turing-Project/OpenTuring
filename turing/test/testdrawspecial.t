drawfillbox (0, 0, 100, 100, brightred)
drawbox (0, 0, 100, 100, brightblue)
var p := Pic.New (0, 0, 100, 100)
drawfillbox (0, 0, maxx, maxy, brightgreen)
drawbox (49, 49, 151, 151, black)
drawbox (49, 179, 151, 281, black)
drawbox (179, 49, 281, 151, black)
drawbox (179, 179, 281, 281, black)
drawbox (349, 49, 451, 151, black)
drawbox (349, 179, 451, 281, black)
drawbox (479, 49, 581, 151, black)
drawbox (479, 179, 581, 281, black)
%Pic.Draw (p, 310, 180, picCopy)
var t := Time.Elapsed
Pic.DrawSpecial (p, 50, 50, picCopy, picWipeLeftToRight, 1000)
put Time.Elapsed - t, " " ..
t := Time.Elapsed
Pic.DrawSpecial (p, 50, 180, picCopy, picWipeRightToLeft, 1000)
put Time.Elapsed - t, " " ..
t := Time.Elapsed
Pic.DrawSpecial (p, 180, 50, picCopy, picWipeTopToBottom, 1000)
put Time.Elapsed - t, " " ..
t := Time.Elapsed
Pic.DrawSpecial (p, 180, 180, picCopy, picWipeBottomToTop, 1000)
put Time.Elapsed - t, " " ..
t := Time.Elapsed
Pic.DrawSpecial (p, 350, 50, picCopy, picWipeLeftToRightNoBar, 1000)
put Time.Elapsed - t, " " ..
t := Time.Elapsed
Pic.DrawSpecial (p, 350, 180, picCopy, picWipeRightToLeftNoBar, 1000)
put Time.Elapsed - t, " " ..
t := Time.Elapsed
Pic.DrawSpecial (p, 480, 50, picCopy, picWipeTopToBottomNoBar, 1000)
put Time.Elapsed - t, " " ..
t := Time.Elapsed
Pic.DrawSpecial (p, 480, 180, picCopy, picWipeBottomToTopNoBar, 1000)
put Time.Elapsed - t
