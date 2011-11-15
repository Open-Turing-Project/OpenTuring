% File "list3.tu".  Class extension for a sorted list with separate find procedure.
unit
class SortedList
    inherit NameList in "list.tu"
    export find

    % Procedure to find a value, or the place to put it.
    procedure find (v : string, var location : int,
            var success : boolean)
        location := 1
        loop
            exit when location > size or list (location) >= v
            location += 1
        end loop
        success := location <= size and list (location) = v
    end find

    % Add a name to the list.
    body procedure Insert (v : string)
        var i : int
        var success : boolean
        find (v, i, success)
       % Shift the remaining items over to make room.
        for decreasing j : size  + 1 .. i + 1
            list (j) := list (j - 1)
        end for
        size += 1
        list (i) := v
    end Insert

    % Delete a name from the list.
    body procedure Delete (v : string)
        var i : int
        var success : boolean
        find (v, i, success)
        assert success
        % Shift remaining items down.
        for j : i .. size - 1
            list (j) := list (j + 1)
        end for
        size -= 1
    end Delete

end SortedList
