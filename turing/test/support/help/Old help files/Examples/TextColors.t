% The "TextColors" program.
setscreen ("graphics")
for row : 0 .. 15
    colorback (row)
    for column : 0 .. 15
	color (column)
	locate (row + 1, column * 5 + 1)
	put "Test " ..
    end for
end for
