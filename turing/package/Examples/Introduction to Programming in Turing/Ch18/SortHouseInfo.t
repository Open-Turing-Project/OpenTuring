% The "SortHouseInfo" program
% Reads file of records from disk file
% Sorts records in order of ascending price
% Stores sorted records on disk
type houseType :
    record
	color : string (10)
	location : string (20)
	price : int
    end record
const maxHouses := 100
var houseFile : array 1 .. maxHouses of houseType
var count : int

procedure readDiskFile (fileStream : int,
	var houseFile : array 1 .. * of houseType,
	var count : int)
    % Read records from file
    count := 0
    loop
	get : fileStream, skip
	exit when eof (fileStream)
	count := count + 1
	bind var house to houseFile (count)
	get : fileStream, house.color : 10,
	    house.location : 20, house.price
    end loop
end readDiskFile

procedure outputFileToDisk (fileStream : int,
	houseFile : array 1 .. * of houseType, count : int)
    % Output records from memory to disk
    for i : 1 .. count
	const house := houseFile (i)
	put : fileStream, house.color : 10,
	    house.location : 20, house.price
    end for
end outputFileToDisk

procedure sortFile (var houseFile :
	array 1 .. * of houseType, count : int)
    % Sort list into ascending order of price
    % Use Shell sort algorithm
    var space : int := count
    loop
	space := floor (space / 2)
	exit when space <= 0
	for i : space + 1 .. count
	    var j : int := i - space
	    loop
		exit when j <= 0
		if houseFile (j).price >
			houseFile (j + space).price then
		    const temp : houseType := houseFile (j)
		    houseFile (j) := houseFile (j + space)
		    houseFile (j + space) := temp
		    j := j - space
		else
		    j := 0
		    % Signal exit
		end if
	    end loop
	end for
    end loop
end sortFile

var filename : string
put "What is the file name for the unsorted records? " ..
get filename
var inStream : int
open : inStream, filename, get
assert inStream > 0
readDiskFile (inStream, houseFile, count)
close : inStream
sortFile (houseFile, count)
put "What is the file name for the sorted records to be? " ..
get filename
var outStream : int
open : outStream, filename, put
assert outStream > 0
outputFileToDisk (outStream, houseFile, count)
