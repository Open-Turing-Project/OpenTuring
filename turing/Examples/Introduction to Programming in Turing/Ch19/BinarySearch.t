% The "BinarySearch" program
% Finds a particular record in a sorted file
% using the binary search method
type studentType :
    record
	name : string (30)
	address : string (40)
	phone : string (8)
    end record
const maxFile := 500
var studentFile : array 1 .. maxFile of studentType
var count : int

procedure readFile
    % Uses global variables
    var students : int
    open : students, "SortedStudents", get
    assert students > 0
    count := 0
    loop
	get : students, skip
	exit when eof (students)
	count := count + 1
	get : students, studentFile (count).name : 30,
	    studentFile (count).address : 40,
	    studentFile (count).phone : 8
    end loop
    put "There are ", count, " students in file"
end readFile

procedure search (studentFile : array 1 .. * of studentType,
	var key : string (*), count : int, var place : int)
    var first, last, middle : int
    if length (key) <= 30 then
	% Pad with blanks
	key := key + repeat (" ", 30 - length (key))
    end if
    % Initialize the binary search
    first := 1
    last := count
    % Search until one element is left
    % If key sought is in list this will be it
    loop
	middle := (first + last) div 2
	if studentFile (middle).name >= key then
	    % Discard last half of list
	    last := middle
	else
	    % Discard first half of list including middle
	    first := middle + 1
	end if
	% exit when only one record left
	exit when first >= last
    end loop
    if studentFile (first).name = key then
	place := first
    else
	place := 0
    end if
end search

readFile
var nameSought : string (30)
loop
    var place : int
    put "Enter name of student to be found (\"stop\" to exit)"
    get nameSought : *
    exit when nameSought = "stop"
    search (studentFile, nameSought, count, place)
    if place not= 0 then
	bind sought to studentFile (place)
	put "name" : 30, "address" : 40, "phone" : 8
	put sought.name : 30, sought.address : 40,
	    sought.phone : 8
    else
	put "Not in file"
    end if
end loop
