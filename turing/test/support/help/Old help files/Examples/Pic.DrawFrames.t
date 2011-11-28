% The "Pic.DrawFrames" program.
% Determine the number of frames in "globe.gif"
const fileName := "%oot/Support/Help/Examples/Data Files/globe.gif"
var numFrames : int := Pic.Frames (fileName)
% Load the picture
var delayTime : int
var pics : array 1 .. numFrames of int
Pic.FileNewFrames (fileName, pics, delayTime)
% Set the window to fit the picture
var w := Pic.Width (pics (1)) + 20
var h := Pic.Height (pics (1)) + 20
View.Set ("graphics:" + intstr (w) + ";" + intstr (h) + ",nobuttonbar")
% Display the pictures
for i : 1 .. 50
    Pic.DrawFrames (pics, 10, 10, picCopy, upper (pics), 50, false)
end for
