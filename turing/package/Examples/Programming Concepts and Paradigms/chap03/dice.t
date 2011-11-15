% File "dice.t".
% Simulate the throw of two dice 300 times
% and store the generated data in file "dice".
var diceFile, die1, die2, throw : int
open : diceFile, "dice", put
assert diceFile > 0
for count : 1 .. 300
        die1 := Rand.Int (1, 6)
        die2 := Rand.Int (1, 6)
        throw := die1 + die2
        put : diceFile, throw
end for
put "Simulated data of 300 dice throws stored in file 'dice' "

