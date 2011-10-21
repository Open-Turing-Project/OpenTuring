% The "GUI.CreateVerticalSlider Example" program.

import GUI

View.Set ("graphics:100;200,nobuttonbar")

var slider : int

procedure SliderMoved (value : int)
    Text.Locate (9, 7)
    put "Slider"
    Text.Locate (10, 9)
    put value : 3
end SliderMoved

slider := GUI.CreateVerticalSlider (20, 10, 180, 50, 150, 50, SliderMoved)
loop
    exit when GUI.ProcessEvent
end loop
