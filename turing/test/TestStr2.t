var src : char (1000)
var dest : char (1000)
var pos, size := 1
var line : string

size := 1
Str.WriteLineToCharArray (src, "Hello", size)
Str.WriteLineToCharArray (src, "Hello", size)
Str.WriteLineToCharArray (src, "Hello", size)
Str.WriteLineToCharArray (src, "Hello", size)
put src (1 .. size - 1)

Str.ReadLineFromCharArray (src, size - 1, line, pos)
put line, " ", Error.Last
Str.ReadLineFromCharArray (src, size - 1, line, pos)
put line, " ", Error.Last
Str.ReadLineFromCharArray (src, size - 1, line, pos)
put line, " ", Error.Last
Str.ReadLineFromCharArray (src, size - 1, line, pos)
put line, " ", Error.Last
Str.ReadLineFromCharArray (src, size - 1, line, pos)
put line, " ", Error.Last
Str.ReadLineFromCharArray (src, size - 1, line, pos)
put line, " ", Error.Last
