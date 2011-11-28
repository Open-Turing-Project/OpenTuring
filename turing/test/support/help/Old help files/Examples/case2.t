% The "case2" program.
var name : string
put "Enter the name of an animal: " ..
get name
case name of
    label "horse", "cow" :
	put "Farm animal"
    label "tiger", "lion" :
	put "Jungle animal"
    label "cat", "dog" :
	put "Pet"
    label :
	put "Unknown animal"
end case
