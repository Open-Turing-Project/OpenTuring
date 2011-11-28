% The "hasch" program
put "Press any key to stop the dice rolling"
var die1, die2 : int
loop
    randint (die1, 1, 6)
    randint (die2, 1, 6)
    locate (1, 1)
    put "You rolled ", die1 + die2
    exit when hasch
end loop
