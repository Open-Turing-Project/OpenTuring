% The "MusicAndSound" program.
const STAR_SIZE : int := 80
const BOING_FILE : string := "%oot/support/help/Examples/Data Files/boing.wav"
const BACKGROUND : string := "%oot/support/help/Examples/Data Files/music.mid"
var pic, x, y, dx, dy : int
var finished : boolean := false

% Get the original picture
Draw.FillStar (3, 3, STAR_SIZE - 3, STAR_SIZE - 3, brightred)
pic := Pic.New (0, 0, STAR_SIZE, STAR_SIZE)
cls

% Set the initial location and direction of movement
x := Rand.Int (0, maxx - STAR_SIZE)
y := Rand.Int (0, maxy - STAR_SIZE)
dx := 2
dy := 2

Music.PlayFileLoop (BACKGROUND)         % Start background music playing

loop
    if x + dx < -3 or x + dx > maxx - STAR_SIZE + 3 then
	dx := -dx
	Music.PlayFileReturn (BOING_FILE)
    end if
    if y + dy < -3 or y + dy > maxy - STAR_SIZE + 3 then
	dy := -dy
	Music.PlayFileReturn (BOING_FILE)
    end if
    x += dx
    y += dy
    Pic.Draw (pic, x, y, picCopy)
    exit when hasch
    Time.DelaySinceLast (5)
end loop

% Stop the background music.
finished := true
Music.PlayFileStop
