/*
 * Pic module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Pic
    export New, Free, Draw, FileNew, Width, Height, Save,
	ScreenLoad,ScreenLoadNoCache, ScreenSave, Mirror, Flip, Rotate, Scale,
	SetTransparentColor, SetTransparentColour,
	~.*picCopy, ~.*picXor, ~.*picMerge, ~.*picUnderMerge,
    % New with Turing 4.1
	Blend, Blur, Frames, FileNewFrames,
	DrawFrames, DrawFramesBack, DrawSpecial, DrawSpecialBack,
	~.*picWipeLeftToRight, ~.*picWipeRightToLeft,
	~.*picWipeTopToBottom, ~.*picWipeBottomToTop,
	~.*picWipeCentreToEdge, ~.*picWipeCenterToEdge,
	~.*picWipeEdgeToCentre, ~.*picWipeEdgeToCenter,
	~.*picWipeUpperLeftToLowerRight, ~.*picWipeLowerLeftToUpperRight,
	~.*picWipeUpperRightToLowerLeft, ~.*picWipeLowerRightToUpperLeft,
	~.*picWipeLeftToRightNoBar, ~.*picWipeRightToLeftNoBar,
	~.*picWipeTopToBottomNoBar, ~.*picWipeBottomToTopNoBar,
	~.*picWipeCentreToEdgeNoBar, ~.*picWipeCenterToEdgeNoBar,
	~.*picWipeEdgeToCentreNoBar, ~.*picWipeEdgeToCenterNoBar,
	~.*picWipeUpperLeftToLowerRightNoBar, ~.*picWipeLowerLeftToUpperRightNoBar,
	~.*picWipeUpperRightToLowerLeftNoBar, ~.*picWipeLowerRightToUpperLeftNoBar,
	~.*picGrowLeftToRight, ~.*picGrowRightToLeft,
	~.*picGrowTopToBottom, ~.*picGrowBottomToTop,
	~.*picGrowCentreToEdge, ~.*picGrowCenterToEdge,
	~.*picGrowUpperLeftToLowerRight, ~.*picGrowLowerLeftToUpperRight,
	~.*picGrowUpperRightToLowerLeft, ~.*picGrowLowerRightToUpperLeft,
	~.*picGrowLeftToRightNoBar, ~.*picGrowRightToLeftNoBar,
	~.*picGrowTopToBottomNoBar, ~.*picGrowBottomToTopNoBar,
	~.*picGrowCentreToEdgeNoBar, ~.*picGrowCenterToEdgeNoBar,
	~.*picGrowUpperLeftToLowerRightNoBar, ~.*picGrowLowerLeftToUpperRightNoBar,
	~.*picGrowUpperRightToLowerLeftNoBar, ~.*picGrowLowerRightToUpperLeftNoBar,
	~.*picSlideLeftToRight, ~.*picSlideRightToLeft,
	~.*picSlideTopToBottom, ~.*picSlideBottomToTop,
	~.*picSlideLeftToRightNoBar, ~.*picSlideRightToLeftNoBar,
	~.*picSlideTopToBottomNoBar, ~.*picSlideBottomToTopNoBar,
	~.*picFadeIn, ~.*picBlend

    
	
    % Exported constants

    const picCopy := 0
    const picXor := 1
    const picMerge := 2
    const picUnderMerge := 3

    const picWipeLeftToRight := 1
    const picWipeRightToLeft := 2
    const picWipeTopToBottom := 3
    const picWipeBottomToTop := 4
    const picWipeCentreToEdge := 5
    const picWipeCenterToEdge := 5
    const picWipeEdgeToCentre := 6
    const picWipeEdgeToCenter := 6
    const picWipeUpperLeftToLowerRight := 7
    const picWipeLowerLeftToUpperRight := 8
    const picWipeUpperRightToLowerLeft := 9
    const picWipeLowerRightToUpperLeft := 10

    const picWipeLeftToRightNoBar := 21
    const picWipeRightToLeftNoBar := 22
    const picWipeTopToBottomNoBar := 23
    const picWipeBottomToTopNoBar := 24
    const picWipeCentreToEdgeNoBar := 25
    const picWipeCenterToEdgeNoBar := 25
    const picWipeEdgeToCentreNoBar := 26
    const picWipeEdgeToCenterNoBar := 26
    const picWipeUpperLeftToLowerRightNoBar := 27
    const picWipeLowerLeftToUpperRightNoBar := 28
    const picWipeUpperRightToLowerLeftNoBar := 29
    const picWipeLowerRightToUpperLeftNoBar := 30

    const picGrowLeftToRight := 41
    const picGrowRightToLeft := 42
    const picGrowTopToBottom := 43
    const picGrowBottomToTop := 44
    const picGrowCentreToEdge := 45
    const picGrowCenterToEdge := 45
    const picGrowUpperLeftToLowerRight := 47
    const picGrowLowerLeftToUpperRight := 48
    const picGrowUpperRightToLowerLeft := 49
    const picGrowLowerRightToUpperLeft := 50

    const picGrowLeftToRightNoBar := 61
    const picGrowRightToLeftNoBar := 62
    const picGrowTopToBottomNoBar := 63
    const picGrowBottomToTopNoBar := 64
    const picGrowCentreToEdgeNoBar := 65
    const picGrowCenterToEdgeNoBar := 65
    const picGrowUpperLeftToLowerRightNoBar := 67
    const picGrowLowerLeftToUpperRightNoBar := 68
    const picGrowUpperRightToLowerLeftNoBar := 69
    const picGrowLowerRightToUpperLeftNoBar := 70

    const picSlideLeftToRight := 81
    const picSlideRightToLeft := 82
    const picSlideTopToBottom := 83
    const picSlideBottomToTop := 84

    const picSlideLeftToRightNoBar := 91
    const picSlideRightToLeftNoBar := 92
    const picSlideTopToBottomNoBar := 93
    const picSlideBottomToTopNoBar := 94

    const picFadeIn := 99

    const picBlend := 100
    
    % THE CACHE
    
    var cache : int := 0

    % Exported routines

    external "pic_new" function New (x1, y1, x2, y2 : int)
    picId : int

    external "pic_free" procedure Free (picId : int)

    external "pic_blend" function Blend (picId1, picId2, percent : int) : int
    external "pic_blur" function Blur (picId, blurFactor : int) : int

    external "pic_draw" procedure Draw (picId, x, y, mode : int)

    external "pic_drawframes"
	procedure DrawFrames (picIds : array 1 .. * of int, x, y, mode : int,
	numFrames, delayBetweenFrames : int, eraseAfter : boolean)

    external "pic_drawspecial"
	procedure DrawSpecial (picId, x, y, mode, transition, duration : int)

    external "pic_filenew" function FileNew (fileStr : string) : int

    external "pic_filenewframes" procedure FileNewFrames (fileStr : string,
	var picIds : array 1 .. * of int, var delayTime : int)

    external "pic_filesave" procedure Save (picId : int, fileStr : string)

    external "pic_flip" function Flip (picID : int) : int

    external "pic_frames" function Frames (fileStr : string) : int

    external "pic_mirror" function Mirror (picID : int) : int

    external "pic_rotate"
	function Rotate (picID, angle, xAxis, yAxis : int) : int

    external "pic_scale" function Scale (picID, newWidth, newHeight : int) : int

    external "pic_settransclr" procedure SetTransparentColour (picId, clr : int)

    external "pic_settransclr" procedure SetTransparentColor (picId, clr : int)

    procedure ScreenLoad (fileStr : string, x, y, mode : int)
	var picID : int
    
	if cache = 0 then
	    cache := IntHashMap.New()
	end if
	
	var wasStored := IntHashMap.Get(cache,fileStr,picID)
	
	if wasStored = 0 then % not cached
	    picID := Pic.FileNew (fileStr)
	    if picID not= 0 then
				IntHashMap.Put(cache,fileStr,picID)
	    end if
	end if
	
	if picID > 0 then
	    Pic.Draw (picID, x, y, mode)
	end if
    end ScreenLoad
    
    procedure ScreenLoadNoCache (fileStr : string, x, y, mode : int)
	var picID : int
	picID := Pic.FileNew (fileStr)
	if picID > 0 then
	    Pic.Draw (picID, x, y, mode)
	    Pic.Free (picID)
	end if
    end ScreenLoadNoCache

    procedure ScreenSave (x1, y1, x2, y2 : int, fileStr : string)
	var picID : int
	picID := Pic.New (x1, y1, x2, y2)
	if picID > 0 then
	    Pic.Save (picID, fileStr)
	    Pic.Free (picID)
	end if
    end ScreenSave

    function Width (picId : int) : int
	external procedure pic_getdimensions (picId : int, var w : int, var h : int)

	var w, h : int

	pic_getdimensions (picId, w, h)

	result w
    end Width

    function Height (picId : int) : int
	external procedure pic_getdimensions (picId : int, var w : int, var h : int)

	var w, h : int

	pic_getdimensions (picId, w, h)

	result h
    end Height

    process DrawSpecialProcess (picId, x, y, mode, transition, duration : int)
	DrawSpecial (picId, x, y, mode, transition, duration)
    end DrawSpecialProcess

    procedure DrawSpecialBack (picId, x, y, mode, transition, duration : int)
	fork DrawSpecialProcess (picId, x, y, mode, transition, duration)
    end DrawSpecialBack

    process DrawFramesProcess (picIds : array 1 .. * of int, x, y, mode : int,
	    numFrames, delayTime : int, eraseAfter : boolean)
	DrawFrames (picIds, x, y, mode, numFrames, delayTime, eraseAfter)
    end DrawFramesProcess

    procedure DrawFramesBack (picIds : array 1 .. * of int, x, y, mode : int,
	    numFrames, delayTime : int, eraseAfter : boolean)
	fork DrawFramesProcess (picIds, x, y, mode, numFrames,
	    delayTime, eraseAfter)
    end DrawFramesBack
end Pic
