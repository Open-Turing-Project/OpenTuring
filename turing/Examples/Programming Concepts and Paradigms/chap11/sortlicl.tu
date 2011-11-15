% File "sortlicl.tu". % A SortedList class.
% The list contains a set of items each consisting of a key and info.
% New items are entered so that the list is kept sorted.
% Existing items can be looked up, changed, or deleted.
% The list can be printed in sorted order.
unit
class SortedList
    inherit List in "listcl.tu"

    % Override procedure Enter to maintain sorted order.
    body procedure Enter (key, info : string)
        var p : ^entry
        new p
        p -> key := key
        p -> info := info
        % Find location to insert new entry.
        % See if new item goes first in list.
        if first = nil or key < first -> key then
            p -> next := first
            first := p
        else
            % Find place to insert new entry.
            var previous : ^entry := first
            var following : ^entry := first -> next
            loop
                exit when following = nil or key < following -> key
                previous := following
                following := following -> next
            end loop
            % Adjust links to make insertion.
            previous -> next := p
            p -> next := following
        end if
    end Enter

end SortedList
