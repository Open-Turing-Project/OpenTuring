% The "color2" program.
setscreen ("graphics")
const message := "Happy New Year!!"
for i : 1 .. length (message)
    color (i mod maxcolor + 1)
    put message (i) ..
end for
