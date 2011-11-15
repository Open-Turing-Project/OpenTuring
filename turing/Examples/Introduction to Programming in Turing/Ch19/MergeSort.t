% The "MergeSort" program
type studentType :
    record
	name : string (30)
	address : string (40)
	phone : string (8)
    end record
var maxFile := 100
var studentFile : array 1 .. maxFile of studentType
var count : int

procedure readFile
    % Uses global variables
    var students : int
    open : students, "UnsortedStudents", get
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

procedure merge (var studentFile : array 1 .. * of studentType,
	first, middle, last : int)
    % Merges two sorted lists of records one going from
    % first to middle the other from middle + 1 to last
    var temp : array 1 .. last of studentType
    % Initialize pointers to the two sorted lists
    var point1 := first
    var point2 := middle + 1
    for point3 : first .. last
	%  point3 locates item in  merged list
	if point1 < middle + 1 and (point2 > last or
		studentFile (point2).name >
		studentFile (point1).name) then
	    % Move record from first half
	    temp (point3) := studentFile (point1)
	    % Advance pointer 1
	    point1 := point1 + 1
	else
	    % Move record from second half
	    temp (point3) := studentFile (point2)
	    % Advance pointer 2
	    point2 := point2 + 1
	end if
    end for
    % Copy merged array back to original place
    for point3 : first .. last
	studentFile (point3) := temp (point3)
    end for
end merge

procedure mergesort (var studentFile : array 1 .. * of studentType,
	first, last : int)
    if last > first then
	const middle := (first + last) div 2
	mergesort (studentFile, first, middle)
	mergesort (studentFile, middle + 1, last)
	merge (studentFile, first, middle, last)
    end if
end mergesort

procedure outFile
    % Uses global variables
    % Output file in order
    put "name" : 30, "address" : 40, "phone" : 8
    for i : 1 .. count
	put studentFile (i).name : 30,
	    studentFile (i).address : 40,
	    studentFile (i).phone : 8
    end for
end outFile

% This is the main program
readFile
mergesort (studentFile, 1, count)
outFile
