% File "listcl.tu".  % List class.
% The list contains a set of items each consisting of a key and info.
% New items can be entered, existing items deleted, or have info changed.
% Items can be looked up and list can be printed.
unit
class List
    export Enter, Delete, Change, LookUp, Display
    type entry :
        record
            key, info : string
            next : ^entry
        end record
    % Initialize list to be empty.
    var first : ^entry := nil

    % This function is not exported but used internally.
    function find (key : string) : ^entry
        % This is a sequential search.
        var where : ^entry := first
        loop
            exit when where = nil or where -> key = key
            where := where -> next
        end loop
        result where
    end find

    % Add new item assumed not already present.
    procedure Enter (key, info : string)
        var p : ^entry
        new p
        p -> key := key
        p -> info := info
        % Add new entry to head of list.
        p -> next := first
        first := p
    end Enter

    % Delete item assumed already in list.
    procedure Delete (key : string)
        var where : ^entry
        assert first not= nil
        if first -> key = key then
            where := first
            first := where -> next
        else
            var prev : ^entry := first
            where := prev -> next
            loop
                assert where not= nil
                exit when where -> key = key
                prev := where
                where := prev -> next
            end loop
            prev -> next := where -> next
        end if
        free where
    end Delete

    % Change info for item assumed in the list.
    procedure Change (key, info : string)
        const where := find (key)
        assert where not= nil
        where -> info := info
    end Change

    % Look up info for key. If found, success set to true and info returned.
    procedure LookUp (key : string, var info : string, var success : boolean)
        const where := find (key)
        if where not= nil then
            info := where -> info
            success := true
        else
            success := false
        end if
    end LookUp

    % Print all items in list.
    procedure Display
        var p : ^entry := first
        if p not= nil then
            loop
                put p -> key : 20, p -> info
                p := p -> next
                exit when p = nil
            end loop
        else
            put "There are no items in list"
        end if
    end Display
end List
