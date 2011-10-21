% The "TestSprite3" program.

Draw.FillBox (0, 0, 100, 100, brightblue)
Draw.FillOval (50, 50, 50, 50, brightred)
Draw.FillBox (200, 0, 250, 50, brightred)
Draw.FillOval (225, 25, 25, 25, brightblue)
var p1 := Pic.New (0, 0, 100, 100)
var p2 := Pic.New (200, 0, 250, 50)
var s := Sprite.New (p1)
cls
drawfillbox (0, 0, maxx, maxy, red)
Sprite.SetPosition (s, 50, 50, false)
Sprite.Show (s)
delay (1000)
Sprite.SetPosition (s, -50, 50, false)
delay (10000)
