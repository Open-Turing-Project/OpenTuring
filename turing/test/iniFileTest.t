var name : string

setscreen ("text")

IniFile.Open ("test.ini")
IniFile.SetString ("", "name1", "Fred")
IniFile.SetString ("alts", "name3", "Fal")
put IniFile.GetString ("alts", "name2")
put IniFile.GetString ("", "name")
put IniFile.GetString ("", "name1")

put "List"
IniFile.StartSection
loop
    name := IniFile.GetSection
    exit when name = ""
    IniFile.StartProperty (name)
    put "Section: ", name
    loop
	name := IniFile.GetProperty
	exit when name = ""
	put "Property: ", name
    end loop
end loop
