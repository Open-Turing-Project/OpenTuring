% The "Font.GetStyle" program.
var fontName : string
var bold, italic, underline : boolean
setscreen ("text")
Font.StartName
loop
    fontName := Font.GetName
    exit when fontName = ""
    Font.GetStyle (fontName, bold, italic, underline)
    put fontName : 30 ..
    if bold then
	put "bold  " ..
    end if
    if italic then
	put "italic  " ..
    end if
    if underline then
	put "underline  " ..
    end if
    put ""
end loop
