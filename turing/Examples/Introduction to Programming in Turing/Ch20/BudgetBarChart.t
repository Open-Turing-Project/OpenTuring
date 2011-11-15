% The "BudgetBarChart" program
% Displays monthly expenses graphically

function maxlist (list : array 1 .. * of int, listlength : int) : int
    % Finds the largest element of list
    var maximum := list (1)
    for i : 2 .. listlength
	maximum := max (maximum, list (i))
    end for
    result maximum
end maxlist

procedure barchart (item : array 1 .. * of int, itemcount : int)
    % Draws a bar chart representing array item
    const chartleft := maxx div 2
    const chartlow := maxy div 2 + 20
    const chartwidth := maxx div 2
    const chartheight := maxy div 2 - 30
    const barspacing := round (chartwidth / itemcount)
    const barwidth := barspacing - 3
    const barheightscale := chartheight / maxlist (item, itemcount)

    for i : 0 .. itemcount - 1
	const barleft := chartleft + i * barspacing
	const barheight := round (item (i + 1) * barheightscale)
	const Color := i mod maxcolor + 1
	drawfillbox (barleft, chartlow,
	    barleft + barwidth, chartlow + barheight, Color)
    end for
end barchart

setscreen ("graphics")
var expense : array 1 .. 8 of int
put "Enter monthly expenses to nearest dollar"
put "Rent =" ..
get expense (1)
put "Food = " ..
get expense (2)
put "Clothing = " ..
get expense (3)
put "Insurance = " ..
get expense (4)
put "Car cost = " ..
get expense (5)
put "Taxes = " ..
get expense (6)
put "Utilities = " ..
get expense (7)
put "Entertainment = " ..
get expense (8)
barchart (expense, 8)
var reply : string (1)
getch (reply)
