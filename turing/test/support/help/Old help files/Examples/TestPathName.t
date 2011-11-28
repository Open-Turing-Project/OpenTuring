% The "TestPathName" program.
setscreen ("text")
var pathName, parent : string
put "Enter a path: " ..
get pathName

pathName := File.FullPath (pathName)    % Obtain the full pathname of the file

if File.Exists (pathName) then          % Output if file exists or not
    put pathName, " exists"
else
    put pathName, " does not exist"
end if

parent := File.Parent (pathName)        % Obtain the parent directory
loop
    
    if Dir.Exists (parent) then         % Output if it exists or not
	put parent, " exists"
    else
	put parent, " does not exist"
    end if
    parent := File.Parent (parent)      % Obtain the parent directory
    exit when Error.Last not= eNoError  % If there is no parent, then exit
end loop
