external "sdlgraph_newwin" proc NewWin (width, height, mode : int)
external "sdlgraph_closewin" proc CloseWin ()
external "sdlgraph_cls" proc SDLCls ()
external "sdlgraph_update" proc Update ()
external "sdlgraph_setlight" proc SetLight (enabled : int)

external "sdlgraph_pushmatrix" proc PushMatrix ()
external "sdlgraph_popmatrix" proc PopMatrix ()
external "sdlgraph_clearmatrix" proc ClearMatrix ()

external "sdlgraph_scale" proc Scale (x,y,z : real)
external "sdlgraph_translate" proc Translate (x,y,z : real)
external "sdlgraph_rotate" proc Rotate (angle,x,y,z : real)

external "sdlgraph_drawline" proc DrawLine (x1, y1, z1, x2, y2, z2 : real, r, g, b : int)
external "sdlgraph_drawfilltriangle" proc DrawFillTriangle (x1, y1, z1, x2, y2, z2, x3, y3, z3:real, r, g, b : int)
external "sdlgraph_drawtriangle" proc DrawTriangle (x1, y1, z1, x2, y2, z2, x3, y3, z3:real, r, g, b : int)

const width := 640
const height := 480

var sTime : int

NewWin (width, height, 0)
%View.Set("graphics:640;480,offscreenonly")

SetLight(1)

loop
    ClearMatrix()
    for i : 0 .. 360
	SDLCls ()
	Rotate(1,1,0,0) % rotate 1 degree per iteration around x axis
	DrawTriangle (-1, -0.1, 0,-1,0.1,0, (i/180) - 1, 1, 0, 100, 100, 50)
	Update ()
    end for
    for i : 0 .. 360
	SDLCls ()
	Rotate(1,0,1,0)
	DrawFillTriangle (-1, -0.1, 0,-1,0.1,0, 0.2, 1, 0, 100, 100, 50)
	Update ()
    end for
    for i : 0 .. 360
	SDLCls ()
	Rotate(2,1,0,0)
	DrawLine (-1, 0, 0, (i/180) - 1, 1, 0, 100, 100, 50)
	Update ()
    end for
end loop

delay (10000)
CloseWin ()
