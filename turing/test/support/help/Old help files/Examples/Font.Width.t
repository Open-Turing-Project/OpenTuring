% The "Font.Width" program.
const testString : string := "Test String"
var width : int
var fontID : int
fontID := Font.New ("Palatino:24:Bold")
width := Font.Width (testString, fontID)
put "The width of \"" + testString + "\" is ", width, " pixels"
Font.Draw (testString, 100, 100, fontID, black)
Draw.Line (100, 50, 100, 150, brightred)
Draw.Line (100 + width, 50, 100 + width, 150, brightred)
Font.Free (fontID)
