% The "GUI.ShowWindow" program.
import GUI

var windowA, windowB : int
var buttonA, buttonB, buttonQuit, backToMainA, backToMainB : int

procedure WindowA
    GUI.HideWindow (defWinID)
    GUI.ShowWindow (windowA)
end WindowA

procedure WindowB
    GUI.HideWindow (defWinID)
    GUI.ShowWindow (windowB)
end WindowB

procedure BackToMain
    GUI.HideWindow (Window.GetActive)
    GUI.ShowWindow (defWinID)
end BackToMain

View.Set ("graphics:280;100,nobuttonbar")
% Place the buttons in the main window
buttonA := GUI.CreateButton (10, 10, 0, "Show Window A", WindowA)
buttonB := GUI.CreateButton (150, 10, 0, "Show Window B", WindowB)
buttonQuit := GUI.CreateButton (100, 60, 0, "Quit", GUI.Quit)

windowA := Window.Open ("title:Window A,graphics:150;100,position:bottom;left")
backToMainA := GUI.CreateButton (20, 20, 0, "Back to Main", BackToMain)
Window.Hide (windowA)
windowB := Window.Open ("title:Window B,graphics:150;100,position:bottom;right")
backToMainB := GUI.CreateButton (20, 20, 0, "Back to Main", BackToMain)
Window.Hide (windowB)

loop
    exit when GUI.ProcessEvent
end loop
