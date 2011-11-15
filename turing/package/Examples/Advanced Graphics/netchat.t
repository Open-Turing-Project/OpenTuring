% The "NetChat" program
const chatPort : int := 5055
var choice : int
loop
    put "Enter 1 to run chat server"
    put "Enter 2 to run chat session"
    put "Choice: " ..
    get choice
    exit when choice = 1 or choice = 2
end loop


var netStream : int
var netAddress : string

if choice = 1 then
    put "This machine is '", Net.LocalName, "' at net addresss ", Net.LocalAddress
    put "Waiting for a connection... "
    netStream := Net.WaitForConnection (chatPort, netAddress)
else
    put "Enter the address to connect to: " ..
    get netAddress
    netStream := Net.OpenConnection (netAddress, chatPort)
    if netStream <= 0 then
	put "Unable to connect to ", netAddress
	return
    end if
end if

Text.Cls
put "This machine is '", Net.LocalName, "' at net addresss ", Net.LocalAddress
put "Connected to '", Net.HostNameFromAddress (netAddress),
    "' at net address ", netAddress, "    [Ctrl+Q to quit]"
put "------------------------------------------------"
put "> "
Text.Locate (maxrow div 2 + 1, 1)
put "------------------------------------------------"
var localRow : int := 4
var localCol : int := 3
var remoteRow := maxrow div 2 + 2
var remoteCol : int := 1
var ch : char

View.Set ("noecho")
% Draw the cursors
Text.Locate (localRow, localCol)
put "_" ..
Text.Locate (remoteRow, remoteCol)
put "_" ..

loop
    % If there is a character waiting ..
    if hasch then
	% Read the character from the keyboard.
	ch := getchar

	exit when ch = KEY_CTRL_Q

	% Erase the cursor in the current position.
	Text.Locate (localRow, localCol)
	put " " ..

	if ch = '\n' then
	    localRow := localRow + 1
	    if localRow = maxrow div 2 + 1 then
		localRow := 4
	    end if
	    localCol := 3
	    Text.Locate (localRow, 1)
	    put ">"             % Clear to end of line
	    Text.Locate (localRow, localCol)
	    put : netStream, ch ..
	elsif ch = '\b' then
	    if localCol > 1 then     % Don't backspace at beginning of line
		Text.Locate (localRow, localCol - 1)
		put " " ..
		localCol -= 1
		put : netStream, ch ..
	    end if
	else
	    Text.Locate (localRow, localCol)
	    put ch ..
	    localCol += 1
	    put : netStream, ch ..
	end if

	% Check to make certain that the transmission worked
	exit when Error.Last not= 0

	% Draw the cursor in the current position.
	Text.Locate (localRow, localCol)
	put "_" ..
    end if

    if Net.CharAvailable (netStream) then
	% Read the character from the net stream
	get : netStream, ch

	% Erase the cursor in the current position.
	Text.Locate (remoteRow, remoteCol)
	put " " ..

	if ch = '\n' then
	    remoteRow := remoteRow + 1
	    if remoteRow > maxrow then
		remoteRow := maxrow div 2 + 2
	    end if
	    remoteCol := 1
	    Text.Locate (remoteRow, remoteCol)
	    put ""              % Clear to end of line
	    Text.Locate (remoteRow, remoteCol)
	elsif ch = '\b' then
	    Text.Locate (remoteRow, remoteCol - 1)
	    put " " ..
	    remoteCol -= 1
	else
	    Text.Locate (remoteRow, remoteCol)
	    put ch ..
	    remoteCol += 1
	end if

	% Draw the cursor in the current position.
	Text.Locate (remoteRow, remoteCol)
	put "_" ..
    end if
    
    exit when Error.Last not= 0
end loop

var error := Error.Last
var msg := Error.LastMsg

Text.Locate (maxrow - 1, 1)
put repeat ("=", maxcol - 1)
if error = 0 then
    Net.CloseConnection (netStream)
    put "Chat terminated by you" ..
else
    put "Chat aborted - Error #", error, ": ", msg ..
end if
