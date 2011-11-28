% The "ScrollPic" program.
import GUI

var h, v : int              % The scroll bars.
var canvas : int            % The canvas.
var pic : int               % The picture.
const width : int := 220    % The width of the canvas.

procedure ScrollPic (ignore : int)
    % Get the current value of the scroll bars.
    var x : int := GUI.GetSliderValue (h)
    var y : int := GUI.GetSliderValue (v)
    GUI.PicDraw (canvas, pic, -x, -y, picCopy)
end ScrollPic

pic := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/Forest.jpg")
if pic <= 0 then
    put "Error loading picture: ", Error.LastMsg
    return
end if

View.Set ("graphics:265;265")

canvas := GUI.CreateCanvas (15, 15 + GUI.GetScrollBarWidth,
    width, width)
% Note the frame of the canvas is:
%   (14, 14 + ScrollbarWidth) - (235, 235 + ScrollbarWidth)
h := GUI.CreateHorizontalScrollBarFull (14, 14,
    221, 0, Pic.Width (pic), 0, ScrollPic, 3, 100, width)
v := GUI.CreateVerticalScrollBarFull (235,
    14 + GUI.GetScrollBarWidth, 221, 0, Pic.Height (pic),
    Pic.Height (pic), ScrollPic, 3, 100, width)
ScrollPic (0)         % Draw the picture initially

loop
    exit when GUI.ProcessEvent
end loop
