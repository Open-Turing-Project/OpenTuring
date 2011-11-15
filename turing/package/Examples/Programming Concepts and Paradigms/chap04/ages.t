% File "ages.t".
% Read sequence of ages, counting
% the number of students, adults, and seniors.
const sentinel := -1
put "Enter ages, end with ", sentinel
var student, adult, senior := 0
var age : int
loop
    get age
    exit when age = sentinel
    if age >= 65 then
        senior += 1
    elsif age <= 12 then
        student += 1
    else
        adult += 1
    end if
end loop
put "seniors=", senior, " adults=", adult, " students=", student
