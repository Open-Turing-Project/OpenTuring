% Insert an item into the heap so as to preserve heap peroprty.
procedure insertHeap (var heap : array 1 .. * of int, i : int, item : int)
    var child := i
    heap (i) := item
    loop % Sift up as required.
        const parent := child div 2
        exit when child = 1 or heap (parent) >= heap (child)
        % Interchange parent and child.
        const temp := heap (parent)
        heap (parent) := heap (child)
        heap (child) := temp
        child := parent
    end loop
end insertHeap

% Sort array elements 1 to n.
procedure removeLargest (var heap : array 1 .. * of int,
        n : int, var largest : int)
    largest := heap (1)
    heap (1) := heap (n)
    var parent := 1
    loop
        var child := 2 * parent
        exit when child > n
        if child < n and heap (child + 1) > heap (child) then
            child += 1
        end if
        exit when heap (parent) >= heap (child)
        % Interchange parent and large child.
        const temp := heap (parent)
        heap (parent) := heap (child)
        heap (child) := temp
        parent := child
    end loop
end removeLargest

procedure heapSort (var heap : array 1 .. * of int, n : int)
    % Make array into heap starting at element 2.
    % Element 1 is already a heap.
    for i : 2 .. n
        insertHeap (heap, i, heap (i))
    end for
    % Remove elements from heap and put in array.
    for decreasing i : n .. 2
        var largest : int
        removeLargest (heap, i, largest)
        heap (i) := largest
    end for
end heapSort

var myArray : array 1 .. 100 of int
for i : 1 .. 100
    myArray (i) := Rand.Int (1, 100)
end for
put "Before: "
for i : 1 .. 100
    put myArray (i) : 3, " " ..
end for
heapSort (myArray, 100)
put "After: "
for i : 1 .. 100
    put myArray (i) : 3, " " ..
end for

