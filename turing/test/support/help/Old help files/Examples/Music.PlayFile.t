% The "Music.PlayFile" program.
process DoMusic
    loop
	Music.PlayFile ("%oot/support/help/Examples/Data Files/music.wav")
    end loop
end DoMusic

fork DoMusic

var x, y, clr : int
loop
    x := Rand.Int (0, maxx)
    y := Rand.Int (0, maxy)
    clr := Rand.Int (0, maxcolor)
    Draw.FillOval (x, y, 30, 30, clr)
end loop
