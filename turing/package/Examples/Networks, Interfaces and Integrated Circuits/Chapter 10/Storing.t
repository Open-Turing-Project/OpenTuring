% The "Storing" program.

% Disables any latch attached to pins 1 to 15 of the demultiplexer since
% setting D0-D3 to 1s outputs highs on the first fifteen outputs.
parallelput (15)

% Sets pin 3 on the second latch to 1.
const dataBit := 1
const selectedPin := 3
% selectedLatch = 0 for the first latch and 1 for the second latch.
const selectedLatch := 1

parallelput (128 * dataBit + 16 * selectedPin + selectedLatch)
