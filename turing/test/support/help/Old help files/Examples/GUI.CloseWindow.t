% The "GUI.CloseWindow" program.
import GUI

const screenWidth : int := Config.Display (cdScreenWidth)
const screenHeight : int := Config.Display (cdScreenHeight)
const titleBarHeight : int := 32
const windowEdgeSize : int := 13
const windowWidth : int := 150
const windowHeight : int := 100
var windowID, windowNumber, closeButton, quitButton : int := 0

procedure CloseAndOpen
    if windowID not= 0 then
	GUI.CloseWindow (windowID)
    end if
    windowNumber += 1
    var xPos : int := Rand.Int (0, screenWidth - windowWidth - windowEdgeSize)
    var yPos : int := Rand.Int (0, screenHeight - windowHeight - titleBarHeight)
    windowID := Window.Open ("title:Window #" +
	intstr (windowNumber) + ",graphics:" +
	intstr (windowWidth) + ";" + intstr (windowHeight) +
	",position:" + intstr (xPos) + ";" + intstr (yPos))
    closeButton := GUI.CreateButton (10, 60, 130,
	"Close And Open", CloseAndOpen)
    quitButton := GUI.CreateButton (10, 10, 130, "Quit", GUI.Quit)
end CloseAndOpen

CloseAndOpen
loop
    exit when GUI.ProcessEvent
end loop
GUI.CloseWindow (windowID)
