% The "MoveLogoWithMusic" program
% This program animates a graphic around the window

var logoPicID, bgPicID : int    % The picture Ids of the logo and the bkgrnd
var logoWidth, logoHeight : int % The width and height of the logo picture
var x, y : int := 0             % The location of the logo
var dx, dy : int := 3           % The direction of movement of the logo
var finished : boolean := false % Set to true when program finished

% Process to continuously play background music
process PlayBackgroundMusic (filename : string)
    loop
	exit when finished
	Music.PlayFile (filename)
    end loop
end PlayBackgroundMusic

% Process to play a sound file once
process PlaySoundEffect (filename : string)
    Music.PlayFile (filename)
end PlaySoundEffect

% Load the logo from file and get its height and width
logoPicID := Pic.FileNew ("logo.bmp")
logoWidth := Pic.Width (logoPic)
logoHeight := Pic.Height (logoPic)

% Load the forest picture from the file and draw it to the window
Pic.ScreenLoad ("forest.bmp", 0, 0, picCopy)

% Start the background music
fork PlayBackgroundMusic ("canyon.mid")

loop
    exit when hasch
    bgPicID := Pic.New (x, y, x + logoWidth, y + logoHeight)
    Pic.Draw (logoPicID, x, y, picMerge)
    delay (50)
    Pic.Draw (bgPicID, x, y, picCopy)
    Pic.Free (bgPicID)
    if x + dx < 0 or x + dx + logoWidth > maxx then
	fork PlaySoundEffect ("boing.wav")
	dx := - dx
    end if
    if y + dy < 0 or y + dy + logoHeight > maxy then
	fork PlaySoundEffect ("boing.wav")
	dy := - dy
    end if
    x := x + dx
    y := y + dy
end loop

finished := true
Music.PlayFileStop
