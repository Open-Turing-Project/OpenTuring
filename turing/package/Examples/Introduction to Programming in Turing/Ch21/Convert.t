% The "Convert" program
import GUI in "%oot/lib/GUI"

View.Set ("graphics:280;70")
Window.Set (defWinId, "title:Temperature Converter")

var celsiusTextField, fahrenheitTextField : int

procedure CelsiusToFahrenheit
    var celsius : string := GUI.GetText (celsiusTextField)
    if strintok (celsius) then
	var fahrenheit : int := round (strint (celsius) * 9 / 5 + 32)
	GUI.SetText (fahrenheitTextField, intstr (fahrenheit))
    end if
end CelsiusToFahrenheit

procedure CelsiusToFahrenheit1 (dummy : string)
    CelsiusToFahrenheit
end CelsiusToFahrenheit1

procedure FahrenheitToCelsius
    var fahrenheit : string := GUI.GetText (fahrenheitTextField)
    if strintok (fahrenheit) then
	var celsius : int := round ( (strint (fahrenheit) - 32) * 5 / 9)
	GUI.SetText (celsiusTextField, intstr (celsius))
    end if
end FahrenheitToCelsius

procedure FahrenheitToCelsius1 (dummy : string)
    FahrenheitToCelsius
end FahrenheitToCelsius1

% Create a text field for the celsius reading
celsiusTextField := GUI.CreateTextFieldFull (15, 20, 50, "",
    CelsiusToFahrenheit1, GUI.INDENT, 0, 0)

% Create a label for the celsius field
var d1 : int := GUI.CreateLabelFull (15, 20 +
    GUI.GetHeight (celsiusTextField),
    "Celsius", 50, 0, GUI.CENTER, 0)

% Create a text field for the fahrenheight reading
fahrenheitTextField := GUI.CreateTextFieldFull (215, 20, 50, "",
    FahrenheitToCelsius1, GUI.INDENT, 0, 0)

% Create a label for the fahrenheit field
var d2 : int := GUI.CreateLabelFull (215, 20 +
    GUI.GetHeight (celsiusTextField),
    "Fahrenheit", 50, 0, GUI.CENTER, 0)

% Create the button to convert from celsius to fahrenheit
var toFahrenheit : int := GUI.CreateButton (100, 33, 80, "Convert ->",
    CelsiusToFahrenheit)

% Create the button to convert from fahrenheit to celsius
var toCelsius : int := GUI.CreateButton (100, 3, 80, "<- Convert",
    FahrenheitToCelsius)

GUI.SetBackgroundColour (grey)

loop
    exit when GUI.ProcessEvent
end loop
