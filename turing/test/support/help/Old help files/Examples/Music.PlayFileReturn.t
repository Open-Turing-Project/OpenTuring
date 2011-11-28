% The "Music.PlayFileReturn" program.

% Play this sound file once, returning immediately so that
% the ovals are drawn while the music is playing.
Music.PlayFileReturn ("%oot/support/help/Examples/Data Files/music.wav")

% Draw circles on the screen until the user presses a key.
var x, y, clr : int
put "Press any key to halt program (and music)."
loop
    x := Rand.Int (0, maxx)
    y := Rand.Int (0, maxy - 45)
    clr := Rand.Int (0, maxcolor)
    Draw.FillOval (x, y, 30, 30, clr)
    exit when hasch
end loop

% If the "Music.PlayFileStop" statement is missing, the program will
% end when the music file is finished.
Music.PlayFileStop
