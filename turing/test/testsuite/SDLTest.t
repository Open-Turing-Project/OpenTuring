external "sdlgraph_newwin" proc NewWin(width,height,mode : int)
external "sdlgraph_closewin" proc CloseWin() 
external "sdlgraph_update" proc Update()
external "sdlgraph_drawline" proc DrawLine(x1,y1,x2,y2,r,g,b,a : int)

const width := 640
const height := 480

var sTime : int

sTime := Time.Elapsed
NewWin(width,height,0)
for i : 1..5000
    DrawLine(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,250),Rand.Int(0,250),Rand.Int(0,250),250)
    %Update()
end for
put Time.Elapsed - sTime
Update()
delay(1000)
CloseWin()

sTime := Time.Elapsed
View.Set("graphics:640;480,offscreenonly")
for i : 1..5000
    Draw.Line(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,250))
    %View.Update()
end for
put Time.Elapsed - sTime
View.Update
delay(1000)
