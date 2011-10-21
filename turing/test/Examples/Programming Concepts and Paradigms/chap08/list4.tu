% File "list4.tu".  Class extension for a sorted list
% with separate binary search procedure find.
unit
class FastSortedList
	inherit SortedList in "list3.tu"

	body procedure find (v : string, var location : int,
		var success : boolean)
		var first := 1
		var last := size
		loop
			exit when first = last or last = 0
			const middle := (first + last) div 2
			if v <= list (middle) then
				% Key lies in first half (or not in list).
   				last := middle  % Discard second half (but not middle element).
		else
			% Discard first half (including middle).
			first := middle + 1
		end if
		end loop
        location := first
        if location <= size and list (location) < v then
            location += 1
        end if
        success := location <= size and list (location) = v
    end find

end FastSortedList
