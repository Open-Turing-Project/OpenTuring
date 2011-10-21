put "test2 = ", Net.GetCookie ("", "test2")
put Error.LastMsg
put "test3 = ", Net.GetCookie ("", "test3")
put Error.LastMsg
delay (2000)

Net.DeleteCookie ("", "test2")
put Error.LastMsg
put "after delete test2 = ", Net.GetCookie ("", "test2")
put Error.LastMsg
delay (2000)

Net.SetCookie ("", "test2", "value", 0)
put Error.LastMsg
put "after set test2 = ", Net.GetCookie ("", "test2")
