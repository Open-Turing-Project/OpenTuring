% The "buttonchoose" program.
buttonchoose ("multibutton")
var x, y, button, left, middle, right : int
loop
    mousewhere (x, y, button)
    cls
    put "Press the mouse buttons"
    put "mouse button = ", button
    left := button mod 10               % left = 0 or 1
    middle := (button - left) mod 100   % middle = 0 or 10
    right := button - middle - left     % right = 0 or 100
    if left = 1 then
	put "left button down"
    end if
    if middle = 10 then
	put "middle button down"
    end if
    if right = 100 then
	put "right button down"
    end if
    delay (20)  % Without delay, the screen flickers.
end loop
