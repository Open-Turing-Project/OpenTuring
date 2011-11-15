%3D parser/renderer demo
%Written by Tristan Hume
%Comments:
% I changed a few lines from my fully in-turing 3D renderer
% so that it used the GL library. It cut out hundreds of lines of code
% and made the program run much faster.

import GUI
include "parser.t"

% look in the "files" folder for more 3D files to try
% some examples "files/sphere.raw" "files/ship.raw" "files/gourd.obj"
const fname := "files/flower.obj"
var parser : ^Parser3D
if index(fname,".obj") not =  0 then
    new ObjParser,parser
elsif index(fname,".raw") not = 0 then
    new RawParser,parser
else
    quit
end if
parser -> construct(fname)

parser -> parseFile()
% GUI ---------------
setscreen("graphics:400;150,offscreenonly,nobuttonbar")
var quitBtn : int := GUI.CreateButton (10, 10, 0, "Quit", GUI.Quit)
var fileMenu := GUI.CreateMenu ("File")
var fileItems : array 1..2 of int
fileItems(1) := GUI.CreateMenuItem ("Quit", GUI.Quit)
fileItems(2) := GUI.CreateMenuItem ("Load", GUI.Quit)
GUI.ShowMenuBar 

var chars : array char of boolean

var drawbounds : boolean := false

var lastFrame : int := Time.Elapsed
GL.Scale(0.01,0.01,0.01)
loop
    put "CONTROL WINDOW"
    put "focus this window to control the program"
    put "arrow keys to rotate, A and S to zoom"
    put "ms per frame: " ..
    var ft : int := Time.Elapsed - lastFrame % frame time
    put ft
    lastFrame := Time.Elapsed
    var stop : boolean := false

    Input.KeyDown (chars)
    if chars (KEY_LEFT_ARROW) xor chars (KEY_RIGHT_ARROW) then
	if chars (KEY_LEFT_ARROW) then
	    GL.Rotate(6,0,1,0)
	else % right arrow key
	    GL.Rotate(6,0,-1,0)
	end if
    end if
    
    if chars (KEY_UP_ARROW) xor chars (KEY_DOWN_ARROW) then
	if chars (KEY_UP_ARROW) then
	    GL.Rotate(6,1,0,0)
	else
	    GL.Rotate(6,-1,0,0)
	end if        
    end if
    
    if chars ('a') xor chars ('s') then
	if chars ('a') then
	    GL.Scale(0.95,0.95,0.95)
	else
	    GL.Scale(1/0.95,1/0.95,1/0.95)
	end if
    end if

    parser -> DrawLines()
    GUI.Refresh
    View.Update
    exit when GUI.ProcessEvent
    %delay (2)
    if chars ('q') then
	stop := true
    end if
    exit when stop
    cls
end loop
