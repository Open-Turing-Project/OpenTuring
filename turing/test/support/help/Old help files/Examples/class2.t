% The "class2" program.
class C
    export f, g

    procedure f
	put "C's f"
    end f

    procedure g
	put "C's g"
    end g
end C

class D
    inherit C           % Inherit f and g

    body procedure g    % Overrides g in C
	put "*** D's g ***"
    end g

    procedure h
	put "*** D's h ***"
    end h
end D
var p : pointer to C    % p can point to any descendant of C
new D, p                % p locates an object of class D
p -> f                  % Outputs "C's f"
p -> g                  % Outputs "*** D's g ***"
p -> h                  % Causes error "'h' is not in export list of 'C'"
			% Comment out previous line and rerun
