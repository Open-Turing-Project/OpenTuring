% The "Music.PlayFileLoop" program.

% Play this sound file, looping continuously.
Music.PlayFileLoop ("%oot/support/help/Examples/Data Files/music.wav")

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

% If the "Music.PlayFileStop" statement is missing, the user will 
% have to manually halt the program.
Music.PlayFileStop
