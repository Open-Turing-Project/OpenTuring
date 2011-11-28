% The "Pic.DrawSpecial" program.
var f := Font.New ("serif:60:bold,italic")
Font.Draw ("Welcome", 100, 100, f, black)
var p1 := Pic.New (90, 70, 600, 160)
cls
Font.Draw ("to New", 170, 100, f, black)
var p2 := Pic.New (90, 70, 600, 160)
cls
Font.Draw ("Turing", 170, 100, f, black)
var p3 := Pic.New (90, 70, 600, 160)
cls
Font.Draw ("Special Effects", 100, 100, f, black)
var p4 := Pic.New (90, 70, 600, 160)
cls
Draw.Box (99, 99, 611, 191, brightred)
locate (13, 35)
put "picWipeLeftToRight"
Pic.DrawSpecial (p1, 100, 100, picCopy, picWipeLeftToRight, 2000)
locate (13, 35)
put "picWipeRightToLeft"
Pic.DrawSpecial (p2, 100, 100, picCopy, picWipeRightToLeft, 2000)
locate (13, 35)
put "picWipeTopToBottom"
Pic.DrawSpecial (p3, 100, 100, picCopy, picWipeTopToBottom, 2000)
locate (13, 35)
put "picWipeBottomToTop"
Pic.DrawSpecial (p4, 100, 100, picCopy, picWipeBottomToTop, 2000)
locate (13, 35)
put "picWipeLeftToRightNoBar"
Pic.DrawSpecial (p1, 100, 100, picCopy, picWipeLeftToRightNoBar, 2000)
locate (13, 35)
put "picWipeRightToLeftNoBar"
Pic.DrawSpecial (p2, 100, 100, picCopy, picWipeRightToLeftNoBar, 2000)
locate (13, 35)
put "picWipeTopToBottomNoBar"
Pic.DrawSpecial (p3, 100, 100, picCopy, picWipeTopToBottomNoBar, 2000)
locate (13, 35)
put "picWipeBottomToTopNoBar"
Pic.DrawSpecial (p4, 100, 100, picCopy, picWipeBottomToTopNoBar, 2000)
locate (13, 35)
put "picSlideLeftToRight"
Pic.DrawSpecial (p1, 100, 100, picCopy, picSlideLeftToRight, 2000)
locate (13, 35)
put "picSlideRightToLeft"
Pic.DrawSpecial (p2, 100, 100, picCopy, picSlideRightToLeft, 2000)
locate (13, 35)
put "picSlideTopToBottom"
Pic.DrawSpecial (p3, 100, 100, picCopy, picSlideTopToBottom, 2000)
locate (13, 35)
put "picSlideBottomToTop"
Pic.DrawSpecial (p4, 100, 100, picCopy, picSlideBottomToTop, 2000)
locate (13, 35)
put "picSlideLeftToRightNoBar"
Pic.DrawSpecial (p1, 100, 100, picCopy, picSlideLeftToRightNoBar, 2000)
locate (13, 35)
put "picSlideRightToLeftNoBar"
Pic.DrawSpecial (p2, 100, 100, picCopy, picSlideRightToLeftNoBar, 2000)
locate (13, 35)
put "picSlideTopToBottomNoBar"
Pic.DrawSpecial (p3, 100, 100, picCopy, picSlideTopToBottomNoBar, 2000)
locate (13, 35)
put "picSlideBottomToTopNoBar"
Pic.DrawSpecial (p4, 100, 100, picCopy, picSlideBottomToTopNoBar, 2000)
locate (13, 35)
put "picFadeIn"
Pic.DrawSpecial (p1, 100, 100, picCopy, picFadeIn, 1000)
Pic.DrawSpecial (p2, 100, 100, picCopy, picFadeIn, 1000)
Pic.DrawSpecial (p3, 100, 100, picCopy, picFadeIn, 1000)
Pic.DrawSpecial (p4, 100, 100, picCopy, picFadeIn, 1000)
