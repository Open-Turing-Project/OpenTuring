% The "Font.StartName" program.
var fontName : string
Font.StartName
loop
    fontName := Font.GetName
    exit when fontName = ""
    put fontName
end loop
