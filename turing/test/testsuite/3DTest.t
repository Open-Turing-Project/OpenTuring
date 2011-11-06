const width := 640
const height := 480

var sTime : int

var res := GL.NewWin (width, height)

GL.SetLight (1)

loop
    GL.ClearMatrix ()
    for i : 0 .. 360
	GL.Cls ()
	GL.Rotate (1, 1, 0, 0) % rotate 1 degree per iteration around x axis
	GL.DrawTriangle (-1, -0.1, 0, -1, 0.1, 0, (i / 180) - 1, 1, 0, 100, 100, 50)
	GL.Update ()
    end for
    for i : 0 .. 360
	GL.Cls ()
	GL.Rotate (1, 0, 1, 0)
	GL.DrawFillTriangle (-1, -0.1, 0, -1, 0.1, 0, 0.2, 1, 0, 100, 100, 50)
	GL.Update ()
    end for
    for i : 0 .. 360
	GL.Cls ()
	GL.Rotate (2, 1, 0, 0)
	GL.DrawLine (-1, 0, 0, (i / 180) - 1, 1, 0, 100, 100, 50)
	GL.Update ()
    end for
end loop

GL.CloseWin ()
