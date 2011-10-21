% The "TextBarChart" program
% Plot a bar chart for class marks
var count : int

procedure readFreq (var freqList : array 1 .. * of int)
    for i : 1 .. upper (freqList)
	get freqList (i)
    end for
end readFreq

function maximum (list : array 1 .. * of int) : int
    var biggest : int := list (1)
    for i : 2 .. upper (list)
	if list (i) > biggest then
	    biggest := list (i)
	end if
    end for
    result biggest
end maximum

procedure graph (grafList : array 1 .. * of int, width : int)
    const scale := width / maximum (grafList)
    for i : 1 .. upper (grafList)
	put repeat ("*", round (scale * grafList (i)))
    end for
    put "Maximum is ", maximum (grafList)
    put "Scale is ", scale : 10 : 2
end graph

put "Enter number of entries in list: " ..
get count
var entries : array 1 .. count of int
readFreq (entries)
graph (entries, 60)
