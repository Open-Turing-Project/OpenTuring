var p, bg : int
const t : int := 4000
const mode : int := picMerge

procedure DrawBG
    cls
    var x := 0
    loop
	exit when x > maxx
	drawline (x, 0, x, maxy, x mod 8 + 9)
	x += 8
    end loop
    var y := 0
    loop
	exit when y > maxy
	drawline (0, y, maxx, y, y mod 8 + 9)
	y += 8
    end loop
end DrawBG


procedure PicDrawBoxes (doing : string, kind : int)
    locatexy (100, 80)
    put " ", doing, "? " ..
    Input.Pause

    DrawBG
    Pic.Draw (bg, -75, -51, picCopy)
    Pic.Draw (bg, -75, 149, picCopy)
    Pic.Draw (bg, -75, maxy - 50, picCopy)
    Pic.Draw (bg, 269, -51, picCopy)
    Pic.Draw (bg, 269, 149, picCopy)
    Pic.Draw (bg, 269, maxy - 50, picCopy)
    Pic.Draw (bg, maxx - 74, -51, picCopy)
    Pic.Draw (bg, maxx - 74, 149, picCopy)
    Pic.Draw (bg, maxx - 74, maxy - 50, picCopy)

    locatexy (100, 100)
    put " ", doing, " " ..

    Pic.DrawSpecialBack (p, -75, -51, mode, kind, t)
    Pic.DrawSpecialBack (p, -75, 149, mode, kind, t)
    Pic.DrawSpecialBack (p, -75, maxy - 50, mode, kind, t)
    Pic.DrawSpecialBack (p, 269, -51, mode, kind, t)
    Pic.DrawSpecialBack (p, 269, 149, mode, kind, t)
    Pic.DrawSpecialBack (p, 269, maxy - 50, mode, kind, t)
    Pic.DrawSpecialBack (p, maxx - 74, -51, mode, kind, t)
    Pic.DrawSpecialBack (p, maxx - 74, 149, mode, kind, t)
    Pic.DrawSpecial (p, maxx - 74, maxy - 50, mode, kind, t)
end PicDrawBoxes

function GetMain : int
    DrawBG
    Draw.Box (101, 101, 251, 203, brightred)
    Font.Draw ("Top", 159, 187, defFontID, black)
    Font.Draw ("Bottom", 148, 110, defFontID, black)
    Font.Draw ("Left", 107, 147, defFontID, black)
    Font.Draw ("Right", 205, 147, defFontID, black)
    result Pic.New (101, 101, 251, 203)
end GetMain

function GetBG : int
    cls
    %var x := -maxy
    %loop
    %    exit when x > maxx
    %    drawline (x, 0, x + maxy, maxy, x mod 8 + 8)
    %    x += 4
    %end loop
    Draw.FillBox (103, 103, 249, 201, yellow)
    Font.Draw ("Top", 159, 177, defFontID, brightmagenta)
    Font.Draw ("Bottom", 148, 120, defFontID, brightmagenta)
    Font.Draw ("Left", 107, 137, defFontID, brightmagenta)
    Font.Draw ("Right", 205, 137, defFontID, brightmagenta)
    result Pic.New (101, 101, 251, 203)
end GetBG

p := GetMain
bg := GetBG

% PicDrawBoxes ("picWipeLeftToRight", picWipeLeftToRight)
% PicDrawBoxes ("picWipeRightToLeft", picWipeRightToLeft)
% PicDrawBoxes ("picWipeTopToBottom", picWipeTopToBottom)
% PicDrawBoxes ("picWipeBottomToTop", picWipeBottomToTop)
% PicDrawBoxes ("picWipeCentreToEdge", picWipeCentreToEdge)
% PicDrawBoxes ("picWipeCenterToEdge", picWipeCenterToEdge)
% /*PicDrawBoxes ("picWipeEdgeToCentre", picWipeEdgeToCentre)*/
% /*PicDrawBoxes ("picWipeEdgeToCenter", picWipeEdgeToCenter)*/
PicDrawBoxes ("picWipeUpperLeftToLowerRight", picWipeUpperLeftToLowerRight)
PicDrawBoxes ("picWipeLowerLeftToUpperRight", picWipeLowerLeftToUpperRight)
PicDrawBoxes ("picWipeUpperRightToLowerLeft", picWipeUpperRightToLowerLeft)
PicDrawBoxes ("picWipeLowerRightToUpperLeft", picWipeLowerRightToUpperLeft)

% PicDrawBoxes ("picWipeLeftToRightNoBar", picWipeLeftToRightNoBar)
% PicDrawBoxes ("picWipeRightToLeftNoBar", picWipeRightToLeftNoBar)
% PicDrawBoxes ("picWipeTopToBottomNoBar", picWipeTopToBottomNoBar)
% PicDrawBoxes ("picWipeBottomToTopNoBar", picWipeBottomToTopNoBar)
% PicDrawBoxes ("picWipeCentreToEdgeNoBar", picWipeCentreToEdgeNoBar)
% PicDrawBoxes ("picWipeCenterToEdgeNoBar", picWipeCenterToEdgeNoBar)
% /*PicDrawBoxes ("picWipeEdgeToCentreNoBar", picWipeEdgeToCentreNoBar)*/
% /*PicDrawBoxes ("picWipeEdgeToCenterNoBar", picWipeEdgeToCenterNoBar)*/
PicDrawBoxes ("picWipeUpperLeftToLowerRightNoBar", picWipeUpperLeftToLowerRightNoBar)
PicDrawBoxes ("picWipeLowerLeftToUpperRightNoBar", picWipeLowerLeftToUpperRightNoBar)
PicDrawBoxes ("picWipeUpperRightToLowerLeftNoBar", picWipeUpperRightToLowerLeftNoBar)
PicDrawBoxes ("picWipeLowerRightToUpperLeftNoBar", picWipeLowerRightToUpperLeftNoBar)

PicDrawBoxes ("picGrowLeftToRight", picGrowLeftToRight)
PicDrawBoxes ("picGrowRightToLeft", picGrowRightToLeft)
PicDrawBoxes ("picGrowTopToBottom", picGrowTopToBottom)
PicDrawBoxes ("picGrowBottomToTop", picGrowBottomToTop)
PicDrawBoxes ("picGrowCentreToEdge", picGrowCentreToEdge)
PicDrawBoxes ("picGrowCenterToEdge", picGrowCenterToEdge)
PicDrawBoxes ("picGrowUpperLeftToLowerRight", picGrowUpperLeftToLowerRight)
PicDrawBoxes ("picGrowLowerLeftToUpperRight", picGrowLowerLeftToUpperRight)
PicDrawBoxes ("picGrowUpperRightToLowerLeft", picGrowUpperRightToLowerLeft)
PicDrawBoxes ("picGrowLowerRightToUpperLeft", picGrowLowerRightToUpperLeft)
PicDrawBoxes ("picGrowLeftToRightNoBar", picGrowLeftToRightNoBar)
PicDrawBoxes ("picGrowRightToLeftNoBar", picGrowRightToLeftNoBar)
PicDrawBoxes ("picGrowTopToBottomNoBar", picGrowTopToBottomNoBar)
PicDrawBoxes ("picGrowBottomToTopNoBar", picGrowBottomToTopNoBar)
PicDrawBoxes ("picGrowCentreToEdgeNoBar", picGrowCentreToEdgeNoBar)
PicDrawBoxes ("picGrowCenterToEdgeNoBar", picGrowCenterToEdgeNoBar)
PicDrawBoxes ("picGrowUpperLeftToLowerRightNoBar", picGrowUpperLeftToLowerRightNoBar)
PicDrawBoxes ("picGrowLowerLeftToUpperRightNoBar", picGrowLowerLeftToUpperRightNoBar)
PicDrawBoxes ("picGrowUpperRightToLowerLeftNoBar", picGrowUpperRightToLowerLeftNoBar)
PicDrawBoxes ("picGrowLowerRightToUpperLeftNoBar", picGrowLowerRightToUpperLeftNoBar)

PicDrawBoxes ("picSlideLeftToRight", picSlideLeftToRight)
PicDrawBoxes ("picSlideRightToLeft", picSlideRightToLeft)
PicDrawBoxes ("picSlideTopToBottom", picSlideTopToBottom)
PicDrawBoxes ("picSlideBottomToTop", picSlideBottomToTop)
PicDrawBoxes ("picSlideLeftToRightNoBar", picSlideLeftToRightNoBar)
PicDrawBoxes ("picSlideRightToLeftNoBar", picSlideRightToLeftNoBar)
PicDrawBoxes ("picSlideTopToBottomNoBar", picSlideTopToBottomNoBar)
PicDrawBoxes ("picSlideBottomToTopNoBar", picSlideBottomToTopNoBar)
PicDrawBoxes ("picFadeIn", picFadeIn)
PicDrawBoxes ("picBlend + 50", picBlend + 50)

/*
 !   picWipeLeftToRight, picWipeRightToLeft,
 !   picWipeTopToBottom, picWipeBottomToTop,
 !   picWipeCentreToEdge, picWipeCenterToEdge,
 x   picWipeEdgeToCentre, picWipeEdgeToCenter,
 !   picWipeUpperLeftToLowerRight, picWipeLowerLeftToUpperRight,
 !   picWipeUpperRightToLowerLeft, picWipeLowerRightToUpperLeft,
 !   picWipeLeftToRightNoBar, picWipeRightToLeftNoBar,
 !   picWipeTopToBottomNoBar, picWipeBottomToTopNoBar,
 !   picWipeCentreToEdgeNoBar, picWipeCenterToEdgeNoBar,
 x   picWipeEdgeToCentreNoBar, picWipeEdgeToCenterNoBar,
 !   picWipeUpperLeftToLowerRightNoBar, picWipeLowerLeftToUpperRightNoBar,
 !   picWipeUpperRightToLowerLeftNoBar, picWipeLowerRightToUpperLeftNoBar,

 !   picGrowLeftToRight, picGrowRightToLeft,
 !   picGrowTopToBottom, picGrowBottomToTop,
 !   picGrowCentreToEdge, picGrowCenterToEdge,
 !   picGrowUpperLeftToLowerRight, picGrowLowerLeftToUpperRight,
 !   picGrowUpperRightToLowerLeft, picGrowLowerRightToUpperLeft,
 !   picGrowLeftToRightNoBar, picGrowRightToLeftNoBar,
 !   picGrowTopToBottomNoBar, picGrowBottomToTopNoBar,
 !   picGrowCentreToEdgeNoBar, picGrowCenterToEdgeNoBar,
 !   picGrowUpperLeftToLowerRightNoBar, picGrowLowerLeftToUpperRightNoBar,
 !   picGrowUpperRightToLowerLeftNoBar, picGrowLowerRightToUpperLeftNoBar,

 !   picSlideLeftToRight, picSlideRightToLeft,
 !   picSlideTopToBottom, picSlideBottomToTop,
 !   picSlideLeftToRightNoBar, picSlideRightToLeftNoBar,
 !   picSlideTopToBottomNoBar, picSlideBottomToTopNoBar,

 !   picFadeIn, picBlend
 */

