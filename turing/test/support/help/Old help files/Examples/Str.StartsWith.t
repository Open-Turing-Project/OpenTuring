% The "Str.StartsWith" program.
var line : string
loop
    get line : *
    if Str.StartsWith (line, "the") then
	put "'", line, "' starts with 'the'"
    else
	put "'", line, "' does not start with 'the'"
    end if
end loop
