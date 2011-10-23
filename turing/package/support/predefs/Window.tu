/*
 * Window module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Window
    export Open, Close, Select, GetSelect, SetActive,
	GetActive, Hide, Show, IsVisible, Set,
	SetPosition, GetPosition, Update, UpdateArea,
	SetTransparentColour, SetTransparentColor,
	~.*defWinId, ~.*defWinID, ~.*errWinId, ~.*errWinID,
	~.*winPosLeft, ~.*winPosCentre, ~.*winPosCenter, ~.*winPosRight,
	~.*winPosTop, ~.*winPosMiddle, ~.*winPosTrueMiddle, ~.*winPosBottom

    function Open (winSetupStr : string) : int
	external procedure win_registeropen (winId : int)

	var winId : int

	open : winId, "%window(" + winSetupStr + ")", put, get

	external procedure old_setactive (winId : int)
	old_setactive (winId)
	win_registeropen (winId)

	result winId
    end Open

    procedure Close (winId : int)
	external procedure win_registerclose (winId : int)

	close : winId
	win_registerclose (winId)
    end Close

    procedure Select (winId : int)
	external procedure old_setactive (winId : int)

	old_setactive (winId)
    end Select

    external "win_getselect" function GetSelect : int

    procedure SetActive (winId : int)
	external procedure win_setactive (winId : int)
	external procedure old_setactive (winId : int)

	win_setactive (winId)
	old_setactive (winId)
    end SetActive

    external "win_getactive" function GetActive : int

    external "win_hide" procedure Hide (winId : int)

    external "win_show" procedure Show (winId : int)

    external "win_isvisible" function IsVisible (winId : int) : boolean

    external "win_set" procedure Set (winId : int, winSetupStr : string)

    external "win_setposition" procedure SetPosition (winId : int, x, y : int)

    external "win_getposition" procedure GetPosition (winId : int, var x, y : int)

    external "win_update" procedure Update (winId : int)

    external "win_updatearea" procedure UpdateArea (winId, x1, y1, x2, y2 : int)

    external "win_settransclr"
	procedure SetTransparentColour (winId, clrNum : int)

    external "win_settransclr"
	procedure SetTransparentColor (winId, clrNum : int)

    % CONSTANTS

    const *defWinId : int := -1
    const *errWinId : int := 0
    const *defWinID : int := defWinId
    const *errWinID : int := errWinId

    const *winPosLeft : int := -9911
    const *winPosCentre : int := -9912
    const *winPosCenter : int := -9912
    const *winPosRight : int := -9913
    const *winPosTop : int := -9914
    const *winPosMiddle : int := -9915
    const *winPosTrueMiddle : int := -9916
    const *winPosBottom : int := -9917
end Window
