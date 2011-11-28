% The "GUI.CreateVerticalSlider" program.
import GUI

View.Set ("graphics:125;200,nobuttonbar")
var slider : int

procedure SliderMoved (value : int)
    Text.Locate (7, 7)
    put "Slider"
    Text.Locate (8, 9)
    put value : 3
end SliderMoved

slider := GUI.CreateVerticalSlider (20, 10, 180,
    50, 150, 50, SliderMoved)
loop
    exit when GUI.ProcessEvent
end loop
