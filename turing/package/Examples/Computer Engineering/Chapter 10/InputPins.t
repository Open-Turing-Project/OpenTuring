% The "InputPins" program.

% The values related to each input pin. These numbers change from computer % to computer.
%      I0 (pin 10)     64
%      I1 (pin 11)    128
%      I2 (pin 12)     32
%      I3 (pin 13)     16
%      I4 (pin 15)      8
loop
    var value : int := parallelget
    put "I0: ", (value div 64) mod 2 ..
    put "  I1: ", (value div 128) mod 2 ..
    put "  I2: ", (value div 32) mod 2 ..
    put "  I3: ", (value div 16) mod 2 ..
    put "  I4: ", (value div 8) mod 2
end loop
