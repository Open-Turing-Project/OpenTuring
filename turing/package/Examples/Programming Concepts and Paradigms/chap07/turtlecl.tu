% File "turtlecl.tu".
unit
class TurtleCl
    export SetColor, SetPosition, SetAngle, Move, TurnLeft, TurnRight,
        ShowTrace, HideTrace
    % Declare common variables and initialize them.
    var x : int := maxx div 2   % Set x to center of window.
    var y : int := maxy div 2   % Set y to center of window.
    var angle : int := 0        % Set to point to right of window.
    var clr : int := colorfg    % Set to foreground color.
    var showing : boolean := true % Set to show trace.

    procedure SetColor (newClr : int)
        clr := newClr
    end SetColor

    procedure SetPosition (newX, newY : int)
        x := newX
        y := newY
    end SetPosition

    procedure SetAngle (newAngle : int)
        angle := newAngle
    end SetAngle

    procedure TurnLeft (turnAngle : int)
        angle := angle + turnAngle
        angle := angle mod 360 % Remove any complete rotation.
    end TurnLeft

    procedure TurnRight (turnAngle : int)
        angle := angle - turnAngle
        angle := angle mod 360 % Remove any complete rotation.
    end TurnRight

    procedure ShowTrace
        showing := true
    end ShowTrace

    procedure HideTrace
        showing := false
    end HideTrace

    procedure Move (distance : int)
        var newx, newy : int
        newx := round (x + cosd (angle) * distance)
        newy := round (y + sind (angle) * distance)
        if showing then
            Draw.Line (x, y, newx, newy, clr)
        end if
        x := newx
        y := newy
    end Move

end TurtleCl
