% The "BudgetPieChart" program
% Displays monthly expenses graphically

function sumlist (list : array 1 .. * of int, listlength : int) : int
    % Sum the elements of list
    var sum := list (1)
    for i : 2 .. listlength
	sum := sum + list (i)
    end for
    result sum
end sumlist

procedure piechart (item : array 1 .. * of int, itemcount : int)
    % Draw pie chart
    const xcenter := 3 * maxx div 4
    const ycenter := maxy div 2
    const radius := maxy div 2
    var initialAngle : int
    var finalAngle := 0
    const itemscale := 360 / sumlist (item, itemcount)
    var sumItems := 0.0
    for i : 1 .. itemcount
	sumItems := sumItems + item (i)
	initialAngle := finalAngle
	finalAngle := round (sumItems * itemscale)
	const Color := i mod maxcolor + 1
	drawfillarc (xcenter, ycenter, radius, radius,
	    initialAngle, finalAngle, Color)
    end for
end piechart

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
piechart (expense, 8)
var reply : string (1)
getch (reply)
