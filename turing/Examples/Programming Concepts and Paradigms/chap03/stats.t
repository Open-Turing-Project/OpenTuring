% File "stats.t".
% Compute the mean and variance for the simulated dice throws
% in file "dice" prepared by the dice throw generation program.
var throw : int
var count, sum, sumOfSquares := 0
var diceFile : int
open : diceFile, "dice", get
assert diceFile > 0
loop
	exit when eof (diceFile)
	get : diceFile, throw
	count := count + 1
	sum := sum + throw
	sumOfSquares := sumOfSquares + throw ** 2
end loop
const average := sum / count
const variance := sumOfSquares / count - average ** 2
put "Average = ", average : 4 : 1, " Variance =", variance : 4 : 1
