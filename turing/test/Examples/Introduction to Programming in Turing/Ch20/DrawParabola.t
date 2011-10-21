% The "DrawParabola" program
% Plots a graph of y = x ** 2
% for values of x from 0 to 14
% Number of points to be plotted is 100
setscreen ("graphics")

procedure minmax (list : array 1 .. * of real,
	listlength : int, var listmin, listmax, listrange : real)
    % Find the minimum, maximum and range of the list
    listmin := list (1)
    listmax := list (1)
    for i : 2 .. listlength
	listmin := min (listmin, list (i))
	listmax := max (listmax, list (i))
    end for
    listrange := listmax - listmin
end minmax

procedure graphplot (x, y : array 1 .. * of real,
	pointcount : int, title : string)
    % Label graph at top of window
    locate (1, 1)
    put title
    % Layout plotting area
    const plotleft := 0
    const plotlow := 0
    const plotright := maxx
    const plothigh := maxy - 10
    drawbox (plotleft, plotlow, plotright, plothigh, 1)

    var xmin, xmax, xrange : real
    var ymin, ymax, yrange : real
    minmax (x, pointcount, xmin, xmax, xrange)
    minmax (y, pointcount, ymin, ymax, yrange)
    const xscale := (plotright - plotleft) / xrange
    const yscale := (plothigh - plotlow) / yrange

    % Draw axes if in plotting area
    if ymin <= 0 and ymax > 0 then
	% Plot x-axis
	const yzero := round ( - ymin * yscale) + plotlow
	drawline (plotleft, yzero, plotright, yzero, 2)
    end if
    if xmin <= 0 and xmax > 0 then
	% Plot y-axis
	const xzero := round ( - xmin * xscale) + plotleft
	drawline (xzero, plotlow, xzero, plothigh, 2)
    end if

    % Plot graph
    for i : 1 .. pointcount
	const xplot := round ( (x (i) - xmin) * xscale) +
	    plotleft
	const yplot := round ( (y (i) - ymin) * yscale) +
	    plotlow
	drawdot (xplot, yplot, 3)
    end for
end graphplot

const pointcount := 100
var x, y : array 1 .. pointcount of real
const xleft := 0.0
const xright := 14.0
const xdifference := (xright - xleft) / (pointcount - 1)
% Compute corresponding values of x and y
for i : 1 .. pointcount
    x (i) := xleft + (i - 1) * xdifference
    y (i) := x (i) ** 2
end for
graphplot (x, y, pointcount, "Graph of y = x ** 2")
