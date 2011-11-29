/*
 * Sprite module - a set of routines for using sprites
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Sprite
    export New, Free, Show, Hide, SetPosition, SetHeight, ChangePic, Animate,
	backgroundHeight, SetFrameRate

    const backgroundHeight : int := 0

    external "sprite_new" function New (picId : int) : int

    external "sprite_free" procedure Free (spriteId : int)

    external "sprite_show" procedure Show (spriteId : int)

    external "sprite_hide" procedure Hide (spriteId : int)

    external "sprite_setposition"
	procedure SetPosition (picId : int, x, y : int, centered : boolean)

    external "sprite_setheight"
	procedure SetHeight (spriteId : int, height : int)

    external "sprite_changepic"
	procedure ChangePic (spriteId : int, picId : int)

    external "sprite_animate"
	procedure Animate (spriteId : int, picId : int,
	x, y : int, centered : boolean)
	
    external "sprite_setframerate"
	procedure SetFrameRate (frameRate : int)
end Sprite

