% The "Piano" program
% Makes the computer keyboard into a musical keyboard
cls
var duration : string (1)
put "Enter one digit to control duration"
put "You can enter 1, 2, 4, 8, or 6: " ..
get duration
play (duration)
put "You can now begin to play"
put "Play notes by pressing keys 1 to 8, any other key to stop"
var note : string (1)
loop
    getch (note)     % Wait until a key is pressed
    if note = "1" then
	play ("C")
    elsif note = "2" then
	play ("D")
    elsif note = "3" then
	play ("E")
    elsif note = "4" then
	play ("F")
    elsif note = "5" then
	play ("G")
    elsif note = "6" then
	play ("A")
    elsif note = "7" then
	play ("B")
    elsif note = "8" then
	play (">C<")
    else
	exit
    end if
end loop
