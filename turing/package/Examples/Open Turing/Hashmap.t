var h : int := IntHashMap.New()
var res : int := 9789

IntHashMap.Put(h,"hi",5)
IntHashMap.Put(h,"yo",7)

put "successful: ",IntHashMap.Get(h,"hi",res)
put "should be 5:",res

put "ITEM HI REMOVED"
IntHashMap.Remove(h,"hi")

put "get successful: ",IntHashMap.Get(h,"hi",res)
put "should be 0:",res

put ""

put "successful: ",IntHashMap.Get(h,"yo",res)
put "should be 7:",res

IntHashMap.Free(h)

