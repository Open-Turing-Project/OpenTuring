% The "LinearAddressing3" program.
% Counter will trigger each of the thirty-two lights.
for counter : 0 .. 31
    parallelput (counter)
    delay (500)
end for
