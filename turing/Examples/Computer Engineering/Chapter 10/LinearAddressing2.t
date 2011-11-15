% The "LinearAddressing2" program.
for counter : 0 .. 15
    parallelput (counter)
    delay (500)
end for
% The first demultiplexer is enabled and all output pins
% will be activated one at a time.
for counter : 0 .. 15
    parallelput (16 + counter)
    delay (500)
end for
% When counter is 16, E0 on demultiplexer 1 went high.
% This disabled the first demultiplexer.
% 16 also sent a low to E0 on the second demultiplexer (after the NOT gate).
% The second demultiplexer is enabled and output pin 1 on the
% second demultiplexer will light.
% Similarly all sixteen outputs on the second demultiplexers will light.
