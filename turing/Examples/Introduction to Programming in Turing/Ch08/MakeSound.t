% The "MakeSound" program
% Plays a rising then falling frequency along with colorful graphics
for i : 0 .. maxy
    drawfillbox (0, 0, i * 2, maxy - i, i mod 16)
    sound (i * 2 + 200, 50)
end for
for decreasing i : maxy .. 0
    drawfilloval (0, 0, i * 2, maxy - i, i mod 16)
    sound (i * 2 + 200, 50)
end for
