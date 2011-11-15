% The "GetTwoAges" program.
procedure getInt (var number : int)
    var input : string
    loop
	get input
	exit when strintok (input)
	put "Not a number. Please enter an integer: " ..
    end loop
    number := strint (input)
end getInt

var age1, age2 : int
put "Enter the age of the first person: " ..
getInt (age1)
put "Enter the age of the second person: " ..
getInt (age2)
if age1 > age2 then
    put "The first person is older."
else
    put "The second person is older."
end if
