% The "MusicBox" program.
var value : int
% Infinite loop
loop
    % This sets each bit in value to correspond to
    % one of the input pins.  I0 is bit 3, I1 is bit 4,
    % I2 is bit 2, I3 is bit 1, and I4 is bit 0.
    value := parallelget div 8
    if value = 2#10111 then
	% Push button 1 is pressed. (I0 (bit 3) is low.)
	play ("4C")
    elsif value = 2#01111 then
	% Push button 2 is pressed. (I1 (bit 4) is low.)
	play ("4D")
    elsif value = 2#11011 then
	% Push button 3 is pressed. (I2 (bit 2) is low.)
	play ("4E")
    elsif value = 2#11101 then
	% Push button 4 is pressed. (I3 (bit 1) is low.)
	play ("4F")
    elsif value = 2#11110 then
	% Push button 5 is pressed. (I4 (bit 0) is low.)
	play ("4G")
    elsif value = 2#00111 then
	% Push button 6 is pressed. (I0 (bit 3) and
	% I1 (bit 4) are low.)
	play ("4A")
    elsif value = 2#10011 then
	% Push button 7 is pressed. (I0 (bit 3) and
	% I2 (bit 2) are low.)
	play ("4B")
    elsif value = 2#10101 then
	% Push button 8 is pressed. (I0 (bit 3) and
	% I3 (bit 1) are low.)
	play (">4C<")
    end if
    % Endlessly scans keyboard.
end loop
