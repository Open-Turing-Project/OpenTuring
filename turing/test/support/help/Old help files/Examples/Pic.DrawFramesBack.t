% The "Pic.DrawFramesBack" program.
% Determine the number of frames in "globe.gif"
const fileName := "%oot/Support/Help/Examples/Data Files/globe.gif"
var numFrames : int := Pic.Frames (fileName)
% Load the picture
var delayTime : int
var pics : array 1 .. numFrames of int
Pic.FileNewFrames (fileName, pics, delayTime)
% Set the window to fit the picture
var w := Pic.Width (pics (1)) * 3 + 40
var h := Pic.Height (pics (1)) + 20
View.Set ("graphics:" + intstr (w) + ";" + intstr (h) + ",nobuttonbar")
% Display the pictures
Draw.FillBox (0, 0, maxx, maxy, red)
for i : 1 .. 50
    Pic.DrawFramesBack (pics, 10, 10, picMerge, numFrames, 50, false)
    Pic.DrawFramesBack (pics, 20 + Pic.Width (pics (1)), 10, picMerge, numFrames, 50, false)
    Pic.DrawFrames (pics, 30 + 2 * Pic.Width (pics (1)), 10, picMerge, numFrames, 50, false)
end for
