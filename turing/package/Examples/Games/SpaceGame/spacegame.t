% Some configurable constants
const MAX_ASTEROIDS : int := 200
const MAX_FRAME_RATE : int := 30
const TIME_BETWEEN_ASTEROIDS : int := 3000

const BOUNCE_SOUND : string := "bounce.wav"
const EXPLOSION_SOUND : string := "explosion.wav"
const RUMBLE_SOUND : string := "engine.wav"
const SILENCE_SOUND : string := "silence.wav"

% Some constants that shouldn't be changed without looking at the
% source code.
const NUM_SHIP_IMAGES : int := 36
const UP_ARROW : char := chr (200)
const LEFT_ARROW : char := chr (203)
const RIGHT_ARROW : char := chr (205)
const SHIP_WINDOW_MARGINS : int := 130
const ASTEROID_RADIUS : int := 35
const SHIP_RADIUS : int := 19
const SHIPF_RADIUS : int := 23
const MARGINS : int := SHIP_WINDOW_MARGINS + ASTEROID_RADIUS
const MIN_MILLISECS_BETWEEN_FRAMES : int := 1000 div MAX_FRAME_RATE

% The pictures
var asteroid : array 0 .. 119 of int
var ship : array 0 .. 35 of int
var shipf : array 0 .. 35 of int
var space : int

% More information
var backgroundPictureWidth, backgroundPictureHeight : int
var backgroundPictureX, backgroundPictureY : int
var shipAngle : int
var shipX, shipY, shipDX, shipDY : real

var asteroidX, asteroidY : array 1 .. MAX_ASTEROIDS of real
var asteroidDX, asteroidDY : array 1 .. MAX_ASTEROIDS of real
var asteroidAngle, asteroidDAngle : array 1 .. MAX_ASTEROIDS of real
var numAsteroids : int := 0
var lastAsteroidTime : int := 0

% The font for score display
var displayFont : int := Font.New ("Sanserif:14:bold")
var announceFont : int := Font.New ("Sanserif:72:bold")

% The current time
var startTime : int
var lastFrameTime : int := 0

% The keys currently being pressed
var keys : array char of boolean

% Is the engine rumbling or not
var engineRumble : boolean := false
var finished : boolean := false
var frameTime : int := 0
var lastFrameRate : int := 0

% Used to play a single sound effect
process PlaySound (file : string)
    engineRumble := false
    Music.PlayFile (file)
end PlaySound

% Used to play the rumble sound
process PlayContinuousRumble
    loop
	Music.PlayFile (RUMBLE_SOUND)
	exit when not engineRumble
    end loop
end PlayContinuousRumble
  
% Used to play background music.  The background music should be
% a MIDI file or an MP3 file so as not to interfere with the
% sound effects (WAV files).
process PlayBackgroundMusic (file : string)
    loop
	Music.PlayFile (file)
	exit when finished
    end loop
end PlayBackgroundMusic

procedure CentreText (s : string, row : int)
    locate (row, (maxcol - length (s)) div 2)
    put s
    View.Update
end CentreText

procedure CentreFont (s : string, x, y, f, clr : int)
    var w : int := Font.Width (s, f)
    Font.Draw (s, x - w div 2, y, f, clr)
end CentreFont

% This method is called each time the screen is repainted
procedure DrawScreen
    var frameRate : int

    % Draw the background.  We don't have to clear the screen because
    % this will do it for us.
    Pic.Draw (space, -backgroundPictureX, -backgroundPictureY, picCopy)

    % Draw the number of frames per second
    frameRate := 1000 div (Time.Elapsed - frameTime)
    if abs (lastFrameRate - frameRate) > 1 then
	lastFrameRate := frameRate
    end if
    locate (1, 1)
    put lastFrameRate ..
    frameTime := Time.Elapsed

    % Draw the asteroids
    for i : 1 .. numAsteroids
	Pic.Draw (asteroid (round ((asteroidAngle (i) - 1.5) / 3)),
	    round (asteroidX (i) - backgroundPictureX - ASTEROID_RADIUS),
	    round (asteroidY (i) - backgroundPictureY - ASTEROID_RADIUS),
	    picMerge)
    end for

    % Draw the ship (with flame if engine is rumbling0
    if engineRumble then
	Pic.Draw (shipf (shipAngle),
	    round (shipX) - backgroundPictureX - SHIPF_RADIUS,
	    round (shipY) - backgroundPictureY - SHIPF_RADIUS, picMerge)
    else
	Pic.Draw (ship (shipAngle),
	    round (shipX) - backgroundPictureX - SHIP_RADIUS,
	    round (shipY) - backgroundPictureY - SHIP_RADIUS, picMerge)
    end if

    % Draw score
    var timePassed : int := (Time.Elapsed - startTime) div 1000
    CentreFont ("SCORE: " + intstr (timePassed, 3) + "     " +
	"ASTEROIDS: " + intstr (numAsteroids, 2),
	maxx div 2, 5, displayFont, white)

    % Draw the edge of the universe (the box the ship travels in)
    Draw.Box (MARGINS - backgroundPictureX, MARGINS - backgroundPictureY,
	backgroundPictureWidth - backgroundPictureX - MARGINS,
	backgroundPictureHeight - backgroundPictureY - MARGINS, grey)

    % If finished then
    if finished then
	CentreFont ("BOOM!", maxx div 2, maxy div 2, announceFont, brightred)
	% Stop the background music and any playing sounds
	engineRumble := false
	Music.PlayFileStop
	% Play the explosion
	fork PlaySound (EXPLOSION_SOUND)
    end if

    View.Update
end DrawScreen

% We check for a collision by doing things the easy way.  We assume
% that both the asteroid and the space ship is circular and see if they
% come within (asteroid radius + ship radius) of each other.  If they
% do, then boom!  We're a little generous, which means that ships
% and asteroids may overlap a little without a boom.
function CheckForCollision : boolean
    var radiiSquared : real := (ASTEROID_RADIUS + SHIP_RADIUS - 4) ** 2
    for i : 1 .. numAsteroids
	var dxSquared : real := (asteroidX (i) - shipX) ** 2
	var dySquared : real := (asteroidY (i) - shipY) ** 2
	if dxSquared + dySquared < radiiSquared then
	    result true
	end if
    end for
    result false
end CheckForCollision

% Start the program
setscreen ("graphics:max,max,noecho,offscreenonly")

CentreText ("TURING ASTEROIDS", maxrow div 2 - 7)
CentreText ("Use the left, right and up arrow keys to", maxrow div 2 - 5)
CentreText ("avoid the asteroids for as long as possible", maxrow div 2 - 4)
CentreText ("Number in upper-left corner is the frame rate (max: 30)",
    maxrow div 2 - 2)
CentreText ("Loading the pictures", maxrow div 2)

% Play the background music while loading the picture
fork PlayBackgroundMusic ("background.mid")

% Load the pictures
asteroid (0) := Pic.FileNew ("asteroid.bmp")
Pic.SetTransparentColour (asteroid (0), black)
ship (0) := Pic.FileNew ("ship.bmp")
Pic.SetTransparentColour (ship (0), black)
shipf (0) := Pic.FileNew ("shipf.bmp")
Pic.SetTransparentColour (shipf (0), black)
space := Pic.FileNew ("space.jpg")

CentreText ("Generating rotated versions of the pictures", maxrow div 2 + 2)

for i : 1 .. 35
    ship (i) := Pic.Rotate (ship (0), i * 10, SHIP_RADIUS, SHIP_RADIUS)
    shipf (i) := Pic.Rotate (shipf (0), i * 10, SHIPF_RADIUS, SHIPF_RADIUS)
end for
for i : 1 .. 119
    asteroid (i) := Pic.Rotate (asteroid (0), i * 3, ASTEROID_RADIUS,
	ASTEROID_RADIUS)
end for

% Calculate the background's width and height
backgroundPictureWidth := Pic.Width (space)
backgroundPictureHeight := Pic.Height (space)

% Figure out where the in the background we are
backgroundPictureX := (backgroundPictureWidth - (maxx + 1)) div 2
backgroundPictureY := (backgroundPictureHeight - (maxy + 1)) div 2

% Set the ship's initial angle, location and velocity
shipAngle := 0
shipX := backgroundPictureWidth / 2
shipY := backgroundPictureHeight / 2
shipDX := 0
shipDY := 0

% Set the text colour for frame count
colour (brightred)
colourback (black)

% Get the initial time
startTime := Time.Elapsed

% The main loop
loop
    Input.KeyDown (keys)
    if keys (UP_ARROW) then
	% Up arrow is pressed: The main engine is on.
	% Turn on the rumble sound if it is not already on.
	if not engineRumble then
	    engineRumble := true
	    fork PlayContinuousRumble
	end if
	% Change the velocity by one in the appropriate direction
	shipDX -= sind (shipAngle * 10)
	shipDY += cosd (shipAngle * 10)
    elsif engineRumble then
	% Up arrow is not pressed: The main engine is off.
	
	% Playing another WAV file stops the first one.  (We didn't want
	% to use Music.PlayFileStop because that would stop the background
	% MIDI music as well.)  The PlaySound procedure also sets 
	% engineRumble to false.
	fork PlaySound (SILENCE_SOUND)
    end if

    % Turn the ship left or right if the left or right arrows
    % are pressed.
    if keys (RIGHT_ARROW) then
	shipAngle := (shipAngle - 1) mod NUM_SHIP_IMAGES
    end if
    if keys (LEFT_ARROW) then
	shipAngle := (shipAngle + 1) mod NUM_SHIP_IMAGES
    end if

    % Establish a speed limit
    shipDX := min (max (shipDX, -20), 20)
    shipDY := min (max (shipDY, -20), 20)

    % Change the ship's position
    shipX += shipDX
    shipY += shipDY

    % Bounce the ship when you hit the edges
    if shipX < MARGINS then
	shipX := MARGINS + (MARGINS - shipX)
	shipDX := -shipDX
	fork PlaySound (BOUNCE_SOUND)
    end if
    if shipX > backgroundPictureWidth - MARGINS then
	shipX := backgroundPictureWidth - MARGINS - (shipX - (backgroundPictureWidth - MARGINS))
	shipDX := -shipDX
	fork PlaySound (BOUNCE_SOUND)
    end if
    if shipY < MARGINS then
	shipY := MARGINS + (MARGINS - shipY)
	shipDY := -shipDY
	fork PlaySound (BOUNCE_SOUND)
    end if
    if shipY > backgroundPictureHeight - MARGINS then
	shipY := backgroundPictureHeight - MARGINS - (shipY - (backgroundPictureHeight - MARGINS))
	shipDY := -shipDY
	fork PlaySound (BOUNCE_SOUND)
    end if

    % Move the background when the ship get close to the edges
    if shipX - backgroundPictureX < SHIP_WINDOW_MARGINS then
	backgroundPictureX := round (shipX - SHIP_WINDOW_MARGINS)
    end if
    if backgroundPictureX + maxx + 1 - shipX < SHIP_WINDOW_MARGINS then
	backgroundPictureX := round (shipX + SHIP_WINDOW_MARGINS - (maxx + 1))
    end if
    if shipY - backgroundPictureY < SHIP_WINDOW_MARGINS then
	backgroundPictureY := round (shipY - SHIP_WINDOW_MARGINS)
    end if
    if backgroundPictureY + maxy + 1 - shipY < SHIP_WINDOW_MARGINS then
	backgroundPictureY := round (shipY + SHIP_WINDOW_MARGINS - (maxy + 1))
    end if

    backgroundPictureX := max (backgroundPictureX, 0)
    backgroundPictureX := min (backgroundPictureX,
	backgroundPictureWidth - (maxx + 1))
    backgroundPictureY := max (backgroundPictureY, 0)
    backgroundPictureY := min (backgroundPictureY,
	backgroundPictureHeight - (maxy + 1))

    % Create a new asteroid, if it is time (one is created
    % every 3 seconds)
    var currentTime := Time.Elapsed
    if (currentTime > lastAsteroidTime + TIME_BETWEEN_ASTEROIDS) and
	    (numAsteroids < MAX_ASTEROIDS) then
	% Enough time has passed and theirs room in the array,
	% create another asteroid.

	% We create the astreoid off the screen so it won't
	% appear to teleport into the game.
	numAsteroids += 1
	asteroidX (numAsteroids) := Rand.Int (-ASTEROID_RADIUS,
	    backgroundPictureWidth + ASTEROID_RADIUS)
	asteroidY (numAsteroids) := Rand.Int (-ASTEROID_RADIUS,
	    backgroundPictureHeight + ASTEROID_RADIUS)
	asteroidDX (numAsteroids) := Rand.Int (-30, 30) / 10
	asteroidDY (numAsteroids) := Rand.Int (-30, 30) / 10
	asteroidAngle (numAsteroids) := 0
	% Asteroids don't look good when rotating very slowly
	% because you see the jump from one image to the next.
	loop
	    asteroidDAngle (numAsteroids) := Rand.Int (-30, 30) / 10
	    exit when abs (asteroidDAngle (numAsteroids)) >= 1
	end loop

	case Rand.Int (1, 4) of
	    label 1 :
		% Create asteroid above the playing field
		asteroidY (numAsteroids) := -ASTEROID_RADIUS
	    label 2 :
		% Create asteroid below the playing field
		asteroidY (numAsteroids) := backgroundPictureHeight + ASTEROID_RADIUS
	    label 3 :
		% Create asteroid to the left of the playing field
		asteroidX (numAsteroids) := -ASTEROID_RADIUS
	    label 4 :
		% Create asteroid to the right of the playing field
		asteroidX (numAsteroids) := backgroundPictureWidth + ASTEROID_RADIUS
	end case

	% To make the game interesting, 1 in 4 asteroids are (1) faster and
	% (2) start aimed more or less at the player!
	if numAsteroids mod 4 = 1 then
	    % Set asteroidDX/DY to aim it at the space ship
	    var d : real := sqrt ((shipX - asteroidX (numAsteroids)) ** 2 +
		(shipY - asteroidY (numAsteroids)) ** 2)
	    var s : int := Rand.Int (7, 11)
	    asteroidDX (numAsteroids) := s * (shipX - asteroidX (numAsteroids)) / d
	    asteroidDY (numAsteroids) := s * (shipY - asteroidY (numAsteroids)) / d
	end if
	lastAsteroidTime := currentTime
    end if

    % Move all the existing asteroids
    for i : 1 .. numAsteroids
	% Change the asteroid's position
	asteroidX (i) += asteroidDX (i)
	asteroidY (i) += asteroidDY (i)
	asteroidAngle (i) += asteroidDAngle (i)
	if asteroidX (i) < -ASTEROID_RADIUS then
	    asteroidX (i) += backgroundPictureWidth + 2 * ASTEROID_RADIUS
	end if
	if asteroidX (i) > backgroundPictureWidth + ASTEROID_RADIUS then
	    asteroidX (i) -= backgroundPictureWidth + 2 * ASTEROID_RADIUS
	end if
	if asteroidY (i) < -ASTEROID_RADIUS then
	    asteroidY (i) += backgroundPictureWidth + 2 * ASTEROID_RADIUS
	end if
	if asteroidY (i) > backgroundPictureHeight + ASTEROID_RADIUS then
	    asteroidY (i) -= backgroundPictureHeight + 2 * ASTEROID_RADIUS
	end if
	if asteroidAngle (i) < 0 then
	    asteroidAngle (i) += 360
	end if
	if asteroidAngle (i) >= 360 then
	    asteroidAngle (i) -= 360
	end if
    end for
    if CheckForCollision then
	finished := true
    end if
    DrawScreen

    % If the frame rate is too high, then pause.  Otherwise the game
    % runs too quickly!
    if Time.Elapsed - lastFrameTime < MIN_MILLISECS_BETWEEN_FRAMES then
	delay (MIN_MILLISECS_BETWEEN_FRAMES - (Time.Elapsed - lastFrameTime))
    end if
    lastFrameTime := Time.Elapsed

    exit when finished
end loop

