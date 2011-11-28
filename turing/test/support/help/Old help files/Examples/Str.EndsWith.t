% The "Str.EndsWith" program.
var line : string
loop
    get line : *
    if Str.EndsWith (line, "ere") then
	put "'", line, "' ends with 'ere'"
    else
	put "'", line, "' does not end with 'ere'"
    end if
end loop
