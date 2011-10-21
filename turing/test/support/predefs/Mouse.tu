/*
 * Mouse module - the simplified mouse interface
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Mouse
    export Where, ButtonMoved, ButtonWait, ButtonChoose,
    % Not yet implemented
	Hide, Show

    external "mouse_where"
	procedure Where (var x, y, buttonUpDown : int)

    external "mouse_hide" procedure Hide

    external "mouse_show" procedure Show

    external "mouse_buttonmoved"
	function ButtonMoved (motionStr : string) : boolean

    external "mouse_buttonwait"
	procedure ButtonWait (motionStr : string, var x, y : int,
	var btnNumber, buttonUpDown : int)

    external "mouse_buttonchoose"
	procedure ButtonChoose (chooseStr : string)
end Mouse
