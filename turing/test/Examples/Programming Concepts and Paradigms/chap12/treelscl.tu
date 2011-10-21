% File "treelscl.tu".
% A binary tree implementation of the ordered list of items
% consisting of a key and information.
% New items can be entered and the list displayed.
unit
class SortedList
    export Enter, Display
    type entry :
        record
            key, info : string
            left, right : ^entry
        end record
    % Initialize tree to be empty.
    var root : ^entry := nil

    % Enter new item assumed not already present.
    procedure Enter (key, info : string)
        var p : ^entry
        new p % Create new node.
        p -> key := key
        p -> info := info
        p -> right := nil
        p -> left := nil
        % Find location to insert new entry.
        if root = nil then % See if new item goes at root of tree.
            root := p
        else
            var where : ^entry := root
            loop % Search for place to attach new node.
                % Invariant: "where" locates non-empty subtree where
                % node is to be entered.
                if p -> key < where -> key then
                    if where -> left = nil then
                        % Attach new node here.
                        where -> left := p
                        exit
                    else
                        % Search left subtree.
                        where := where -> left
                    end if
                else
                    if where -> right = nil then
                        % Attach new node here.
                        where -> right := p
                        exit
                    else
                        % Search right subtree.
                        where := where -> right
                    end if
                end if
            end loop
        end if
    end Enter

    % Outputs subtree pointed to by where.
    % Recursive printing.
    procedure RecursiveDisplay (where : ^entry)
        if where not= nil then
            RecursiveDisplay (where -> left) % Output left subtree.
            put where -> key, " ", where -> info % Output node.
            RecursiveDisplay (where -> right) % Output right subtree.
        end if
    end RecursiveDisplay

    % Uses the procedure RecPrint.
    procedure Display
        RecursiveDisplay (root)
    end Display
end SortedList
