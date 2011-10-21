var numFrames : int := Pic.Frames ("bones.gif")
var pics : array 1 .. numFrames of int
var delayTime : int
Pic.FileNewFrames ("bones.gif", pics, delayTime)
Draw.FillBox (0, 0, maxx, maxy, black)
var sprite : int
sprite := Sprite.New (pics (1))
Sprite.SetPosition (sprite, 0, 100, false)
Sprite.Show (sprite)
for x : 8 .. maxx by 8
    Sprite.Animate (sprite, pics ((x div 8) mod numFrames + 1), x, 100, false)
    delay (40)
end for
Sprite.Free (sprite)
