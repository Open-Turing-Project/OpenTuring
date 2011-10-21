% The "TuringTelnet" program.

% ' Telnet commands
const SUBNEGOTIATE_BEGIN : char := chr (250)
const SUBNEGOTIATE_END : char := chr (240)
const WILLTEL : char := chr (251)
const WONTTEL : char := chr (252)
const DOTEL : char := chr (253)
const DONTTEL : char := chr (254)
const IAC : char := chr (255)

% ' Telnet command options
const ECHO : char := chr (1)
const SUPPRESS_GO_AHEAD : char := chr (3)
const STATUS : char := chr (5)
const TERMINAL_TYPE : char := chr (24)
const WINDOW_SIZE : char := chr (31)
const TERMINAL_SPEED : char := chr (32)
const FLOW_CONTROL : char := chr (33)
const X_DISPLAY_LOCATION : char := chr (35)
const ENVIRONMENT_VARS : char := chr (36)
const AUTHENTICATION : char := chr (37)
const ENCRYPT : char := chr (38)
const TELNET_ENVIRONMENT_OPTION : char := chr (39)
%
% ' Authentication Telnet options
const IS : char := chr (0)
const AUTH : char := chr (0)
const NULL_AUTH : char := chr (0)

var bytes : char (1000)
var bytesAvailable : int
type TelnetStatus : enum (Normal, ReceivedIAC, ReceivedDOTEL, ReceivedWILLTEL,
    ReceivedSUBNEG)
var telnetStatus : TelnetStatus := TelnetStatus.Normal

procedure DoTell (option : char, name : string, allow : boolean)
    % If allow is true, reply with WillTell (client is willing to send this)
    % otherwise reply with WontTell (client is unwilling to send this)
    put : 0, "Telnet: Rcvd: Host is willing to receive ", name
    if allow then
	put : 0, "Telnet: Send: Client is willing to send ", name
	% mWinsock.SendData Chr(IAC) & Chr(WILLTEL) & Chr(option)
    else
	put : 0, "Telnet: Send: Client is *not* willing to send ", name
	% mWinsock.SendData Chr(IAC) & Chr(WONTTEL) & Chr(option)
    end if
end DoTell


procedure HandleData (bytes : char (*), size : int)
    var stringData : string := ""
    var subNegData : string := ""

    for i : 1 .. size
	var ch : char := bytes (i)
	if ch < ' ' or ch > '~' then
	    put : 0, "<", ord (ch), ">" ..
	else
	    put : 0, ch ..
	end if

	case telnetStatus of
	    label TelnetStatus.Normal :
		if ch = IAC then
		    telnetStatus := TelnetStatus.ReceivedIAC
		else
		    if ch not= chr (13) and ch not= chr (10) then
			stringData += ch
		    end if
		end if
	    label TelnetStatus.ReceivedIAC :
		if ch = DOTEL then
		    telnetStatus := TelnetStatus.ReceivedDOTEL
		elsif ch = WILLTEL then
		    telnetStatus := TelnetStatus.ReceivedWILLTEL
		elsif ch = SUBNEGOTIATE_BEGIN then
		    subNegData := ""
		    telnetStatus := TelnetStatus.ReceivedSUBNEG
		else
		    put : 0, "Telnet: Confused - received",
			" a ", ord (ch), " in ReceivedIAC mode"
		    telnetStatus := TelnetStatus.Normal
		end if
	    label TelnetStatus.ReceivedDOTEL :
		case ch of
		    label ECHO :
			% We don't send echo
			DoTell (ECHO, "ECHO", false)
		    label TERMINAL_TYPE :
			% We don't send terminal type
			DoTell (TERMINAL_TYPE, "TERMINAL TYPE", false)
		    label WINDOW_SIZE :
			% We don't send window size
			DoTell (WINDOW_SIZE, "WINDOW SIZE", false)
		    label TERMINAL_SPEED :
			% We don't send terminal speed
			DoTell (TERMINAL_SPEED, "TERMINAL SPEED", false)
		    label FLOW_CONTROL :
			% We don't send flow control
			DoTell (FLOW_CONTROL, "FLOW CONTROL", false)
		    label X_DISPLAY_LOCATION :
			% We don't send X windows display location
			DoTell (X_DISPLAY_LOCATION,
			    "X WINDOWS DISPLAY LOCATION", false)
		    label TELNET_ENVIRONMENT_OPTION :
			% We don't send X windows display location
			DoTell (TELNET_ENVIRONMENT_OPTION,
			    "TELNET ENVIRONMENT OPTIONS", false)
		    label AUTHENTICATION :
			% Send we do do this
			put : 0, "Telnet: Rcvd DOTEL ",
			    "AUTHENTICATION"
			put : 0, "Telnet: Sent WILLTEL ",
			    "AUTHENTICATION"
			% mWinsock.SendData Chr$ (IAC) & Chr$ (WILLTEL) & _
			% Chr$ (AUTHENTICATION)
		    label ENCRYPT :
			% We don't send encryption
			DoTell (ENCRYPT, "ENCRYPTION", false)
		    label ENVIRONMENT_VARS :
			% We don't send environment variables
			DoTell (ENVIRONMENT_VARS, "ENVIRONMENT VARIABLES",
			    false)
		    label :
			% We won't accept encryption
			DoTell (ch, "Option#" + ch, false)
		end case
		telnetStatus := TelnetStatus.Normal
	    label TelnetStatus.ReceivedWILLTEL :
		%                     case ch of
		%                         label ECHO:
		%                             ' We don't accept echo
		%                             Call WillTell(ECHO, "ECHO", false)
		%                         label SUPPRESS_GO_AHEAD:
		%                             ' We do accept suppress go ahead
		%                             Call WillTell(SUPPRESS_GO_AHEAD, "SUPPRESS GO AHEAD", True)
		%                         label STATUS:
		%                             ' We don't accept status
		%                             Call WillTell(STATUS, "STATUS", false)
		%                         label ENCRYPT:
		%                             ' We won't accept encryption
		%                             Call WillTell(ENCRYPT, "ENCRYPT", false)
		%                         label Else
		%                             ' We won't accept encryption
		%                             Call WillTell(ch, "Option#" & ch, false)
		%                     End Select
		%                     telnetStatus := TelnetStatus.Normal
	    label TelnetStatus.ReceivedSUBNEG :
		%                     case ch of
		%                         label SUBNEGOTIATE_END:
		%                             ' Send we don't do this.
		%                             if Left(mySubNegData, 1) = Chr(AUTHENTICATION) then
		%                                 put : 0,  "Telnet: Rcvd SUBNEGOTIATE " & _
		%                                         "AUTHENTICATION")
		%                                 put : 0,  "Telnet: Sent SUBNEGOTIATE " & _
		%                                         "NULL-NULL AUTHENTICATION")
		%                                 mWinsock.SendData Chr$(IAC) & _
		%                                         Chr(SUBNEGOTIATE_BEGIN) & _
		%                                         Chr(AUTHENTICATION) & _
		%                                         Chr(IS_) & _
		%                                         Chr(NULL_AUTH) & Chr(NULL_AUTH) & _
		%                                         Chr(AUTH) & _
		%                                         Chr(IAC) & Chr(SUBNEGOTIATE_END)
		%                             Else
		%                                 put : 0,  "Telnet: Rcvd SB " & _
		%                                         mySubNegData & "SE")
		%                             End if
		% '                            put : 0,  "Telnet: Sent DONTTEL " & _
		% '                                    "ECHO")
		% '                            mWinsock.SendData Chr$(IAC) & Chr$(DONTTEL) & _
		% '                                    Chr$(ECHO)
		%                             telnetStatus := TelnetStatus.Normal
		%                         default:
		%                             mySubNegData = mySubNegData & Chr(ch)
		%                     End Select
	end case
    end for
end HandleData

%         For i = 0 To pmBytesTotal - 1
%             ch = chs(i)
%         Next i
%     End if
%
%     if myStringData <> "" then
%         put : 0,  "Telnet: Rcvd '" & myStringData & "' from Host")
%     End if
%
%     if LCase(Right(myStringData, 7)) = "login: " then
%         ' Send login name
%         put : 0,  "Telnet: Received login prompt from host")
%         Call gMAIN_Main.WriteFeedback("Telnet protocol negotiated succesfully " & _
%                 "- login prompt received")
%         mWinsock.SendData mUserName & vbNewLine
%         put : 0,  "Telnet: Replied '" & mUserName & "'")
%     End if
%
%     if LCase(Right(myStringData, 10)) = "password: " then
%         ' Send password
%         put : 0,  "Telnet: Received password prompt from host")
%         mWinsock.SendData mPassword & vbNewLine
%         put : 0,  "Telnet: Replied '" & mPassword & "'")
%     End if

const host : string := "192.168.123.6"

var conn : int

conn := Net.OpenConnectionBinary (host, Net.ipport_telnet)
if conn <= 0 then
    put "Unable to open connection to ", host, ": ", Error.LastMsg
    assert false
end if

loop
    loop
	bytesAvailable := Net.BytesAvailable (conn)
	exit when bytesAvailable > 0
	delay (100)
    end loop
    put "About to read ", bytesAvailable, " bytes"
    read : conn, bytes : bytesAvailable
    HandleData (bytes, bytesAvailable)
end loop
