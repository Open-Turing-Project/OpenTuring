% The "GUI.CreateHorizontalSlider" program.
import GUI

View.Set ("graphics:300;60,nobuttonbar")
var slider : int
procedure SliderMoved (value : int)
    Text.Locate (2, 3)
    put "Horizontal Slider: ", value : 4
end SliderMoved

slider := GUI.CreateHorizontalSlider (10, 10, 250,
    50, 150, 50, SliderMoved)
loop
    exit when GUI.ProcessEvent
end loop
