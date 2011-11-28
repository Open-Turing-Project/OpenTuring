% The "Draw.Text" program.
var font : int

% Create the font - A 36 pt. serif font.
font := Font.New ("serif:36")   % "serif:36:italic" for italic version

% Determine the width of the text (in pixels) in that particular font.
var width : int := Font.Width ("This is in a good serif font", font)

% Determine the height of the font.
var height, ascent, descent, internalLeading : int
Font.Sizes (font, height, ascent, descent, internalLeading)

% Draw the text in the font
Draw.Text ("This is in a good serif font", 50, 30, font, red)

% Draw a box around the text. Notice the base line for the text
% is at y = 30, but the bottom of the 'g' goes below that. 
Draw.Box (50, 30 - descent, 50 + width, 30 + ascent, brightblue)

% Free the font after use.
Font.Free (font)
