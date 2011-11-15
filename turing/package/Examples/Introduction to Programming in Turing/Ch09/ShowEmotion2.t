% The "ShowEmotion2" program
% Here we select commands by number from a menu
var command : int
loop
    put "Choose from 1-sing, 2-cry, 3-laugh, 4-stop: " ..
    get command
    case command of
	label 1 :
	    put "la la la"
	label 2 :
	    put "boo hoo"
	label 3 :
	    put "ha ha ha"
	label 4 :
	    exit
	label :
	    put "I don't understand"
    end case
end loop
put "That's all folks"
