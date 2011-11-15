% The "SortNames" program
% Reads, sorts, and outputs a list of 10 names

procedure readList (var people : array 1 .. 10 of string (30))
    % Read 10 names
    put "Enter 10 names one to a line"
    for i : 1 .. 10
	get people (i)
    end for
end readList

procedure bubble (var list : array 1 .. 10 of string (30))
    % Sort list of 10 names into alphabetic order
    for decreasing last : 10 .. 2
	var sorted : boolean := true
	% If there is any swapping, list is not sorted
	% Swaps elements so largest in last
	for i : 1 .. last - 1
	    if list (i) > list (i + 1) then
		sorted := false
		% swap elements in list
		const temp := list (i)
		list (i) := list (i + 1)
		list (i + 1) := temp
	    end if
	end for
	exit when sorted
    end for
end bubble

procedure writeList (name : array 1 .. 10 of string (30))
    % Output 10 names
    put "Here is sorted list"
    for i : 1 .. 10
	put name (i)
    end for
end writeList

var friend : array 1 .. 10 of string (30)
readList (friend)
bubble (friend)
writeList (friend)
