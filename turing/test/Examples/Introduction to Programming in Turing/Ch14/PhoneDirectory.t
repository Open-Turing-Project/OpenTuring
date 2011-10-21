% The "PhoneDirectory" program
% Reads a phone directory
% then looks up numbers in it
const maxEntries := 50
var name : array 1 .. maxEntries of string (20)
var phoneNumber : array 1 .. maxEntries of string (8)
var count : int := 0
% Assign a stream number to directory
var directory : int
open : directory, "Direct", get
assert directory > 0
loop
    get : directory, skip
    exit when eof (directory)
    count := count + 1
    assert count <= maxEntries
    get : directory, name (count) : 20,
	phoneNumber (count) : 8
    % All names will be of length 20
end loop
close : directory
put "There are ", count, " entries in directory"
var friend : string
loop
    put "Enter friend's name: " ..
    exit when eof
    get friend : *
    if length (friend) < 20 then
	% Pad with blanks to make string friend
	% of length 20 because strings must have
	% same length to be equal
	friend := friend + repeat (" ", 20 - length (friend))
    end if
    var place : int := 0
    % Search list for name
    for i : 1 .. count
	if name (i) = friend then
	    place := i
	    exit
	end if
    end for
    if place not= 0 then
	put "Phone number is ", phoneNumber (place)
    else
	put "Not in list"
    end if
end loop
