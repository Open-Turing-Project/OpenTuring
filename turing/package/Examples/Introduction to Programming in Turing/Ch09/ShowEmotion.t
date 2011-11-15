% The "ShowEmotion" program
% Respond to various commands
var command : string
loop
    put "Enter command: " ..
    get command
    if command = "stop" then
	exit
    elsif command = "sing" then
	put "la la la"
    elsif command = "cry" then
	put "boo hoo"
    elsif command = "laugh" then
	put "ha ha ha"
    else
	put "I don't understand"
    end if
end loop
put "That's all folks"
