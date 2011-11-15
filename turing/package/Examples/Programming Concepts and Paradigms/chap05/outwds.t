% File "outwds.t".  Use a procedure with two variable parameters.

procedure pickOffWord (var line, word : string)
    % Procedure must not be called if there are no words in line.
    % Set word to first word of line and remove it from line.
    var i := 1   % Set scanning index.
    loop % Find the first non-blank character in line.
        exit when line (i) not= " "
        i += 1
    end loop
    var j := i + 1  % Set j to position after first non-blank.
    loop
        exit when j > length (line) or line (j) = " "  % End of line or blank.
        j += 1
    end loop
    word := line (i .. j - 1)  % Pick off word.
    line := line (j .. *)  % Remove word from line; line may be null string.
end pickOffWord

var text := "A storm was coming, but the winds were still"
loop % Print one word on each iteration.
    exit when text = repeat (" ", length (text))  % Exit when only blanks left.
    % The exit is true if length (text) = 0.
    var nextWord : string
    pickOffWord (text, nextWord)
    put nextWord
end loop
