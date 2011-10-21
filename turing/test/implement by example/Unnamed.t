class A
    implement by Abody

    deferred procedure MyA
end A

class B
    implement by Bbody

    deferred procedure MyB
end B

class Abody
    implement A

    procedure MyA
	put "A"
    end MyA
end Abody
