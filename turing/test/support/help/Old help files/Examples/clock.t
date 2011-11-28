% The "clock" program.
for i : 1 .. 2000
    put i : 5 ..
end for
var timeRunning : int
clock (timeRunning)
put skip, skip, "This program has run ", timeRunning, " milliseconds"
