% The "condition" program.
monitor resource
    export request, release

    var available : boolean := true
    var nowAvailable : condition

    procedure request (name : string)
	if available then
	    put "Worker ", name, " needs the pickaxe and it is available"
	else
	    put "Worker ", name, " needs the pickaxe and must wait"
	    wait nowAvailable           % Go to sleep
	end if
	put "Worker ", name, " now has the pickaxe"
	assert available
	available := false              % Allocate resource
    end request

    procedure release (name : string)
	assert not available            % Resource is allocated
	available := true               % Free the resource
	put "Worker ", name, " is now finished with the pickaxe"
	signal nowAvailable             % Wake up one process
	% If any are sleeping
    end release

end resource

process worker (name : string)
    loop
	put "Worker ", name, " is working without the pickaxe"
	delay (Rand.Int (1000, 3000))
	resource.request (name)                % Block until available
	put "Worker ", name, " is using the pickaxe"
	delay (Rand.Int (1000, 3000))
	resource.release (name)
    end loop
end worker

setscreen ("text")
put "Several workers are digging a pit.  Often they need a pickaxe, but"
put "they only brought one.  Thus they must share it.", skip
fork worker ("A")               % Activate one worker
fork worker ("B")               % Activate another worker
fork worker ("C")               % Activate another worker
