/*
 * Music module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Music
    export Play, PlayFile, PlayFileStop, PlayFileLoop, PlayFileReturn,
	Sound, SoundOff,
    % Not yet implemented
	PreLoad

    external "music_play" procedure Play (playStr : string)

    external "music_playfile" procedure PlayFile (fileStr : string)

    var playFileStopped : int := 0

    process PlayFileLoopProc (fileStr : string)
	external function error_last : int
	var startPlayFileStopped : int := playFileStopped
	loop
	    PlayFile (fileStr)
	    exit when error_last not= 0
	    exit when startPlayFileStopped not= playFileStopped
	end loop
    end PlayFileLoopProc

    procedure PlayFileLoop (fileStr : string)
	fork PlayFileLoopProc (fileStr)
    end PlayFileLoop

    process PlayFileReturnProc (fileStr : string)
	PlayFile (fileStr)
    end PlayFileReturnProc

    procedure PlayFileReturn (fileStr : string)
	fork PlayFileReturnProc (fileStr)
    end PlayFileReturn

    procedure PlayFileStop
	external procedure music_playfilestop
	playFileStopped += 1
	music_playfilestop
    end PlayFileStop

    external "music_preload" procedure PreLoad (fileStr : string)

    external "music_sound" procedure Sound (frequency, duration : int)

    external "music_soundoff" procedure SoundOff
end Music
