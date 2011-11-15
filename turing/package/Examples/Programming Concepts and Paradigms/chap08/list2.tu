% File "list2.tu".  Class extension for a sorted list.
unit
class SortedList
    inherit NameList in "list.tu" % This new class will inherit NameList.
    % Add a name to the list.
    body procedure Insert (v : string)
        var i := 1
        loop
            exit when i > size or list (i) > v
            i += 1
        end loop
        % Shift the remaining items over to make room.
        for decreasing j : size + 1 .. i + 1
            list (j) := list (j - 1)
        end for
        size += 1
        list (i) := v
    end Insert

    % Delete a name from the list.
    body procedure Delete (v : string)
        var i := 1
        loop
            exit when list (i) = v
            i += 1
        end loop
        % Shift remaining items down.
        for j : i .. size - 1
            list (j) := list (j + 1)
        end for
        size -= 1
    end Delete

end SortedList
