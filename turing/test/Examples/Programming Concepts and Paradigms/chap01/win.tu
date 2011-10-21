unit
class Win
    export Close, Expand, Hide, Select, Set, SetActive, SetPosition, SetSize,
        SetTitle, Show

    var winID : int

    procedure Close
        Window.Close (winID)
    end Close

    procedure Hide
        Window.Hide (winID)
    end Hide

    procedure Select
        Window.Select (winID)
    end Select

    procedure Set (str : string)
        Window.Set (winID, str)
    end Set

    procedure SetActive
        Window.SetActive (winID)
    end SetActive

    procedure SetPosition (x, y : int)
        Window.SetPosition (winID, x, y)
    end SetPosition

    procedure SetSize (width, height : int)
        Window.Set (winID, "graphics:" + intstr (width) + ";" + 
            intstr (height))
    end SetSize

    procedure SetTitle (title : string)
        Window.Set (winID, "title:" + title)
    end SetTitle

    procedure Show
        Window.Show (winID)
    end Show
    
    procedure Expand
        const screenWidth := Config.Display (cdScreenWidth)
        const screenHeight := Config.Display (cdScreenHeight)
        SetSize (screenWidth - 12, screenHeight - 32)
        SetPosition (0, 0)
    end Expand

    winID := Window.Open ("")
end Win

