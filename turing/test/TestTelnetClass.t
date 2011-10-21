% The "NetAudit" Program.
import TelnetClass

% Constants
const host : string := "192.168.123.6"
const userName : string := "tom"
const password : string := ""
const tmpName2 : string := "tmp_netaudit.txt"

var debugStream : int

put "*** Getting NetAudit ***"

debugStream := Window.Open ("text")
var telnet : ^TelnetClass

Window.Select (defWinID)

function WaitFor (waitFor : string) : boolean
    telnet -> WaitFor (waitFor)
    if Error.Last = TelnetClass.TIMEOUT_ERROR then
	put : debugStream, "[Telnet Timed Out]"
	put "Telnet timed out - aborting TLF"
	Window.Show (debugStream)
	result false
    elsif Error.Last = TelnetClass.DISCONNECT_ERROR1 then
	put : debugStream, "[Telnet Disconnected (1)]"
	put "Telnet disconnected - aborting TLF"
	Window.Show (debugStream)
	result false
    elsif Error.Last = TelnetClass.DISCONNECT_ERROR2 then
	put : debugStream, "[Telnet Disconnected (2)]"
	put "Telnet disconnected - aborting TLF"
	Window.Show (debugStream)
	result false
    end if
    result true
end WaitFor

function SendAndWait (sendCmd, waitFor : string) : boolean
    telnet -> Send (sendCmd)
    result WaitFor (waitFor)
end SendAndWait



new telnet

telnet -> SetDebugStream (debugStream)
telnet -> SetDebug (true)
%telnet -> SetEcho (true)

put "Opening connection to " + host

% Open
telnet -> Open (host)
if Error.Last not= 0 then
    put "Unable to open connection"
    return
end if

put "Connected"
if not WaitFor ("login: ") then
    return
end if
put "Starting logon sequence"
if not SendAndWait ("tom", "Password:") then
    return
end if
put "Sending password"
if not SendAndWait ("", "G4:~ tom$ ") then
    return
end if
put "Sending change directory"
if not SendAndWait ("cd Documents", "tom$ ") then
    return
end if
put "Getting Netaudit"
telnet -> StartCapture (false)
telnet -> Send ("cat nums.dat")
telnet -> SetTimeOut (2)
telnet -> WaitFor ("tom$ ")

var newWin : int
newWin := Window.Open ("text")

put "Processing NetAudit"

var line : string

var i : int := 1
loop
    loop
	exit when telnet -> AtEndOfCapture
	line := telnet -> ReadLineFromCapture
	if line not= intstr (i) then
	    put "Should be ", i, " read '", line, "'"
	    return
	end if
	if i mod 1000 = 1 then
	    put i
	end if
	i += 1
    end loop
    put "Wait again"
    telnet -> WaitFor ("tom$ ")
end loop

Window.Close (debugStream)
