% File "turtfat.tu".
% A class for creating a turtle that can draw with different widths of lines.
unit
class TurtleFatCl
    inherit TurtleCl in "turtlecl.tu"
    % This will use the Shape module of Chapter. 1.
    import Shape in "shape.tu"
    % Modify to control width of line.
    % Add new exported setWidth operation.
    export SetWidth
    
    % Add new data element called width; initialize to fine line.
    var width : int := 1

    procedure SetWidth (newWidth : int)
        width := newWidth
    end SetWidth

    % This procedure is not exported.
    procedure drawFatLine (x1, y1, x2, y2, clr, width : int)
        % Use a ball-point pen where ball has radius of half the line width
        % and is drawn at x values separated by dx equal to half the radius.
        const len : real := sqrt ( (x2 - x1) ** 2 + (y2 - y1) ** 2)
        const sinTheta : real := (y2 - y1) / len
        const cosTheta : real := (x2 - x1) / len
        const radius := width div 2 + 1
        const dx : real := radius * cosTheta / 2
        const dy : real := radius * sinTheta / 2

        % Set position to draw first ball's center at (x1, y1). 
        var xpos : real := x1
        var ypos : real := y1

        % Draw series of balls along line from (x1, y1) to (x2, y2).
        loop
            Shape.ball (round (xpos), round (ypos), radius, clr)
            xpos += dx
            ypos += dy
            exit when sqrt ((x2 - xpos) ** 2 + (y2 - ypos) ** 2) < radius / 2
        end loop
    end drawFatLine

    % This procedure overrides the old move procedure.
    body procedure Move (distance : int)
        const newx := round (x + cosd (angle) * distance)
        const newy := round (y + sind (angle) * distance)
        if showing then
            if width = 1 then
                Draw.Line (x, y, newx, newy, clr)
            else
                drawFatLine (x, y, newx, newy, clr, width)
            end if
        end if
        x := newx
        y := newy
    end Move

end TurtleFatCl

