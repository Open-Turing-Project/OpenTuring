% The "HouseLocator" program
% Reads series of records for houses
% Allows questions about records in file
type houseType :
    record
	color : string (10)
	location : string (20)
	price : int
    end record
const maxHouses := 100
var houseFile : array 1 .. maxHouses of houseType

procedure readFile (var entry : array 1 .. * of houseType, var count : int)
    % Read houseType records into an array
    var sale : int
    open : sale, "HouseInfo", get
    assert sale > 0
    count := 0
    loop
	get : sale, skip
	exit when eof (sale)
	count := count + 1
	bind var house to entry (count)
	get : sale, house.color : 10, house.location : 20,
	    house.price
    end loop
end readFile

procedure outputRecord (h : houseType)
    put h.color : 10, h.location : 20, h.price : 10
end outputRecord

function wanted (house : houseType, desiredColor, desiredLocation : 
    string (*),
	desiredUpperPrice : int) : boolean
    result (house.color = desiredColor or
	desiredColor = "any       ")
	and (house.location = desiredLocation
	or desiredLocation = "any                 ")
	and (house.price <= desiredUpperPrice)
end wanted

procedure readSpecs (var Color : string (*), var location : string (*),
	var price : int)
    put "Answer 'any' for color or location when it doesn't matter."
    put "What color of house do you want?"
    get Color            % must pad with blanks
    Color := Color + repeat (" ", upper (Color) - length (Color))
    put "What location of house do you want?"
    get location         % must pad with blanks
    location := location + repeat (" ", upper (location) - length (location))
    put "What is your highest price in dollars?"
    get price
end readSpecs

var howMany : int
readFile (houseFile, howMany)
var wantedColor : string (10)
var wantedLocation : string (20)
var upperPrice : int
readSpecs (wantedColor, wantedLocation, upperPrice)
for i : 1 .. howMany
    if wanted (houseFile (i), wantedColor,
	    wantedLocation, upperPrice) then
	outputRecord (houseFile (i))
    end if
end for
