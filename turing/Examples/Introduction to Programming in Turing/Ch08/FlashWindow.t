% The "FlashWindow" program
% Randomly changes the window color
setscreen ("graphics")
var c : int
loop
    randint (c, 0, maxcolor)
    drawfillbox (0, 0, maxx, maxy, c)
    delay (500)
end loop
