% File "randtest.t".
% Test the random number generator for
% producing integers between 1 and 31.
type randSet : set of 1 .. 31
var complete : randSet := randSet (all) % Complete set.
% Initialize trials to null set in declaration.
var trials : randSet := randSet () % Null set.
var number : int
% Generate 100 trial random integers.
for : 1 .. 100
    number := Rand.Int (1, 31)
    % Add number to set of trials.
    trials := trials + randSet (number)
end for
% Find set of integers not produced in trials.
const notInSet : randSet := complete - trials
% Output these integers.
put "Values not produced in trials"
var cardinality := 0
for value : 1 .. 31
    if value in notInSet then
        put value, " " ..
        cardinality := cardinality + 1
    end if
end for
put " "
put "There were ", cardinality, " members of the ",
    "complete set not produced"
