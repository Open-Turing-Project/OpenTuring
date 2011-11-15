% The "AddressBook" program
% Creates and maintains an addressbook of names and addresses.

% The type of each record in the file
type entryType :
    record
	name : string
	address : string
    end record

% Calculate the record size
const recordSize : int := sizeof (entryType)
const maxEntries : int := 100
const bookFileName : string := "address.dat"

var numNames : int % The number of names in the file

% The array stores the names in the same order as
% those in the records in the disk file.
var names : array 1 .. maxEntries of string

% Read records in the file, initializing the
% array of names and determining the number of records.
procedure readNames
    var f : int
    var entry : entryType
    numNames := 0
    open : f, bookFileName, read
    if f not= 0 then
	loop
	    exit when eof (f)
	    read : f, entry
	    numNames := numNames + 1
	    names (numNames) := entry.name
	end loop
	close : f
    end if
    put numNames, " names read in"
end readNames

% Find a matching name in the array of names, returning
% -1 if no matching name is found.
function findName (name : string) : int
    for i : 1 .. numNames
	if name = names (i) then
	    result i
	end if
    end for
    result - 1
end findName

% Output the address associated with the name.
procedure lookUpName (name : string)
    var recordNumber : int := findName (name)
    var f : int
    var entry : entryType
    if recordNumber = - 1 then
	put name, " not in address book"
    else
	% Open the file, seek to the beginning of the
	% record and read the entry.
	open : f, bookFileName, read, seek
	assert f > 0
	seek : f, (recordNumber - 1) * recordSize
	read : f, entry
	put "Address: ", entry.address
	close : f
    end if
end lookUpName

% Change the address associated with the name
procedure changeAddress (name : string)
    var recordNumber : int := findName (name)
    var entry : entryType
    var f : int

    if recordNumber = - 1 then
	put name, " not in address book"
    else
	% Open the file, seek to the beginning of the
	% record and read the entry.
	open : f, bookFileName, read, write, mod, seek
	assert f > 0
	seek : f, (recordNumber - 1) * recordSize
	read : f, entry
	% Now get the new address, seek back to the
	% beginning of the record and write the new entry.
	put "Old Address: ", entry.address
	put "New Address: " ..
	get entry.address : *
	seek : f, (recordNumber - 1) * recordSize
	write : f, entry
	close : f
    end if
end changeAddress

% Add an entry to the end of the array and to the end of
% the file.
procedure addEntry (name, address : string)
    var f : int
    var entry : entryType
    % Add to the array of entries
    numNames := numNames + 1
    names (numNames) := name
    % Add the record to the file
    entry.name := name
    entry.address := address
    % Open the file, seek to the end of the file and
    % write the record.
    open : f, bookFileName, write, mod, seek
    assert f > 0
    seek : f, *
    write : f, entry
    close : f
end addEntry

% We do not implement this yet. However, we do leave
% a place for it to make it easy to add to this program
% later.
procedure deleteEntry (name : string)
    put "Deletion is not yet supported"
end deleteEntry

% Main Program
var name, address : string
var choice : int

% Read the names in the array
readNames

loop
    % Get the user's choice
    put "Enter 1 .. Look up a name"
    put "      2 .. Change an address"
    put "      3 .. Add a name"
    put "      4 .. List all names"
    put "      5 .. Delete a Name"
    put "      6 .. Exit"
    put "Choice: " ..
    get choice

    if choice = 1 then
	% Look up a name.
	put "Enter the name: " ..
	get name : *
	lookUpName (name)
    elsif choice = 2 then
	% Change the address associated with a name.
	put "Enter the name: " ..
	get name : *
	changeAddress (name)
    elsif choice = 3 then
	% Add an entry to the address book.
	put "Enter the new name: " ..
	get name : *
	put "Enter the address: " ..
	get address : *
	addEntry (name, address)
    elsif choice = 4 then
	% List all the names in the address book.
	put "Names in address book"
	for i : 1 .. numNames
	    put names (i)
	end for
    elsif choice = 5 then
	% Delete an entry in the address book.
	put "Enter the name: " ..
	get name : *
	deleteEntry (name)
    elsif choice = 6 then
	% Exit the loop
	exit
    else
	% Invalid choice
	put "Choice must be between 1 and 6"
    end if
end loop
