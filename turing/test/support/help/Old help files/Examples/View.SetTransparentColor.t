% The "View.SetTransparentColor" program.
const size : int := 20
proc DrawCheckerBoard
    cls
    for x : 0 .. maxx by size
	for y : 0 .. maxy by size
	    if ((x + y) div size) mod 2 = 0 then
		Draw.FillBox (x, y, x + size, y + size, brightgreen)
	    end if
	end for
    end for
end DrawCheckerBoard

var pic : int := Pic.FileNew ("data files/airplane.bmp")
Pic.SetTransparentColor (pic, brightred)
RGB.SetColor (0, 1., 0., 0.)                % Set the background color to red
DrawCheckerBoard                            % Draw a checkboard
Pic.Draw (pic, 100, 100, picUnderMerge)     % Draw left plane on top of red
					    % and behind green
View.SetTransparentColor (brightgreen)      % Set the transparent color to
					    % green instead of red
Pic.Draw (pic, 400, 100, picUnderMerge)     % Draw left plane on top of green
					    % and behind red
