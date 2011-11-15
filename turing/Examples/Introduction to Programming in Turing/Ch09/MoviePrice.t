% The "MoviePrice" program
% Reads age and gives ticket price
var age : int
loop
    put "Please enter your age " ..
    get age
    assert age > 0
    if age >= 65 then
	put "Please pay $3.00"
	put "You are a senior"
    elsif age >= 14 then
	put "Please pay $6.00"
	put "You are an adult"
    else
	put "Please pay $2.50"
	put "You are a child"
    end if
end loop
