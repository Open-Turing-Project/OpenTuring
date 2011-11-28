% The "Input.KeyDown" program.
var chars : array char of boolean
put "Press the arrow keys.  Note you can press multiple arrow keys at once."
loop
    Input.KeyDown (chars)
    locate (4, 1)
    if chars (KEY_UP_ARROW) then
	put "Up Arrow Pressed  " ..
    else
	put "                  " ..
    end if
    if chars (KEY_RIGHT_ARROW) then
	put "Right Arrow Pressed  " ..
    else
	put "                     " ..
    end if
    if chars (KEY_LEFT_ARROW) then
	put "Left Arrow Pressed  " ..
    else
	put "                    " ..
    end if
    if chars (KEY_DOWN_ARROW) then
	put "Down Arrow Pressed  " ..
    else
	put "                  " ..
    end if
end loop
