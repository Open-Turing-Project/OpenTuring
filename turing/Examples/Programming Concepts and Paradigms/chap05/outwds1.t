% File "outwds1.t".  A procedure with a parameter.
procedure printWords (line : string)
    % Set scanning variable to locate first character of line.
    var i := 1
    loop 
        % Print a word each time through the loop.
        % Terminate at end of line or when only blanks remain.
        exit when i > length (line) or 
            line (i .. *) = repeat (" ", length (line) - i + 1)
        loop 
           % Find the first character.
            exit when line (i) not= " "
            i += 1
        end loop
        loop
            put line (i) ..  % Output next character of word.
            i += 1
           % Stop reading characters at end of line or at a blank.
            exit when i > length (line) or line (i) = " "
        end loop
        put ""  % End the output line.
    end loop
end printWords

put "What file do you want to print? "
var fileName : string
get fileName
var streamNo : int
open : streamNo, fileName, get
assert streamNo > 0
var line : string
loop
    exit when eof (streamNo)
    get : streamNo, line : *
    printWords (line)
end loop
