% The "SortArray" program
% Reads a list of positive integers and
% sorts it into ascending order
var count : int
put "How many integers in list? " ..
get count
var list : array 1 .. count of int
% Read list into array
put "Enter the numbers in the list"
for i : 1 .. count
    get list (i)
end for
% Declare second array for sorted list
var sortList : array 1 .. count of int
for i : 1 .. count
    % Find smallest remaining in list
    var smallest := 999
    var where : int
    for j : 1 .. count
	if list (j) < smallest then
	    smallest := list (j)
	    where := j
	end if
    end for
    % Store smallest in sortList array
    sortList (i) := smallest
    % Replace smallest in array list by 999
    list (where) := 999
end for
% Output sorted list
put "Here are the numbers in ascending order"
for i : 1 .. count
    put sortList (i)
end for
