unit
class Abody
    implement A

    import B

    body procedure MyA
	var b : ^B
	new b
	b -> MyB
	put "A"
    end MyA

    body procedure MyA2
	put "C"
    end MyA2
end Abody
