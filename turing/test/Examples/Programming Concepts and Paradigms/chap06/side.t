% File "side.t".  Program using a function with a side effect.
var y := 0

% Function with side effect.
function f (x : int) : int
    y += 1
    result x + y
end f

const z := f (10)
put 2 * z : 5, z + f (10) : 5
