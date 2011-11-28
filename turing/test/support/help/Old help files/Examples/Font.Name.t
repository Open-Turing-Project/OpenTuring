% The "Font.Name" program.
var serifFont, sansSerifFont, monoFont : int
serifFont := Font.New ("serif:12")
assert serifFont > 0
sansSerifFont := Font.New ("sans serif:12")
assert sansSerifFont > 0
monoFont := Font.New ("mono:12")
assert monoFont > 0
put "serif = ", Font.Name (serifFont)
put "sans serif = ", Font.Name (sansSerifFont)
put "mono = ", Font.Name (monoFont)
Font.Free (serifFont)
Font.Free (sansSerifFont)
Font.Free (monoFont)
