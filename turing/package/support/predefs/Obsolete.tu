/*
 * module Obsolete - obsolete routines we choose to keep around for now
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Obsolete
    export ~.*palette, ~.*whatpalette, ~.*whattextchar, ~.*whattextcolor,
	~.*whattextcolour, ~.*whattextcolorback, ~.*whattextcolourback,
	~.*serialget, ~.*serialput, ~.*randnext, ~.*randseed, ~.*getenv,
	~.*getpid, ~.*drawtext, ~.*strfont, ~.*flushstreams, ~.*setactive

    % This procedure causes a run-time error indicating that the feature
    % is obsolete.  "obsolete_replace" indicates a suitable replacement
    % routine.
    external procedure obsolete (routine : string)
    external procedure obsolete_replace (routine, replacementRoutine :
	string)

    %
    % Documented routines no longer supported
    %
    procedure palette (p : int)
	obsolete ("palette")
    end palette

    function whatpalette : int
	obsolete ("whatpalette")
	result 0
    end whatpalette

    function whattextchar : string (1)
	    obsolete ("whattextchar")
	result " "
    end whattextchar

    function whattextcolor : int
	obsolete ("whattextcolor")
	result 0
    end whattextcolor

    function whattextcolour : int
	obsolete ("whattextcolour")
	result 0
    end whattextcolour

    function whattextcolorback : int
	obsolete ("whattextcolorback")
	result 0
    end whattextcolorback

    function whattextcolourback : int
	obsolete ("whattextcolourback")
	result 0
    end whattextcolourback

    function serialget : int
	obsolete ("serialget")
	result 0
    end serialget

    procedure serialput (p : int)
	obsolete ("serialput")
    end serialput

    procedure randnext (var r : real, seq : 1 .. 10)
	obsolete_replace ("randnext", "Rand.Next")
    end randnext

    procedure randseed (seed : int, seq : 1 .. 10)
	obsolete_replace ("randseed", "Rand.Seed")
    end randseed

    function getenv (symbol : string) : string
	obsolete_replace ("getenv", "Sys.GetEnv")
	result ""
    end getenv

    function getpid : int
	obsolete_replace ("getpid", "Sys.GetPid")
	result 0
    end getpid

    %
    % Old, undocumented routines no longer supported
    %
    procedure drawtext (x, y : int, str : string, font,
	    clr : int)
	obsolete_replace ("drawtext", "Font.Draw")
    end drawtext

    function strfont (fontStr : string) : int
	obsolete_replace ("drawtext", "Font.New")
    end strfont

    procedure flushstreams
	obsolete_replace ("flushstreams",
	    "Stream.FlushAll")
    end flushstreams

    procedure setactive (stream : int)
	obsolete_replace ("setactive", "Window.Select")
    end setactive
end Obsolete

