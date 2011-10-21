var f := Font.New ("serif:60:bold,italic,noantialias")
Font.Draw ("Turing", 170, 100, f, black)
var p3 := Pic.New (90, 70, 600, 160)
Draw.FillBox (80, 200, 631, 331, brightred)
var p4 := Pic.New (100, 220, 610, 310)
var p5 := Pic.Blend (p3, p4, 50)
Draw.Box (99, 219, 611, 311, black)
Pic.DrawSpecial (p3, 100, 220, picCopy, picBlend + 30, 1000)
Draw.FillBox (80, 80, 631, 211, brightred)
Draw.Box (99, 99, 611, 191, black)
Draw.Box (100, 100, 610, 190, brightgreen)
var t := Time.Elapsed
Pic.DrawSpecial (p3, 100, 100, picMerge, picFadeIn, 10000)
%put  Time.Elapsed - t
