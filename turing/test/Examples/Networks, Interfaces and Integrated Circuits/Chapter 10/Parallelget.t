% The "Parallelget" program.

% The values related to each input pin. These numbers change from computer % to computer.
%      I0 (pin 10)     64
%      I1 (pin 11)    128
%      I2 (pin 12)     32
%      I3 (pin 13)     16
%      I4 (pin 15)      8
loop
    var value : int := parallelget
    if (value div 64) mod 2 = 1 then
	put "I0 (pin 10)    " ..
    end if
    if (value div 128) mod 2 = 1 then
	put "I1 (pin 11)    " ..
    end if
    if (value div 32) mod 2 = 1 then
	put "I2 (pin 12)    " ..
    end if
    if (value div 16) mod 2 = 1 then
	put "I3 (pin 13)    " ..
    end if
    if (value div 8) mod 2 = 1 then
	put "I4 (pin 15)    " ..
    end if
    put ""
end loop
