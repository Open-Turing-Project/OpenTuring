% The "div" program.
setscreen ("text")
var eggCount : int
put "How many eggs? " ..
get eggCount
put "You have ", eggCount div 12, " dozen eggs"
put "You have ", eggCount mod 12, " left over"
