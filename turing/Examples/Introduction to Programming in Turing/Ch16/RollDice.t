% The "RollDice" program
% Simulate a sequence of dice throws
var die1, die2 : int
loop
    put "Do you want to roll (y or n)? " ..
    var reply : string (1)
    get reply
    if reply = "y" then
	randint (die1, 1, 6)
	randint (die2, 1, 6)
	put "You rolled ", die1 + die2
    elsif reply = "n" then
	exit
    else
	put "Reply y or n"
    end if
end loop
