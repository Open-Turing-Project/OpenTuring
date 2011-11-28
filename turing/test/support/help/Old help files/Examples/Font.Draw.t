% The "Font.Draw" program.
var font1, font2, font3, font4 : int
font1 := Font.New ("serif:12")
assert font1 > 0
font2 := Font.New ("sans serif:18:bold")
assert font2 > 0
font3 := Font.New ("mono:9")
assert font3 > 0
font4 := Font.New ("Arial:24:bold,italic")
assert font4 > 0
Font.Draw ("This is in a serif font", 50, 30, font1, red)
Font.Draw ("This is in a sans serif font", 50, 80, font2, brightblue)
Font.Draw ("This is in a mono font", 50, 130, font3, colorfg)
Font.Draw ("This is in Arial (if available)", 50, 180, font4, green)
Font.Free (font1)
Font.Free (font2)
Font.Free (font3)
Font.Free (font4)
