% The "Time.SecStr2" program.
setscreen ("text")
var formatLetters : array 1 .. * of char :=
    init ('a', 'A', 'b', 'B', 'c', 'd', 'H', 'I', 'j', 'm', 'M',
    'p', 'S', 'U', 'w', 'W', 'x', 'X', 'y', 'Y', 'z', 'Z')
for i : 1 .. upper (formatLetters)
    put formatLetters (i), ": ", Time.SecStr (Time.Sec, formatLetters (i))
end for
