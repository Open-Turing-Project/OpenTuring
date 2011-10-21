% The "BadMoviePrice" program.
% Reads age and incorrectly gives ticket price
var age : int
loop
    put "Please enter your age " ..
    get age
    assert age > 0
    if age >= 14 then
	put "Please pay $6.00."
	put "You are an adult."
    elsif age >= 65 then
	put "Please pay $3.00."
	put "You are a senior."
    else
	put "Please pay $2.50."
	put "You are a child."
    end if
end loop
