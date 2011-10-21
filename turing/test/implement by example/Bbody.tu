unit
class Bbody
    implement B
    
    import A

    body procedure MyB
	var a : ^A
	new a
	a -> MyA2
	put "B"
    end MyB
end Bbody
