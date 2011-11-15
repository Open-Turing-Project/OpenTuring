% The "StoringData" program.
parallelput (15)
% Disables any latch attached to pins 1 to 15 of the demultiplexer.
% 48 in binary is 0110000.  The 0000 would set pin 1 of
% the 74LS154 low. This enables the first latch.
%  The 011 would address Q3 on latch 1.
parallelput (48 + 128 * 1)
% The pin addressed now is sent a high through D7.
