% "The Sprite.Animate" program.
var numFrames : int := Pic.Frames ("%oot/Support/Help/Examples/Data Files/bones.gif")
var pics : array 1 .. numFrames of int
var delayTime : int

% Load the picture
Pic.FileNewFrames ("%oot/Support/Help/Examples/Data Files/bones.gif", pics, delayTime)

% Draw the background black
Draw.FillBox (0, 0, maxx, maxy, black)

% Create the sprite
var sprite : int
sprite := Sprite.New (pics (1))
Sprite.SetPosition (sprite, 0, 100, false)
Sprite.Show (sprite)

% Move the sprite across the screen
for x : 8 .. maxx by 8
    Sprite.Animate (sprite, pics ((x div 8) mod numFrames + 1), x, 100, false)
    delay (40)
end for
Sprite.Free (sprite)
