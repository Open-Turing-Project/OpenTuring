% The "LoopExample5" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
% The for is a counting structure command.
% The x is the variable that the computer will use to count.
% x will count by 1 starting at 10 and continue until it reaches 630.
for x : 10 .. 630
    drawoval (x, 40, 10, 12, brightblue)
    % drawoval is the command to draw an oval on the screen.
    % The oval is the ball.
    % The ball's center will start at the point x, 40 on the screen.
    % Since x starts at 10, the ball's center will be (10, 40).
    % The ball's horizontal axis is 10 pixels wide.
    % Its vertical axis is 12 pixels long.
    % The ball will bright blue.
    delay (5)
    % The delay will pause the program for the duration
    % of the number in brackets, which in this case is 5 milliseconds.
    drawoval (x, 40, 10, 12, white)
    % The drawoval will draw the same size oval but this time
    % it is drawn in white which is the background colour so it appears to have      
    % been erased.
end for
