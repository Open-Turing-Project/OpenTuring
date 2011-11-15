% File "freq.t".  Count the occurrences of characters in a text.
var count : array 0 .. 255 of int
% Initialize array elements to zero
for i : 0 .. 255
    count (i) := 0
end for

var infile : int
var fileName : string
put "What file do you want to test? "
get fileName
open : infile, fileName, get
assert infile > 0

% Read lines of text and count characters.
loop
    exit when eof (infile)
    var line : string
    get : infile, line : * % Read entire line.
    for i : 1 .. length (line) % Count the characters.
        count (ord (line (i))) += 1
    end for
end loop

% Display the counts for letters, digits, and punctuation marks.
const valuesPerLine := 8
var valuesPrinted := 0
procedure display (s : string (1), i : int)
    put s, i : 4, " " ..
    valuesPrinted += 1
    % Start new line after each valuesPerLine elements output.
    if valuesPrinted mod valuesPerLine = 0 then
        put "" % Start new output line.
    end if
end display

const reportable := "abcdefghijklmnopqrstuvwxyz"
    + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    + "0123456789"
    + ",./?><;':[]{}|=+-_()*&%$#@!~`"
% Print counts for reportable characters.
for i : 1 .. length (reportable)
    display (reportable (i), count (ord (reportable (i))))
end for
% Print counts for \ and " characters.
display ("\\", count (ord ("\\"))) % Display count for \.
display ("\"", count (ord ("\""))) % Display count for ".
if valuesPrinted mod valuesPerLine not= 0 then
    put ""
end if
