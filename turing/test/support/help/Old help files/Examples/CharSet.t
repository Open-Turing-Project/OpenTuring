% The "CharSet" program.
put "ASCII values from 32 to 127", skip
for i : 32 .. 47
    for j : 0 .. 80 by 16
	color (red)
	put i + j : 3, "  " ..
	color (black)
	put chr (i + j), "    " ..
    end for
    put ""
end for
put skip, "Press any key to see values from 128 to 255 " ..
Input.Pause
cls
put "ASCII values from 128 to 255", skip
for i : 128 .. 143
    for j : 0 .. 112 by 16
	color (red)
	put i + j : 3, "  " ..
	color (black)
	put chr (i + j), "    " ..
    end for
    put ""
end for
put skip, "Press any key to see MSDOS characters for values from 128 to 255 " ..
Input.Pause
cls
put "MS-DOS character set values from 128 to 255", skip
setscreen ("msdos")
for i : 128 .. 143
    for j : 0 .. 112 by 16
	color (red)
	put i + j : 3, "  " ..
	color (black)
	put chr (i + j), "    " ..
    end for
    put ""
end for
