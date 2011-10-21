% File "pie.tu". Class to displays unlabelled pie charts.
unit
class Pie
    export ShowPie, HidePie, AddSegment, SetCenter, SetRadius

    const maxSegments : int := 10 % The maximum number of segments.

    var total : int := 0 % Total of segment values being displayed.
    var numSegments : int := 0 % The number of segments in the pie.
    var showing : boolean := false % Whether the pie is visible.
    var xc, yc : int := 50 % Center of the pie set to default value of (50,50).
    var radius : int := 50 % Radius of the pie set to default value of 50.

    var values : array 1 .. maxSegments of int
    var colors : array 1 .. maxSegments of int :=
        init (red, green, blue, cyan, magenta, brightred, brightblue,
        yellow, brightpurple, colorbg)

    % Display the pie, if not already visible.
    procedure ShowPie
        var angle, newAngle, subTotal : int := 0

        if showing then % Return immmediately if pie already visible.
            return
        end if

        if total = 0 then % Return immediately if there are no segments.
            put "No segments for this pie chart allocated"
            return
        end if

        showing := true % Set showing indicator on.

        % For each segment, draw the filled arc.
        for count : 1 .. numSegments
            % Calculate the beginning and ending angle
            % of the arc for this segment.
            subTotal += values (count)
            newAngle := subTotal * 360 div total

            % Draw the colored segment
            Draw.FillArc (xc, yc, radius, radius, angle, newAngle, 
                colors (count))

            angle := newAngle
        end for

        % Draw outline of pie.
        Draw.Oval (xc, yc, radius, radius, colorfg)
    end ShowPie

    % Erase the pie, if visible.
    procedure HidePie
        if not showing then % Return immmediately if pie already invisible.
            return
        end if

        showing := false % Set showing indicator off.

        Draw.FillOval (xc, yc, radius, radius, colorbg) % Erase the picture.
    end HidePie

    % Add a segment to the pie.  If the pie is visible, 
    % it will hide it and then show it with the new segment.
    procedure AddSegment (value : int)
        var wasShowing : boolean := showing
        if showing then
            HidePie
        end if
        numSegments += 1
        values (numSegments) := value
        total += value
        if wasShowing then
            ShowPie
        end if
    end AddSegment

    % Set the new center for the pie.  If the pie is visible, 
    % erase the pie in the old position and then display
    % it in the new position.
    procedure SetCenter (newX, newY : int)
        var wasShowing : boolean := showing
        if showing then
            HidePie
        end if
        xc := newX
        yc := newY
        if wasShowing then
            ShowPie
        end if
    end SetCenter

    % Set the new radius of the pie.  If the pie is visible, 
    % erases the pie and then display the pie with its new radius.
    procedure SetRadius (newRadius : int)
        var wasShowing : boolean := showing
        if showing then
            HidePie
        end if
        radius := newRadius
        if wasShowing then
            ShowPie
        end if
    end SetRadius

end Pie
