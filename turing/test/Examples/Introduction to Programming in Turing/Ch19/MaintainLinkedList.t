% The "MaintainLinkedList" program
% Allows insertions and deletions to list
% Permits listing in order
type labelType :
    record
	name : string (20)
	address : string (25)
	link : int
    end record
const null := 0
% Avoid using keyword label
const maxLabels := 100
var labels : array 1 .. maxLabels of labelType
% Initialize pointers to beginning of linked lists
% The label record list is empty
var first := null
% Initialize links of vacant array
var vacant : int := 1
% Each vacant record in array points to next
for i : 1 .. maxLabels - 1
    labels (i).link := i + 1
end for
% Last record in array has null link
labels (maxLabels).link := null

procedure insert (newLabel : labelType)
    % Uses global variables
    % Insert a new label record
    % Obtain a vacant space for new label record
    assert vacant not= null
    var spot := vacant
    vacant := labels (vacant).link
    % Place new label record in vacant space
    labels (spot).name := newLabel.name
    labels (spot).address := newLabel.address
    % See if new label goes first in list
    if first = null or newLabel.name < labels (first).
	    name then
	labels (spot).link := first
	first := spot
    else
	% Find place to insert new label record
	var previous := first
	var next := labels (first).link
	loop
	    exit when next = null or newLabel.name < labels (next).name
	    previous := next
	    next := labels (next).link
	end loop
	% Fix links to make insertion
	labels (previous).link := spot
	labels (spot).link := next
    end if
end insert

procedure delete (oldLabel : labelType)
    % Uses global variables
    % Find label to be deleted from linked list
    var old : int := first
    var previous : int
    loop
	exit when old = null or labels (old).name = oldLabel.name
	previous := old
	old := labels (old).link
    end loop
    % Remove label record from linked list
    if old = null then
	put oldLabel.name, " not found"
	return
    elsif first = old then
	first := labels (old).link
    else
	labels (previous).link := labels (old).link
    end if
    % Return unused record to vacant list
    labels (old).link := vacant
    vacant := old
end delete

procedure outputList
    % Output linked list in order
    var current := first
    loop
	exit when current = null
	put labels (current).name : 20,
	    labels (current).address : 25
	current := labels (current).link
    end loop
end outputList

% Respond to commands
var newLabel, oldLabel : labelType
var reply : int
const inserts := 1
const deletes := 2
const lists := 3
const stops := 4
loop
    put "Do you want to 1 - insert, 2 - delete, 3 - list, 4 - stop? " ..
    get reply
    case reply of
	label inserts :
	    put "Enter name of label to be inserted"
	    get newLabel.name
	    put "Enter address"
	    get newLabel.address
	    insert (newLabel)
	label deletes :
	    put "Enter name of record to be deleted"
	    get oldLabel.name
	    delete (oldLabel)
	label lists :
	    outputList
	label stops :
	    exit
	label :
	    put "Bad command, try again"
    end case
end loop
