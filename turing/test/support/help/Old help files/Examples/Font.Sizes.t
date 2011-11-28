% The "Font.Sizes" program.
var fontID, height, ascent, descent, internalLeading : int
var externalLeading : int
fontID := Font.New ("Palatino:72:bold")
Font.Sizes (fontID, height, ascent, descent, internalLeading)
externalLeading := height - ascent - descent
put "The height of the font is ", height, " pixels"
put "The ascent of the font is ", ascent, " pixels"
put "The descent of the font is ", descent, " pixels"
put "The internal leading of the font is ", internalLeading, " pixels"
put "The external leading of the font is ", externalLeading, " pixels"

Draw.Line (0, 100, maxx, 100, red)
Draw.Line (0, 100 - descent, maxx, 100 - descent, red)
Draw.Line (0, 100 + ascent, maxx, 100 + ascent, red)
Draw.Line (0, 100 + ascent - internalLeading, maxx, 100 + ascent - internalLeading, red)
Font.Draw ("Mj" + chr (209), 20, 100, fontID, black)
Font.Free (fontID)

% Height
fontID := Font.New ("Courier New:114")
Font.Draw ("}", 450, 97, fontID, black)
Font.Free (fontID)
fontID := Font.New ("Serif:14:bold")
Font.Draw ("Height", 518, 127, fontID, black)
Font.Free (fontID)

% Ascent
fontID := Font.New ("Courier New:90")
Font.Draw ("}", 330, 115, fontID, black)
Font.Free (fontID)
fontID := Font.New ("Serif:14:bold")
Font.Draw ("Ascent", 381, 140, fontID, black)
Font.Free (fontID)

% Descent
fontID := Font.New ("Courier New:22:bold")
Font.Draw ("}", 220, 83, fontID, black)
Font.Free (fontID)
fontID := Font.New ("Serif:12:bold")
Font.Draw ("Descent", 241, 84, fontID, black)
Font.Free (fontID)

% Internal Leading
fontID := Font.New ("Serif:11:bold")
Font.Draw ("} Internal Leading", 221, 178, fontID, black)
Font.Free (fontID)
