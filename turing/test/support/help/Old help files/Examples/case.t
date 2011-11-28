% The "case" program.
var mark : int
put "Enter a mark from 1 - 10: " ..
get mark
case mark of
    label 9, 10 :
	put "Excellent"
    label 7, 8 :
	put "Good"
    label 6 :
	put "Fair"
    label :
	put "Poor"
end case
