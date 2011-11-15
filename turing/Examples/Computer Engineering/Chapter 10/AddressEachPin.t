% The "AddressEachPin" program.
% Count addresses for each output pin.
for count : 0 .. 15
    % Outputs the equivalent binary number.
    parallelput (count)
    % Delays for half of a second.
    delay (500)
end for
