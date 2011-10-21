% The "WhereMouse" Program
var x, y, mousebutton : int
loop
    mousewhere (x, y, mousebutton)
    locatexy (10, 350)
    put "(", x, ",", y, ")"
    exit when mousebutton = 1
end loop
