% The "GUI.SetEchoChar" program.
import GUI

proc P (s : string)
    put "You entered \"", s, "\""
end P

var t := GUI.CreateTextField (10, 100, 100, "", P)
GUI.SetEchoChar (t, '*')
loop
    exit when GUI.ProcessEvent
end loop
