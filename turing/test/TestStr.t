var src : char (1000)
var dest : char (1000)

var pos := 1
Str.WriteToCharArray (src, "Hello", pos)
Str.WriteToCharArray (src, "Hello", pos)
Str.WriteToCharArray (src, "Hello", pos)
Str.WriteToCharArray (src, "Hello", pos)
put src (1 .. pos - 1)

pos := 1
Str.WriteLineToCharArray (src, "Hello", pos)
Str.WriteLineToCharArray (src, "Hello", pos)
Str.WriteLineToCharArray (src, "Hello", pos)
Str.WriteLineToCharArray (src, "Hello", pos)
put src (1 .. pos - 1)

Str.CharArrayCopy (src, 10, dest, 1, pos - 10)
put dest (1 .. pos - 10)
