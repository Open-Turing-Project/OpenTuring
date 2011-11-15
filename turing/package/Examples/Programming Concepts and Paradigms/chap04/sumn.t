% File "sumn.t". A program to sum the first n integers.
put "Enter the value of a positive number n"
var n : int
get n
var sum := 0
var number := 0
loop
    exit when number = n
    number := number + 1
    sum := sum + number
end loop
put "Sum of the first ", n, " integers is ", sum
