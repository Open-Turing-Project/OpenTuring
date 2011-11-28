% The "MouseDrag" program.
var x1, y1, b1, b2, x2, y2, x, y : int
setscreen ("xor")
loop
    % Wait for a key to be pressed
    buttonwait ("down", x1, y1, b1, b2)
    x2 := x1
    y2 := y1
    drawbox (x1, y1, x2, y2, brightred)

    loop
	% Get the current mouse position
	mousewhere (x, y, b1)
	% If the mouse button has been released, exit the loop
	exit when b1 = 0
	% Has the mouse position has changed?
	if x not= x2 or y not= y2 then
	    % Draw the same box over the old box. In "xor" mode
	    % this erases the original box.
	    drawfillbox (x1, y1, x2, y2, brightred)
	    x2 := x
	    y2 := y
	    % Draw the box in its new location
	    drawfillbox (x1, y1, x2, y2, brightred)
	end if
    end loop
end loop
