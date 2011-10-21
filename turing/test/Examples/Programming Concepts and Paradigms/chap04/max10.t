% File "max10.t".
% Read a maximum of 10 positive integers.
% Stop immediately if -1 is entered.
var number : int
var howMany := 0
put "Enter 10 marks, or fewer ended by -1"
for count : 1 .. 10
    get number
    exit when number = - 1
    howMany := count
    put number
end for
put "There were ", howMany, " numbers read"
