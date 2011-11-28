% The "char2" program.
var frequency : array 'A' .. 'Z' of nat
for d : 'A' .. 'Z'
    frequency (d) := 0
end for

setscreen ("text")
put "Enter upper case text ending with a Ctrl+Z or Ctrl+D"

loop                        % Tabulate use of capital letters
    exit when eof
    var c : char
    get c                   % Read one character
    if c >= 'A' and c <= 'Z' then
	frequency (c) := frequency (c) + 1
    end if
end loop
for d : 'A' .. 'Z'          % Print frequency of capital letters
    put d, " ", frequency (d)
end for
