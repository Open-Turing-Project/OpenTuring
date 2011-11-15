% File "testmark.t".  Produce letter grades from marks for test.
var A, B, C, D, F := 0 % Count of each grade, set to zero.
var mark : int
put "Enter marks, end with -1"
loop
    get mark
    exit when mark = - 1
    case mark of
        label 8, 9, 10 :
            put "Mark is an A"
            A += 1
        label 7 :
            put "Mark is a B"
            B += 1
        label 6 :
            put "Mark is a C"
            C += 1
        label 5 :
            put "Mark is a D"
            D += 1
        label 0, 1, 2, 3, 4 :
            put "Mark is a F"
            F += 1
        label :
            put "Please enter a mark between 0 and 10"
    end case
end loop
put "As=", A, " Bs=", B, " Cs=", C, " Ds=", D, " Fs=", F
