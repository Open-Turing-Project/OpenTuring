% File "books.tu".  Class to maintain a list of book authors and titles.
unit
class BookList
    export Insert, Delete, PrintList

    const maxSize := 10
    var author, title : array 1 .. maxSize of string
    var size := 0

    % Add a book to the list.
    procedure Insert (a, t : string)
        pre size < maxSize
        post size <= maxSize and author (size) = a and title (size) = t
        size += 1
        author (size) := a
        title (size) := t
    end Insert

    % Delete a book from the list.
    procedure Delete (a, t : string)
        pre size > 0 % and the pair (a, t) is in list
        var i := 1
        loop
            exit when author (i) = a and title (i) = t
            i += 1
        end loop
        % Put the last item in the list at the deletion point.
        author (i) := author (size)
        title (i) := title (size)
        size -= 1
    end Delete

    % Print the list of books.
    procedure PrintList
        pre size >= 0
        put "["
        for i : 1 .. size
            put "  ", author (i), "/", title (i)
        end for
        put "]"
    end PrintList
end BookList
