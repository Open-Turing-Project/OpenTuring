var h : int := HashMap.New()
var res : int := 9789

HashMap.Put(h,"hi",5)
HashMap.Put(h,"yo",7)

put "successful: ",HashMap.Get(h,"hi",res)
put "should be 5:",res

put "ITEM HI REMOVED"
HashMap.Remove(h,"hi")

put "get successful: ",HashMap.Get(h,"hi",res)
put "should be 0:",res

put ""

put "successful: ",HashMap.Get(h,"yo",res)
put "should be 7:",res

HashMap.Free(h)

