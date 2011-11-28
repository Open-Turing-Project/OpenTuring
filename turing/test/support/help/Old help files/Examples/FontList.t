% The "FontList" program.

setscreen ("text")

var fontName : string
var bold, italic, underline : boolean
var size : int
var styles : array boolean, boolean, boolean of string :=
    init ("", "underline", "italic", "italic,underline", "bold",
    "bold,underline", "bold,italic", "bold,italic,underline")

% Start the font listing    
Font.StartName
loop
    % Get the font name
    fontName := Font.GetName
    exit when fontName = ""
    % Get the styles the font comes in
    Font.GetStyle (fontName, bold, italic, underline)
    % For each possible style (bold, italic, underline & combo thereof)
    for b : false .. bold
	for i : false .. italic
	    for u : false .. underline
		put fontName : 30, styles (b, i, u) : 22 ..
		% Start the listing of the font sizes
		Font.StartSize (fontName, styles (b, i, u))
		loop
		    % List each font size. A size of -1 means the font
		    % is scalable to pretty much any size
		    size := Font.GetSize
		    exit when size = 0;
		    if size = -1 then
			put "scalable" ..
		    else
			put size, " " ..
		    end if
		end loop
		put ""
	    end for
	end for
    end for
end loop
