% The "Drag" program.
setscreen ("graphics")

procedure DrawTheBox (x, y : int)
    drawfillbox (x, y, x + 40, y + 40, 12)
    drawfilloval (x + 20, y + 20, 20, 20, 4)
    drawfillarc (x + 20, y + 20, 20, 20, 135, 225, 14)
    drawfillarc (x + 20, y + 20, 20, 20, 315, 45, 15)
end DrawTheBox

var xBox : int := 0
var yBox : int := 0
var x, y, diffX, diffY, btn, updown : int
var buf : array 1 .. sizepic (0, 0, 40, 40) of int

takepic (xBox, yBox, xBox + 40, yBox + 40, buf)

DrawTheBox (xBox, yBox)

loop
    if buttonmoved ("down") then
	buttonwait ("down", x, y, btn, updown)
	if xBox <= x and x <= xBox + 40 and yBox <= y and y <= yBox + 40 then
	    % The mouse has been clicked in the box
	    diffX := x - xBox
	    diffY := y - yBox
	    loop
		mousewhere (x, y, updown)
		exit when updown = 0
		if xBox not= x - diffX or yBox not= y - diffY then
		    drawpic (max (xBox, 0), max (yBox, 0), buf, 0)
		    xBox := x - diffX
		    yBox := y - diffY
		    takepic (max (0, xBox), max (0, yBox),
			min (xBox + 40, maxx), min (yBox + 40, maxy), buf)
		    DrawTheBox (xBox, yBox)
		end if
	    end loop
	end if
    end if
end loop
