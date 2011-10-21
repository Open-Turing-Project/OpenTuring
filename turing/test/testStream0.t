var f : int
for i : 1 .. 100
    put i
    put : 0, "* ", i
    if i mod 5 = 0 then
	open : f, "junk.dat", put
    else
	open : f, "xyaxbvsd", get
    end if
    delay (1000)
end for
