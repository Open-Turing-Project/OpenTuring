% The "Font.GetName" program.
var fontName : string
setscreen ("text")
Font.StartName
loop
    fontName := Font.GetName
    exit when fontName = ""
    put fontName
end loop
