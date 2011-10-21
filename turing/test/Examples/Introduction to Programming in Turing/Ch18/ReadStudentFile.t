% The "ReadStudentFile" program
% Reads records from student file into array
% then allows you to read individual records at random
type studentType :
    record
	name : string (20)
	address : string (40)
	phone : string (8)
    end record

const maxFile := 100
var studentFile : array 1 .. maxFile of studentType
% Read records from student file
var students : int
open : students, "Students", get
assert students > 0
var count := 0
loop
    get : students, skip
    exit when eof (students)
    count := count + 1
    get : students, studentFile (count).name : 20,
	studentFile (count).address : 40,
	studentFile (count).phone : 8, skip
end loop
put "There are ", count, " students in file"
var number : int
put "Enter negative record number to exit"
loop
    put "Give number of record you want: " ..
    get number
    exit when number < 0
    assert number <= count
    put studentFile (number).name : 20,
	studentFile (number).address : 40,
	studentFile (number).phone : 8
end loop

