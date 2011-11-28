% The "Pic.DrawSpecial2" program.

const pressMessage := "Press any other key to continue"
const startMessage := "+=faster  -=slower  space=pause  backspace=previous"
const repeatMessage := "Displaying same transition again"
var timeInMillis : int := 2000

% Create the four text images
var text : array 1 .. 4 of int
var photos : array 1 .. 4 of int
var f1, f2, f3 : int % Fonts
const seNames : array 1 .. * of string := init (
    "picWipeLeftToRight", "picWipeRightToLeft",
    "picWipeTopToBottom", "picWipeBottomToTop",
    "picWipeCentreToEdge",
    "picWipeUpperLeftToLowerRight", "picWipeLowerLeftToUpperRight",
    "picWipeUpperRightToLowerLeft", "picWipeLowerRightToUpperLeft",
    "picWipeLeftToRightNoBar", "picWipeRightToLeftNoBar",
    "picWipeTopToBottomNoBar", "picWipeBottomToTopNoBar",
    "picWipeCentreToEdgeNoBar",
    "picWipeUpperLeftToLowerRightNoBar", "picWipeLowerLeftToUpperRightNoBar",
    "picWipeUpperRightToLowerLeftNoBar", "picWipeLowerRightToUpperLeftNoBar",
    "picGrowLeftToRight", "picGrowRightToLeft",
    "picGrowTopToBottom", "picGrowBottomToTop",
    "picGrowCentreToEdge", "picGrowCenterToEdge",
    "picGrowUpperLeftToLowerRight", "picGrowLowerLeftToUpperRight",
    "picGrowUpperRightToLowerLeft", "picGrowLowerRightToUpperLeft",
    "picGrowLeftToRightNoBar", "picGrowRightToLeftNoBar",
    "picGrowTopToBottomNoBar", "picGrowBottomToTopNoBar",
    "picGrowCentreToEdgeNoBar", "picGrowCenterToEdgeNoBar",
    "picGrowUpperLeftToLowerRightNoBar", "picGrowLowerLeftToUpperRightNoBar",
    "picGrowUpperRightToLowerLeftNoBar", "picGrowLowerRightToUpperLeftNoBar",
    "picSlideLeftToRight", "picSlideRightToLeft",
    "picSlideTopToBottom", "picSlideBottomToTop",
    "picSlideLeftToRightNoBar", "picSlideRightToLeftNoBar",
    "picSlideTopToBottomNoBar", "picSlideBottomToTopNoBar",
    "picFadeIn", "picFadeIn",
    "picFadeIn", "picFadeIn")
const seValues : array 1 .. * of int := init (
    picWipeLeftToRight, picWipeRightToLeft,
    picWipeTopToBottom, picWipeBottomToTop,
    picWipeCentreToEdge,
    picWipeUpperLeftToLowerRight, picWipeLowerLeftToUpperRight,
    picWipeUpperRightToLowerLeft, picWipeLowerRightToUpperLeft,
    picWipeLeftToRightNoBar, picWipeRightToLeftNoBar,
    picWipeTopToBottomNoBar, picWipeBottomToTopNoBar,
    picWipeCentreToEdgeNoBar,
    picWipeUpperLeftToLowerRightNoBar, picWipeLowerLeftToUpperRightNoBar,
    picWipeUpperRightToLowerLeftNoBar, picWipeLowerRightToUpperLeftNoBar,
    picGrowLeftToRight, picGrowRightToLeft,
    picGrowTopToBottom, picGrowBottomToTop,
    picGrowCentreToEdge, picGrowCenterToEdge,
    picGrowUpperLeftToLowerRight, picGrowLowerLeftToUpperRight,
    picGrowUpperRightToLowerLeft, picGrowLowerRightToUpperLeft,
    picGrowLeftToRightNoBar, picGrowRightToLeftNoBar,
    picGrowTopToBottomNoBar, picGrowBottomToTopNoBar,
    picGrowCentreToEdgeNoBar, picGrowCenterToEdgeNoBar,
    picGrowUpperLeftToLowerRightNoBar, picGrowLowerLeftToUpperRightNoBar,
    picGrowUpperRightToLowerLeftNoBar, picGrowLowerRightToUpperLeftNoBar,
    picSlideLeftToRight, picSlideRightToLeft,
    picSlideTopToBottom, picSlideBottomToTop,
    picSlideLeftToRightNoBar, picSlideRightToLeftNoBar,
    picSlideTopToBottomNoBar, picSlideBottomToTopNoBar,
    picFadeIn, picFadeIn,
    picFadeIn, picFadeIn)
var maxTransitions := upper (seNames)
 
% Create text pictures
procedure CreateTextPictures
    cls
    var f := Font.New ("serif:60:bold,italic")
    Font.Draw ("Here are", 100, 100, f, black)
    text (1) := Pic.New (90, 70, 410, 160)
    cls
    Font.Draw ("Turing", 130, 100, f, black)
    text (2) := Pic.New (90, 70, 410, 160)
    cls
    Font.Draw ("Special", 130, 100, f, black)
    text (3) := Pic.New (90, 70, 410, 160)
    cls
    Font.Draw ("Effects!", 110, 100, f, black)
    text (4) := Pic.New (90, 70, 410, 160)
end CreateTextPictures

procedure LoadPhotos
    % Load photos
    photos (1) := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/phot1.jpg")
    photos (2) := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/phot2.jpg")
    photos (3) := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/phot3.jpg")
    photos (4) := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/phot4.jpg")
end LoadPhotos


function PauseOnKey (y : int) : boolean
    var w : int
    if hasch then
	var ch := getchar
	Draw.FillBox (0, y - 10, maxx, maxy, white)
	if ch = "+" then
	    timeInMillis := round (timeInMillis * 0.75)
	    w := Font.Width ("Transition time = " + intstr (timeInMillis) + " milliseconds", f2)
	    Font.Draw ("Transition time = " + intstr (timeInMillis) + " milliseconds", (maxx - w) div 2, y, f2, black)
	elsif ch = "-" then
	    timeInMillis := round (timeInMillis * 1.25)
	    w := Font.Width ("Transition time = " + intstr (timeInMillis) + " milliseconds", f2)
	    Font.Draw ("Transition time = " + intstr (timeInMillis) + " milliseconds", (maxx - w) div 2, y, f2, black)
	elsif ch = " " then
	    w := Font.Width (pressMessage, f1)
	    Font.Draw (pressMessage, (maxx - w) div 2, y, f1, black)
	    Input.Pause
	    Draw.FillBox (0, y - 10, maxx, maxy, white)
	    w := Font.Width (startMessage, f3)
	    Font.Draw (startMessage, (maxx - w) div 2, 290, f3, black)
	elsif ch = KEY_BACKSPACE then
	    w := Font.Width (repeatMessage, f1)
	    Font.Draw (repeatMessage, (maxx - w) div 2, y, f1, black)
	    result true
	else
	    w := Font.Width (startMessage, f3)
	    Font.Draw (startMessage, (maxx - w) div 2, 290, f3, black)
	end if
    end if
    Input.Flush
    result false
end PauseOnKey

% Draw Text
proc DrawText
    var pic : int := 1
    var i : int := 1
    cls
    var w := Font.Width (startMessage, f3)
    Font.Draw (startMessage, (maxx - w) div 2, 140, f3, black)
    Draw.Box (39, 29, 361, 121, brightred)
    loop
	w := Font.Width (seNames (i), f2)
	Font.Draw (seNames (i), (maxx - w) div 2, 10, f2, black)
	Pic.DrawSpecial (text (pic), 40, 30, picCopy, seValues (i), timeInMillis)
	if PauseOnKey (140) then
	    Pic.Draw (text ((pic + 2) mod 4 + 1), 40, 30, picCopy)
	else
	    Draw.FillBox (0, 0, maxx, 28, white)
	    pic := pic mod 4 + 1
	    i := i + 1
	end if
	exit when i > maxTransitions
    end loop
end DrawText

% Draw Photos
proc DrawPhotos
    var pic : int := 1
    var i : int := 1
    cls
    var w := Font.Width (startMessage, f3)
    Font.Draw (startMessage, (maxx - w) div 2, 290, f3, black)
    Draw.Box (39, 29, 360, 270, brightred)
    loop
	w := Font.Width (seNames (i), f2)
	Font.Draw (seNames (i), (maxx - w) div 2, 10, f2, black)
	Pic.DrawSpecial (photos (pic), 40, 30, picCopy, seValues (i), timeInMillis)
	if PauseOnKey (290) then
	    Pic.Draw (photos ((pic + 2) mod 4 + 1), 40, 30, picCopy)
	else
	    Draw.FillBox (0, 0, maxx, 28, white)
	    pic := pic mod 4 + 1
	    i := i + 1
	end if
	exit when i > maxTransitions
    end loop
end DrawPhotos

% Draw Both Photo and Text
proc DrawBoth
    var pic : int := 1
    var i : int := 1
    var j : int := maxTransitions
    cls
    var w := Font.Width (startMessage, f3)
    Font.Draw (startMessage, (maxx - w) div 2, 290, f3, black)
    Draw.Box (39, 29, 360, 270, brightred)
    Draw.Box (39, 349, 361, 441, brightred)
    loop
	w := Font.Width (seNames (i), f2)
	Font.Draw (seNames (i), (maxx - w) div 2, 10, f2, black)
	Pic.DrawSpecialBack (photos (pic), 40, 30, picCopy, seValues (i), timeInMillis)
	w := Font.Width (seNames (j), f2)
	Font.Draw (seNames (j), (maxx - w) div 2, 330, f2, black)
	Pic.DrawSpecial (text (pic), 40, 350, picCopy, seValues (j), timeInMillis)
	if PauseOnKey (290) then
	    Pic.Draw (photos ((pic + 2) mod 4 + 1), 40, 30, picCopy)
	    Pic.Draw (text ((pic + 2) mod 4 + 1), 40, 350, picCopy)
	else
	    Draw.FillBox (0, 0, maxx, 28, white)
	    Draw.FillBox (0, 320, maxx, 348, white)
	    pic := pic mod 4 + 1
	    i := i + 1
	    j := j - 1
	end if
	exit when i > maxTransitions
    end loop
end DrawBoth

% Main program
f1 := Font.New ("serif:20:bold,italic")
f2 := Font.New ("serif:14:bold")
f3 := Font.New ("serif:9")
CreateTextPictures
LoadPhotos
View.Set ("graphics:400;320,nobuttonbar")
DrawPhotos
DrawText
View.Set ("graphics:400;450,nobuttonbar")
DrawBoth
