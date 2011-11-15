% The "RemoveDuplicates" program
% Reads a series of names and eliminates duplicates
var nameCount : int
put "Enter number of names " ..
get nameCount
put "Enter a series of ", nameCount, " names"
var nameList : array 1 .. nameCount of string (30)
var arrayCount := 1
for i : 1 .. nameCount
    get nameList (arrayCount) : *
    var found : boolean := false
    for j : 1 .. arrayCount - 1
	if nameList (arrayCount) = nameList (j) then
	    found := true
	    exit
	end if
    end for
    if not found then
	arrayCount := arrayCount + 1
    end if
end for
% Output list without duplicates
put "Here are the names without duplicates"
for i : 1 .. arrayCount - 1
    put nameList (i)
end for
