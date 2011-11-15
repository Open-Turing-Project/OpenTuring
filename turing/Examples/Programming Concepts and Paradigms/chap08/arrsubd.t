% File "arrsubd.t". 
% A program that uses a procedure with an array parameter.
const maxSize := 10
var list : array 1 .. maxSize of int

% Initialize list to random values in range 1 to 10.
procedure initialize (var a : array 1 .. maxSize of int)
    for i : 1 .. maxSize
        a (i) := Rand.Int (1, 10)
    end for
end initialize

include "display1.ti"

initialize (list)
printArray (list, 5)
