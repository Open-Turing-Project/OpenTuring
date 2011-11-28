% The "GUI.CreateMenu" program.
import GUI

View.Set ("graphics:250;150,nobuttonbar")
var first, second : int                 % The menus.
var item : array 1 .. 12 of int         % The menu items.
var name : array 1 .. 12 of string (20) :=
    init ("Quit", "---", "A", "B", "---", "C", "D",
    "Disable B Menu Item", "Enable B Menu Item", "---",
    "Disable Second Menu", "Enable Second Menu")

procedure MenuSelected
    for i : 1 .. 12
	if item (i) = GUI.GetEventWidgetID then
	    Text.Locate (maxrow, 1)
	    put name (i) + " selected               " ..
	end if
    end for
end MenuSelected

procedure DisableB
    GUI.Disable (item (4))
end DisableB

procedure EnableB
    GUI.Enable (item (4))
end EnableB

procedure DisableFirst
    GUI.Disable (first)
end DisableFirst

procedure EnableFirst
    GUI.Enable (first)
end EnableFirst

% Create the menus
first := GUI.CreateMenu ("First")
item (1) := GUI.CreateMenuItem (name (1), GUI.Quit)
for cnt : 2 .. 7
    item (cnt) := GUI.CreateMenuItem (name (cnt),
	MenuSelected)
end for

second := GUI.CreateMenu ("Second")
item (8) := GUI.CreateMenuItem (name (8), DisableB)
item (9) := GUI.CreateMenuItem (name (9), EnableB)
item (10) := GUI.CreateMenuItem (name (10), MenuSelected)
item (11) := GUI.CreateMenuItem (name (11), DisableFirst)
item (12) := GUI.CreateMenuItem (name (12), EnableFirst)

loop
    exit when GUI.ProcessEvent
end loop
