var f := Font.New ("serif:60:bold,italic,noantialias")
Draw.FillBox (maxx div 2 - 2, maxy div 2 - 2,
    maxx div 2 + 2, maxy div 2 + 2, black) % Centre
Draw.FillBox (maxx div 4 - 2, maxy div 4 - 2,
    maxx div 4 + 2, maxy div 4 + 2, brightgreen) % Bottom-left
Draw.FillBox (maxx div 4 - 2, 3 * maxy div 4 - 2,
    maxx div 4 + 2, 3 * maxy div 4 + 2, brightblue)% 
Draw.FillBox (3 * maxx div 4 - 2, maxy div 4 - 2,
    3 * maxx div 4 + 2, maxy div 4 + 2, brightred)
Draw.FillBox (3 * maxx div 4 - 2, 3 * maxy div 4 - 2,
    3 * maxx div 4 + 2, 3 * maxy div 4 + 2, brightmagenta)
var p, p1 : int
p := Pic.New (0, 0, maxx, maxy)
loop
    Input.Pause
    p1 := Pic.Blur (p, 10)
    Pic.Draw (p1, 0, 0, picCopy)
    Pic.Free (p)
    p := p1
end loop
