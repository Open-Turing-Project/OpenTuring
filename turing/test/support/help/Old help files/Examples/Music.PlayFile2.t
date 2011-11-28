% The "Music.PlayFile2" program.
const STAR_SIZE : int := 80
var pic, x, y, dx, dy : int
var finished : boolean := false

% Play sound effect once.
process Boing
    Music.PlayFile ("%oot/support/help/Examples/Data Files/boing.wav")
end Boing

% Loop playing background music until 'finished' is true.
process BackgroundMusic
    loop
	exit when finished
	Music.PlayFile ("%oot/support/help/Examples/Data Files/music.mid")
    end loop
end BackgroundMusic

% Get the original picture
Draw.FillStar (3, 3, STAR_SIZE - 3, STAR_SIZE - 3, brightred)
pic := Pic.New (0, 0, STAR_SIZE, STAR_SIZE)
cls

% Set the initial location and direction of movement
x := Rand.Int (0, maxx - STAR_SIZE)
y := Rand.Int (0, maxy - STAR_SIZE)
dx := 1
dy := 1

fork BackgroundMusic            % Start background music playing

loop
    if x + dx < 0 or x + dx > maxx - STAR_SIZE then
	dx := -dx
	fork Boing
    end if
    if y + dy < 0 or y + dy > maxy - STAR_SIZE then
	dy := -dy
	fork Boing
    end if
    x += dx
    y += dy
    Pic.Draw (pic, x, y, picCopy)
    exit when hasch
    Time.Delay (5)
end loop

% Stop the background music.
finished := true
Music.PlayFileStop
