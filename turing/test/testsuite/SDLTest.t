external "sdlgraph_newwin" proc NewWin(width,height,mode : int)
external "sdlgraph_closewin" proc CloseWin() 
external "sdlgraph_update" proc Update()
external "sdlgraph_drawline" proc DrawLine(x1,y1,x2,y2,r,g,b,a : int)

const width := 640
const height := 480

NewWin(width,height,0)
loop
    DrawLine(Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,width),Rand.Int(0,height),Rand.Int(0,250),Rand.Int(0,250),Rand.Int(0,250),250)
    Update()
end loop
CloseWin()
