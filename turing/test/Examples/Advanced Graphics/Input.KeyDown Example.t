% The "Input.KeyDown Example" program.

var chars : array char of boolean
loop
    Input.KeyDown (chars)
    locate (1, 1)
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
	put "Down Arrow Pressed" ..
    else
	put "                  " ..
    end if
end loop
