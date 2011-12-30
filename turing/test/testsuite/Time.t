put Time.Sec

put Time.Date

var str : string

time (str)

put str

var t : int

wallclock (t)

put t

date (str)

put str

sysclock (t)

put t

put Time.SecStr (Time.Sec, "A B d, Y"), " is day number ",
    Time.SecStr (Time.Sec, "j"), " of ", Time.SecStr (Time.Sec, "Y")

var year, month, day, dayOfWeek, hour, minute, second : int
Time.SecParts (Time.Sec, year, month, day, dayOfWeek,
    hour, minute, second)
var days : array 1 .. 7 of string (10) := init ("Monday", "Tuesday",
    "Wednesday", "Thursday", "Friday", "Saturday", "Sunday")
put "The current day of the week is ", days (dayOfWeek)
