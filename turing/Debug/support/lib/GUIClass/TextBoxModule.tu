unit
module TextBoxModule
    import TextBoxClass, WidgetGlobals
    export HScrollProc, VScrollProc, DoNothing


    procedure HScrollProc (value : int)
	TextBoxClass (WidgetGlobals.selectedWidget -> 
	    GetUserObject).HScrollProc (value)
    end HScrollProc


    procedure VScrollProc (value : int)
	TextBoxClass (WidgetGlobals.selectedWidget -> 
	    GetUserObject).VScrollProc (value)
    end VScrollProc
    
    procedure DoNothing (line : int)
    end DoNothing
end TextBoxModule

