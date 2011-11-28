% The "Music.Sound" program
var finished : boolean := false

process siren
    loop
	for i : 100 .. 3000 by 100
	    Music.Sound (i, 50)             % Sound note
	    exit when finished
	end for
	exit when finished
	for decreasing i : 2900 .. 200 by 100
	    Music.Sound (i, 50)             % Sound note
	    exit when finished
	end for
	exit when finished
    end loop
end siren

% Start the siren sound
fork siren

% Display some graphics for 10 seconds
loop
    var x, y, c : int
    x := Rand.Int (0, maxx)
    y := Rand.Int (0, maxy)
    c := Rand.Int (0, maxcolor)
    Draw.FillOval (x, y, 30, 30, c)
    exit when Time.Elapsed > 10000
end loop

% Set the finished flag to true, which will cause
% the siren process to exit.
finished := true
