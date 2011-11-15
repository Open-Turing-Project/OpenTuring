% File "funcvar.t".  Program using a function with a var parameter.

% Function with var parameter.
function g (var x : int) : int
    x += 1
    result x
end g

var v := 10
const w := g (v)
put 2 * w : 5, w + g (v) : 5
