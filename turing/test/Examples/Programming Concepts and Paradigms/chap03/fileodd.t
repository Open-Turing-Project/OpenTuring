% File "fileodd.t".
% Produce a file of data on disk consisting of the odd integers
% from 1 to 361 inclusive.
var streamOut : int
var fileName : string
put "What is the name of the file for the integers? "
get fileName
open : streamOut, fileName, put
assert streamOut > 0
for number : 1 .. 361 by 2
    put : streamOut, number
end for
close : streamOut
put "File of odd numbers is now stored on file ", fileName
