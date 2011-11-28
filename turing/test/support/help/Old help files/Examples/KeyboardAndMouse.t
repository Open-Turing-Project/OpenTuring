% The "KeyboardAndMouse" program.
var ch : string (1)
var x, y, btnnum, btnupdown : int
put "Press a key or click a mouse button"
loop
    if hasch then
	getch (ch)
	locate (3, 1)
	put "The character entered is a: ", ch
    end if
    if buttonmoved ("down") then
	buttonwait ("down", x, y, btnnum, btnupdown)
	locate (3, 1)
	put "The button was clicked at position: ", x, ", ", y
    end if
end loop
