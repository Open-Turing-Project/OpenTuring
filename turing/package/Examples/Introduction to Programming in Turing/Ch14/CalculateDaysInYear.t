% The "CalculateDaysInYear" program
% Calculate the number of days in a non-leap year
type monthType : 1 .. 12
var days : array monthType of int := init (31, 28, 31, 30, 31, 30, 31, 31,
    30, 31, 30, 31)
var totalDays : int := 0

for month : monthType
    totalDays := totalDays + days (month)
end for
put "There are ", totalDays, " days in a year"
