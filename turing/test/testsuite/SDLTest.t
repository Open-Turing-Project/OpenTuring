external "sdlgraph_newwin" proc NewWin(width,height,mode : int)
external "sdlgraph_closewin" proc CloseWin() 
external "sdlgraph_cls" proc SDLCls() 
external "sdlgraph_update" proc Update()
external "sdlgraph_drawline" proc DrawLine(x1,y1,x2,y2,r,g,b : int)
external "sdlgraph_drawfillrect" proc DrawFillRect(x1,y1,x2,y2,r,g,b : int)

const width := 640
const height := 480

var sTime : int

NewWin(width,height,0)
View.Set("graphics:640;480,offscreenonly")


% double buffered lines
sTime := Time.Elapsed
for i : 1..50000
    DrawLine(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),130,90,180)
    %Update()
end for
var sOne := Time.Elapsed - sTime
Update()
delay(1000)

sTime := Time.Elapsed
for i : 1..50000
    Draw.Line(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),100)
    %View.Update()
end for
var nOne := Time.Elapsed - sTime
View.Update
delay(1000)

% immediate lines
SDLCls()
sTime := Time.Elapsed
for i : 1..500
    DrawLine(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),130,90,180)
    Update()
end for
var si := Time.Elapsed - sTime
Update()
delay(1000)

cls
sTime := Time.Elapsed
for i : 1..500
    Draw.Line(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),100)
    View.Update()
end for
var ni := Time.Elapsed - sTime
delay(1000)

% double buffered fillrects
SDLCls()
sTime := Time.Elapsed
for i : 1..50000
    DrawFillRect(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),130,90,180)
    %Update()
end for
var sfrd := Time.Elapsed - sTime
Update()
delay(1000)

cls
sTime := Time.Elapsed
for i : 1..50000
    Draw.FillBox(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),100)
    %View.Update()
end for
var nfrd := Time.Elapsed - sTime
View.Update

% instant fillrects
SDLCls()
sTime := Time.Elapsed
for i : 1..500
    DrawFillRect(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),130,90,180)
    Update()
end for
var sfri := Time.Elapsed - sTime
Update()
delay(1000)

cls
sTime := Time.Elapsed
for i : 1..500
    Draw.FillBox(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),100)
    View.Update()
end for
var nfri := Time.Elapsed - sTime
View.Update

%out
put "SDL db lines: ",sOne
put "Normal db lines: ",nOne
put "SDL instant lines: ",si
put "Normal instant lines: ",ni
put "SDL db fillrects: ",sfrd
put "Normal db fillrects: ",nfrd
put "SDL instant fillrects: ",sfri
put "Normal instant fillrects: ",nfri
View.Update
delay(5000)
CloseWin()
