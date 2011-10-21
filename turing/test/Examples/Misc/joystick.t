% The "JoyStick" program.
var jx, jy, x, y, ox, oy : int := - 20
var b1, b2, ob1, ob2 : boolean := false
loop
    Joystick.GetInfo (joyStick1, jx, jy, b1, b2)
    x := round (maxx * (jx - joyMin) / (joyMax - joyMin))
    y := round (maxy * (jy - joyMax) / (joyMax - joyMin))
    if x not= ox or y not= oy or b1 not= ob1 or b2 not= ob2 then
        Text.Locate (1, 1)
        put x : 4, " ", y : 4, " ", b1, " ", b2
        Draw.Line (ox - 10, oy, ox + 10, oy, colorbg)
        Draw.Line (ox, oy - 10, ox, oy + 10, colorbg)
        Draw.Line (x - 10, y, x + 10, y, red)
        Draw.Line (x, y - 10, x, y + 10, red)
        ox := x
        oy := y
        ob1 := b1
        ob2 := b2
    end if
end loop
