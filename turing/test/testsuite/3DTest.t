external "glgraph_newwin" proc NewWin (width, height, mode : int)
external "glgraph_closewin" proc CloseWin ()
external "glgraph_cls" proc SDLCls ()
external "glgraph_update" proc Update ()
external "glgraph_setlight" proc SetLight (enabled : int)

external "glgraph_pushmatrix" proc PushMatrix ()
external "glgraph_popmatrix" proc PopMatrix ()
external "glgraph_clearmatrix" proc ClearMatrix ()

external "glgraph_scale" proc Scale (x,y,z : real)
external "glgraph_translate" proc Translate (x,y,z : real)
external "glgraph_rotate" proc Rotate (angle,x,y,z : real)

external "glgraph_drawline" proc DrawLine (x1, y1, z1, x2, y2, z2 : real, r, g, b : int)
external "glgraph_drawfilltriangle" proc DrawFillTriangle (x1, y1, z1, x2, y2, z2, x3, y3, z3:real, r, g, b : int)
external "glgraph_drawtriangle" proc DrawTriangle (x1, y1, z1, x2, y2, z2, x3, y3, z3:real, r, g, b : int)

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
