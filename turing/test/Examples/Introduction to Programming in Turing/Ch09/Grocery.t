% The "Grocery" program
% Runs a cash register for a farmer's market stall selling a dozen corn
% for $2.25, bags of potatoes for $6.50, and artichokes for $1.75.
var total : real := 0
var product : int
loop
    put "Which product (1=Corn, 2=Potatoes, 3=Artichokes, 4=Quit): " ..
    get product
    if product = 1 then
	total := total + 2.25
    elsif product = 2 then
	total := total + 6.50
    elsif product = 3 then
	total := total + 1.75
    elsif product = 4 then
	exit
    end if
    % The phrase "total : 0 : 2" outputs total to two decimal places
    put "The running total is $", total : 0 : 2
end loop
put "Final total = $", total : 0 : 2
