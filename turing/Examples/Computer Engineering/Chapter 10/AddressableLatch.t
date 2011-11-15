% The "AddressableLatch" program.
procedure clear
    for i : 0 .. 7
	parallelput (i)
    end for
end clear

var address, data : int

clear

loop
    put "Enter the pin to be addressed (0 to 7, 8 to exit): " ..
    get address
    exit when address = 8
    put "Set pin Q", address, " high or low (1 or 0): " ..
    get data
    % Disables output.
    parallelput (16)
    % Data is sent to that address.  If data = 0, then data * 8 = 0.
    % Therefore, D receives a 0.
    % If data = 1, then data * 8 = 8.
    % Therefore, D, which is connected to D3, receives a 1.
    parallelput (address + data * 8)
end loop
clear
parallelput (0)
