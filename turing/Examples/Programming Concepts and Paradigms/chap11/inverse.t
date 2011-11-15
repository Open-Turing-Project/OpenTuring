% File "inverse.t".
% Create linked list. Print in reverse order.
include "innames.ti"
put "Here is the list in reverse order"
loop
    exit when last = nil % Quit when list empty.
    put last -> name % Output most recent name on list.
    var p := last % Locate record to deallocate.
    last := p -> link % Change last to point to next record.
    free p
end loop
