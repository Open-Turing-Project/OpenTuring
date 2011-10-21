% The "LoopExample4" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
% The variable x starts at 0.
% The by 12 will add 12 to x each time the counted loop is repeated.
% The adding of 12 to x will continue until x is 360.
for x : 0 .. 360 by 12
    drawbox (x, x, x + 20, x + 30, purple)
    % The bottom-left corner of the first box is at (0, 0).
    % This is the bottom-left corner of the screen.
    % When x is 0, x+20 is 20.
    % When x is 0, x+30 is 30.
    % Therefore the top-right corner of the first box is (20, 30).
    % The second box will have corners (12, 12) and
    % (32, 42) since we are increasing by 12 each time.
end for
