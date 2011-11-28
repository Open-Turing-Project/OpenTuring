% The "Time.SecStr" program.
setscreen ("text")
put Time.SecStr (Time.Sec, "A B d, Y"), " is day number ",
    Time.SecStr (Time.Sec, "j"), " of ", Time.SecStr (Time.Sec, "Y")
