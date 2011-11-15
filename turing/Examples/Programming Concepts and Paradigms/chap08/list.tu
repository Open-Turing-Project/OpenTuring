% File "list.tu".  Class to maintain a list of names.
unit
class NameList
    export Insert, Delete, PrintList

    const maxSize := 10
    var list : array 1 .. maxSize of string
    var size := 0
    % Add a name to the list.
    procedure Insert (v : string)
        size += 1
        list (size) := v
    end Insert

    % Delete a name from the list.
    procedure Delete (v : string)
        var i := 1
        loop
            exit when list (i) = v
            assert i <= maxSize
            i += 1
        end loop
        % Put the last item in the list at the deletion point.
        list (i) := list (size)
        size -= 1
    end Delete

    % Print the list of names.
    procedure PrintList
        put "[" ..
        for i : 1 .. size - 1
            put list (i) + ", " ..
        end for
        if size > 0 then
            put list (size) ..
        end if
        put "]"
    end PrintList

end NameList
