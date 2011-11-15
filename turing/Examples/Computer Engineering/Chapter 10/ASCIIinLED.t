% The "ASCIIinLED" program.
var character, answer : string
var value : int
loop
    put "Please enter the character: " ..
    get character
    % Ord converts the character to its ASCII equivalent.
    value := ord (character)
    % This will output the ASCII value of the number to the LEDs,
    % in binary form.
    % The original character is displayed on the screen.
    parallelput (value)
    put character
    delay (500)
    put "Continue (y/n)? " ..
    get answer
    exit when answer = "n"
end loop
