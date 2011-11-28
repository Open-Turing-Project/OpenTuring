% The "Pic.DrawSpecialBack" program.
% Create the pictures
var redID, blueID : int
Draw.FillStar (0, 0, 100, 100, brightred)
redID := Pic.New (0, 0, 100, 100)
cls
Draw.FillOval (50, 50, 50, 50, brightblue)
blueID := Pic.New (0, 0, 100, 100)
cls
% Draw the pictures in pairs.
Pic.DrawSpecialBack (redID, 10, 10, picCopy, picWipeLeftToRightNoBar, 1000)
Pic.DrawSpecial (blueID, 160, 10, picCopy, picWipeRightToLeftNoBar, 1000)
Pic.DrawSpecialBack (blueID, 10, 10, picCopy, picSlideLeftToRightNoBar, 1000)
Pic.DrawSpecial (redID, 160, 10, picCopy, picSlideRightToLeftNoBar, 1000)
Pic.DrawSpecialBack (redID, 10, 10, picCopy, picGrowRightToLeftNoBar, 1000)
Pic.DrawSpecial (blueID, 160, 10, picCopy, picGrowLeftToRightNoBar, 1000)
Pic.DrawSpecialBack (blueID, 10, 10, picCopy, picWipeTopToBottomNoBar, 1000)
Pic.DrawSpecial (redID, 160, 10, picCopy, picWipeTopToBottomNoBar, 1000)
Pic.DrawSpecialBack (redID, 10, 10, picCopy, picFadeIn, 1000)
Pic.DrawSpecial (blueID, 160, 10, picCopy, picFadeIn, 1000)
Pic.Free (redID)
Pic.Free (blueID)
