var f : int
setscreen ("text")
var word, riding, ridingName, line, candidateName, party : string
open : f, "ridings.txt", get
loop
    exit when eof (f)
    get : f, word
    if word = "RIDING" then
	get : f, riding
	if strint (riding) > 100 then
	    for cnt : 101 .. 183
		put cnt : 3, " CR 100"
	    end for
	    return
	end if
	get : f, ridingName
	get : f, line : *
    elsif word = "CANDIDATE" then
	get : f, candidateName
	get : f, party
	get : f, line : *
	put riding : 3, " CA ", party : 3, "   ", candidateName
    else
	get : f, line : *
    end if
end loop
