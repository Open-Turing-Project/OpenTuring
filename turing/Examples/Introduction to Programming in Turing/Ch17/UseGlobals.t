% The "UseGlobals" program
% Read names and output list of names in order
put "How many names are there? " ..
var number : int
get number
var name : array 1 .. number of string (40)

procedure readNames
    put "Enter the names"
    for i : 1 .. number
	get name (i)
    end for
end readNames

procedure sortNames
    % Sort list of names alphabetically
    % The sorting algorithm is a Shell sort
    % See if you can follow it
    % It is a modification of the bubble sort
    var space : int := number
    loop
	space := floor (space / 2)
	exit when space <= 0
	for i : space + 1 .. number
	    var j : int := i - space
	    loop
		exit when j <= 0
		if name (j) > name (j + space) then
		    const temp := name (j)
		    name (j) := name (j + space)
		    name (j + space) := temp
		    j := j - space
		else
		    j := 0                         % Signal exit
		end if
	    end loop
	end for
    end loop
end sortNames

procedure outputNames
    % Output sorted list
    put "Here are the names in order"
    for i : 1 .. number
	put name (i)
    end for
end outputNames

readNames
sortNames
outputNames
