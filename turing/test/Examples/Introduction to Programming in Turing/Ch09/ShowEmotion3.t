% The "ShowEmotion3" program
% Here we select commands by entering a string
var command : string
loop
    put "Choose from sing, cry, laugh, stop: " ..
    get command
    case command of
	label "sing" :
	    put "la la la"
	label "cry" :
	    put "boo hoo"
	label "laugh" :
	    put "ha ha ha"
	label "stop" :
	    exit
	label :
	    put "I don't understand"
    end case
end loop
put "That's all folks"
