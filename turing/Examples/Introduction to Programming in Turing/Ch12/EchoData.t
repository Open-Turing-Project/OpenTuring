% The "EchoData" program
% Read and output names and phone numbers
var name : string (20)
var phone : string (8)
put "Name" : 20, "Phone" : 8
loop
    get skip
    exit when eof
    get name : 20, phone : 8
    put name : 20, phone : 8
end loop
