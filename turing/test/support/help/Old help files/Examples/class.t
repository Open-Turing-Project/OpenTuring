% The "class" program.
class stackClass    % Template for creating individual stacks
    export push, pop

    var top : int := 0
    var contents : array 1 .. 100 of string

    procedure push (s : string)
	top := top + 1
	contents (top) := s
    end push

    function pop : string
	var s : string
	s := contents (top)
	top := top - 1
	result s
    end pop
end stackClass

var p : pointer to stackClass   % Short form: var p: ^stackClass
var name : string
new stackClass, p               % Short form: new p
put "Pushing names onto the stack", skip
for i : 1 .. 4
    put "Enter name #", i, " of 4: " ..
    get name
    put "Pushing '", name, "' onto the stack"
    p -> push (name)
end for

put skip, "Popping names off the stack", skip
for i : 1 .. 4
    name := p -> pop            % This sets name to be Harvey
    put "Popping '", name, "' off the stack"
end for
