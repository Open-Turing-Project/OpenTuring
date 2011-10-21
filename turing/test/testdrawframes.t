% Testing reading of GIFs

var numFrames1, delayTime1 : int
var numFrames2, delayTime2 : int
var numFrames3, delayTime3 : int
var fileName1 := "2_computers.gif"
var fileName2 := "globe.gif" 
var fileName3 := "police.gif"

numFrames1 := Pic.Frames (fileName1)
put "Error: ", Error.LastMsg
put "There are ", numFrames1, " frames in ", fileName1
numFrames2 := Pic.Frames (fileName2)
put "Error: ", Error.LastMsg
put "There are ", numFrames2, " frames in ", fileName2
numFrames3 := Pic.Frames (fileName3)
put "Error: ", Error.LastMsg
put "There are ", numFrames3, " frames in ", fileName3

var pics1 : array 1 .. numFrames1 of int
Pic.FileNewFrames (fileName1, pics1, delayTime1)
put "Error: ", Error.LastMsg
put "Delay time = ", delayTime1
var pics2 : array 1 .. numFrames2 of int
Pic.FileNewFrames (fileName2, pics2, delayTime2)
put "Error: ", Error.LastMsg
put "Delay time = ", delayTime2
var pics3 : array 1 .. numFrames3 of int
Pic.FileNewFrames (fileName3, pics3, delayTime3)
put "Error: ", Error.LastMsg
put "Delay time = ", delayTime3

var p1 := Pic.FileNew (fileName1)
Pic.Draw (p1, 250, 220, picMerge)
var p2 := Pic.FileNew (fileName2)
Pic.Draw (p2, 250, 100, picMerge)
var p3 := Pic.FileNew (fileName3)
Pic.Draw (p3, 250, 20, picMerge)

Pic.DrawFramesBack (pics1, 10, 220, picMerge, numFrames1, delayTime1, true)
Pic.DrawFramesBack (pics2, 10, 100, picMerge, numFrames2, delayTime2, false)
Pic.DrawFramesBack (pics3, 10, 20, picMerge, numFrames3, delayTime3, true)
