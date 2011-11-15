% File "labelpie.tu". Class to display labelled pie charts.
unit
class LabelledPie
    inherit Pie in "pie.tu"

    export AddLabelledSegment

    const titleLineSize : int := 20

    var titles : array 1 .. maxSegments of string
    var font : int := Font.New ("serif:12")

    % Displays the label of a segment.  This procedure is not exported.
    procedure drawLabel (angle, newAngle, clr : int, title : string)
        var x1, y1, x2, y2 : int
        var midAngle, subTotal : int := 0

        % Calculate the end points and then draw the title radial line.
        midAngle := (angle + newAngle) div 2
        x1 := xc + round (radius * cosd (midAngle))
        y1 := yc + round (radius * sind (midAngle))
        x2 := xc + round ( (radius + titleLineSize) * cosd (midAngle))
        y2 := yc + round ( (radius + titleLineSize) * sind (midAngle))
        Draw.Line (x1, y1, x2, y2, clr)

        % Draw the title
        % if the angle is from 0-90 or 270-360, then draw the title
        % left justified, otherwise draw it right justified.
        if (0 <= midAngle and midAngle <= 90) or
                (270 <= midAngle and midAngle <= 360) then
            Font.Draw (title, x2 + 3, y2 - 6, font, clr)
        else
            var width : int := Font.Width (title, font)
            Font.Draw (title, x2 - 3 - width, y2 - 6, font, clr)
        end if
    end drawLabel

    % Overrides ShowPie in "pie.tu"
    body procedure ShowPie
        if showing then % Return immmediately if pie already visible.
            return
        end if

        if total = 0 then % Return immediately if there are no segments.
            put "No segments for this pie chart allocated"
            return
        end if

        Pie.ShowPie
        var angle, midAngle, newAngle, subTotal : int := 0

        for count : 1 .. numSegments
            % Calculate the beginning and ending angles
            % of the arc for this segment.
            subTotal += values (count)
            newAngle := subTotal * 360 div total
            if titles (count) not= "" then
                drawLabel (angle, newAngle, colorfg, titles (count))
            end if
            angle := newAngle
        end for

        % Draw outline of pie.
        Draw.Oval (xc, yc, radius, radius, colorfg)
    end ShowPie

    body procedure HidePie
        var angle, midAngle, newAngle, subTotal : int := 0

        if not showing then % Return immmediately if pie already invisible.
            return
        end if

        % Hide the pie
        Pie.HidePie

        % Hide the labels by drawing them in the background color.
        for count : 1 .. numSegments
            subTotal += values (count)
            newAngle := subTotal * 360 div total
            drawLabel (angle, newAngle, colorbg, titles (count))
            angle := newAngle
        end for
    end HidePie

    %
    procedure AddLabelledSegment (value : int, title : string)
        var wasShowing : boolean := showing
        if showing then
            HidePie
        end if
        numSegments += 1
        values (numSegments) := value
        total += value
        titles (numSegments) := title
        if wasShowing then
            ShowPie
        end if
    end AddLabelledSegment

    body procedure AddSegment (value : int)
        AddLabelledSegment (value, "") % Segment given a null label.
    end AddSegment

end LabelledPie
