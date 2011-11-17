var h : int := IntHashMap.New ()
var res : int := 9789

IntHashMap.Put (h, "hi", 5)
IntHashMap.Put (h, "yo", 7)

for i : 0 .. 100
    IntHashMap.Put (h,"test " + intstr(i), i)
end for
for i : 50..90
    var got : int
    var worked := IntHashMap.Get (h, "test " + intstr(i), got)
    if worked ~= 1 or got ~= i then
	put "failed on " + intstr(i) + " it gave " + intstr(got)
    end if
end for

put "successful: ", IntHashMap.Get (h, "hi", res)
put "should be 5:", res

put "ITEM HI REMOVED"
IntHashMap.Remove (h, "hi")

put "get successful: ", IntHashMap.Get (h, "hi", res)
put "should be 0:", res

put ""

put "successful: ", IntHashMap.Get (h, "yo", res)
put "should be 7:", res

IntHashMap.Free (h)

