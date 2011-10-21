const font := Font.New ("serif:50:bold,italic")
const fileName := "globe.gif"
const picWidth := 160
const picHeight := 120

var fontHeight, fontAscent, fontDescent, dummy : int
var blank, turing41, featuringsprites, featuringgifs, includingmulti : int
var special, wipes, slides, fadeIns, allthis, slowmachine, nographics : int
var demoprog, linesof, beingrun : int
var numFrames, delayTime : int
var photo1, photo2, photo3, photo4, p1, p2, p3, p4 : int
var x1, y1, x2, y2 : int

function taketext (s : string) : int
    var w := Font.Width (s, font)
    cls
    Font.Draw (s, (maxx - w) div 2, maxy - fontAscent, font, black)
    result Pic.New (0, maxy - fontHeight, maxx, maxy)
end taketext

procedure centre (s : string, var y : int)
    var w := Font.Width (s, font)
    Font.Draw (s, (maxx - w) div 2, y, font, black)
    y -= fontHeight
end centre

proc sprites
    Pic.DrawSpecial (blank, 0, 0, picCopy, picFadeIn, 1000)
    Pic.DrawSpecial (turing41, 0, maxy - fontHeight, picCopy,
	picSlideTopToBottomNoBar, 1000)
    Pic.DrawSpecial (featuringsprites, 0, maxy - 2 * fontHeight, picCopy,
	picSlideLeftToRightNoBar, 1000)
end sprites

proc gifs (pics : array 1 .. * of int)
    Pic.DrawSpecial (blank, 0, 0, picCopy, picFadeIn, 1000)
    Pic.DrawSpecial (turing41, 0, maxy - fontHeight, picCopy,
	picSlideTopToBottomNoBar, 1000)
    Pic.DrawSpecial (featuringgifs, 0, maxy - 2 * fontHeight, picCopy,
	picSlideRightToLeftNoBar, 1000)
    delay (2000)
    Pic.DrawSpecial (includingmulti, 0, maxy - 2 * fontHeight, picCopy,
	picSlideBottomToTopNoBar, 1000)
    for i : 1 .. 4
	Pic.Draw (pics (i * 3), i * 150 - 100, 140, picCopy)
	delay (300)
    end for
    for i : 1 .. 4
	Draw.FillBox (0, 0, maxx, 120, 10 + i)
	Pic.DrawFrames (pics, (maxx - 100) div 2, 10, picMerge, upper (pics), 50, true)
    end for
end gifs


proc specialS
    Pic.DrawSpecial (blank, 0, 0, picCopy, picFadeIn, 1000)
    Pic.DrawSpecial (turing41, 0, maxy - fontHeight, picCopy,
	picSlideTopToBottomNoBar, 1000)
    Pic.DrawSpecial (special, 0, maxy - 2 * fontHeight, picCopy,
	picWipeLeftToRightNoBar, 1000)
	
    delay (500)
    Pic.DrawSpecial (wipes, 0, maxy - 3 * fontHeight, picCopy,
	picWipeBottomToTopNoBar, 1000)
    Draw.Box (x1 - 1, y1 - 1, x2 + 1, y2 + 1, black)
    Pic.DrawSpecial (p1, x1, y1, picCopy, picWipeBottomToTop, 1500)
    Pic.DrawSpecial (p2, x1, y1, picCopy, picWipeLeftToRight, 1500) 
    Pic.DrawSpecial (p3, x1, y1, picCopy, picWipeTopToBottom, 1500) 
    Pic.DrawSpecial (p4, x1, y1, picCopy, picWipeRightToLeft, 1500)
     
    delay (1000)
    Pic.DrawSpecial (slides, 0, maxy - 3 * fontHeight, picCopy,
	picSlideTopToBottomNoBar, 1000)
    Pic.DrawSpecial (p1, x1, y1, picCopy, picSlideBottomToTop, 1500)
    Pic.DrawSpecial (p2, x1, y1, picCopy, picSlideLeftToRight, 1500) 
    Pic.DrawSpecial (p3, x1, y1, picCopy, picSlideTopToBottom, 1500) 
    Pic.DrawSpecial (p4, x1, y1, picCopy, picSlideRightToLeft, 1500)
     
    delay (1000)
    Pic.DrawSpecial (fadeIns, 0, maxy - 3 * fontHeight, picCopy,
	picFadeIn, 1000)
    Pic.DrawSpecial (p1, x1, y1, picCopy, picFadeIn, 1500)
    Pic.DrawSpecial (p2, x1, y1, picCopy, picFadeIn, 1500) 
    Pic.DrawSpecial (p3, x1, y1, picCopy, picFadeIn, 1500)
    Pic.DrawSpecial (p4, x1, y1, picCopy, picFadeIn, 1500) 
    delay (1000)
end specialS

proc slow
    Pic.DrawSpecial (blank, 0, 0, picCopy, picFadeIn, 1000)
    Pic.DrawSpecial (demoprog, 0, maxy - fontHeight, picCopy,
	picSlideTopToBottomNoBar, 1000)
    delay (500)
    Pic.DrawSpecial (linesof, 0, maxy - 2 * fontHeight, picCopy,
	picWipeLeftToRightNoBar, 1000)
    delay (500)
    Pic.DrawSpecial (beingrun, 0, maxy - 3 * fontHeight, picCopy,
	picWipeLeftToRightNoBar, 1000)
    delay (500)
    Pic.DrawSpecial (slowmachine, 0, maxy - 4 * fontHeight, picCopy,
	picFadeIn, 1000)
    delay (500)
    Pic.DrawSpecial (nographics, 0, maxy - 5 * fontHeight, picCopy,
	picSlideRightToLeftNoBar, 1000)
end slow

Font.Sizes (font, fontHeight, fontAscent, fontDescent, dummy)
fontHeight += 3
blank := Pic.New (0, 0, maxx, maxy)
var y := maxy - 60
turing41 := taketext ("Turing 4.1")
featuringsprites := taketext ("Featuring Sprites")
featuringgifs := taketext ("Featuring GIF files")
includingmulti := taketext ("Multi-Frame GIFS!")
special := taketext ("Special Effects")
wipes := taketext ("Including Wipes")
slides := taketext ("Including Slides")
fadeIns := taketext ("Including Fade Ins")
demoprog := taketext ("This demo takes")
linesof := taketext ("160 lines of Turing")
beingrun := taketext ("and is being run")
slowmachine := taketext ("on a 600 MHz Celeron")
nographics := taketext ("with no graphics card")
% Load the GIF
numFrames := Pic.Frames (fileName)
var pics : array 1 .. numFrames of int
Pic.FileNewFrames (fileName, pics, delayTime)

photo1 := Pic.FileNew ("photo1.jpg")
p1 := Pic.Scale (photo1, picWidth, picHeight)
Pic.Free (photo1)
photo2 := Pic.FileNew ("photo2.jpg")
p2 := Pic.Scale (photo2, picWidth, picHeight)
Pic.Free (photo2)
photo3 := Pic.FileNew ("photo3.jpg")
p3 := Pic.Scale (photo3, picWidth, picHeight)
Pic.Free (photo3)
photo4 := Pic.FileNew ("photo4.jpg")
p4 := Pic.Scale (photo4, picWidth, picHeight)
Pic.Free (photo4)
x1 := (maxx - picWidth) div 2
y1 := 10
x2 := x1 + picWidth - 1
y2 := y1 + picHeight - 1

cls
loop
    sprites
    delay (2000)
    gifs (pics)
    delay (2000)
    specialS
    delay (2000)
    slow
    delay (2000)
end loop
