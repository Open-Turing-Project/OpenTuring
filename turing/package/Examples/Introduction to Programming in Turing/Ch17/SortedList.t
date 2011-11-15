% The "SortedList" program
% Maintains a sorted list of names

% Add a name to the sorted list in the correct position
procedure AddName (name : string, var list : array 1 .. * of string (*),
	var listSize : int)
    % The array index where the name should be placed
    var insertPoint : int := 1

    % Determine the location in the array where the name should be
    % placed
    loop
	exit when insertPoint > listSize or list (insertPoint) > name
	insertPoint := insertPoint + 1
    end loop

    % Shift the remaining items over to make room at insertPoint
    for decreasing i : listSize .. insertPoint
	list (i + 1) := list (i)
    end for

    % Insert the name at insertPoint
    list (insertPoint) := name

    listSize := listSize + 1

    put name, " added. The list now has ", listSize, " name(s)"
end AddName

% Delete a name in the sorted list
procedure DeleteName (name : string, var list : array 1 .. * of string (*),
	var listSize : int)
    % The array index holding the name to be deleted
    var deletePoint : int := 1

    % Determine the location in the array of the name to be deleted
    loop
	exit when deletePoint >= listSize or list (deletePoint) >= name
	deletePoint := deletePoint + 1
    end loop

    % Make certain the name was found in the list
    if list (deletePoint) not= name then
	put name + " not found in the list"
	return
    end if

    % Shift the remaining items over to remove the element at
    % deletePoint
    for i : deletePoint + 1 .. listSize
	list (i - 1) := list (i)
    end for

    listSize := listSize - 1

    put name, " deleted. The list now has ", listSize, " name(s)"
end DeleteName

% Display the sorted list of names
procedure ListNames (list : array 1 .. * of string (*), listSize : int)
    for i : 1 .. listSize
	put i : 3, " ", list (i)
    end for
end ListNames

% Main program
var listOfNames : array 1 .. 100 of string (40)
var sizeOfList : int := 0
var choice : int
var name : string (40)
loop
    put "What is your command:"
    put " (1) Add a name"
    put " (2) Delete a name"
    put " (3) List the names"
    put " (4) Exit"
    put "Choice: " ..
    get choice
    if choice = 1 then
	put "Enter the name to be added: " ..
	get name : *
	AddName (name, listOfNames, sizeOfList)
    elsif choice = 2 then
	put "Enter the name to be deleted: " ..
	get name : *
	DeleteName (name, listOfNames, sizeOfList)
    elsif choice = 3 then
	ListNames (listOfNames, sizeOfList)
    elsif choice = 4 then
	exit
    else
	put "Illegal command"
    end if
end loop
