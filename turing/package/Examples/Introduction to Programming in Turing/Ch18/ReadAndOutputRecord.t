% The "ReadAndOutputRecord" program
% Reads a record and outputs it
var student :
    record
	name : string (20)
	address : string (40)
	phone : string (8)
    end record
loop
    % Read in record
    put "Enter name"
    exit when eof
    get student.name : *
    put "Enter address"
    get student.address : *
    put "Enter phone"
    get student.phone : *
    % Output record
    put student.name : 20, student.address : 40,
	student.phone : 8
end loop
