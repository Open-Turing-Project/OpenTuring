% The "EightBitLatch" program.
setscreen ("graphics")

% Sets D0 to D7 low. Includes the CLR on D5 which is set low
% to clear all output.
parallelput (0)

var address, data : int

loop
    cls
    locate (1, 30)
    colour (red)
    put "Eight Bit Latch "
    colour (black)
    locate (3, 20)
    put "Enter the number of the LED to be activated."
    locate (5, 28)
    put "(0 to 7, 8 will exit): " ..
    get address
    exit when address = 8
    locate (8, 25)
    put "Set LED ", address, " high (1) or low (0): " ..
    get data

    % Clears the eight bit latch.
    % The 32 sets D5 high which sets CLR on the latch high.
    % A high CLR allows data to be output.
    % The 16 sets D4 high which disables output.
    % The output needs to be disabled until the address and data are set up.
    parallelput (32 + 16)

    % The address goes from 0 to 7 (000 to 111 in binary) on D0 to D2.
    % The data (either a 0 or a 1) is on D3, hence the data is multiplied by 8.
    % The CLR on D5 has a value of 32. It must be set high to allow
    % data to be output.
    % The D4 is set low and thus enables output.
    parallelput (address + data * 8 + 32)
end loop
