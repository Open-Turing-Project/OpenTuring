unit
class TelnetClass
    export SetDebug, SetDebugStream, SetEcho, StartCapture, StopCapture,
	SetTimeOut, Open, WaitFor, Send, SendNoNL, SetMaxCol, Close,
	AtEndOfCapture, ReadLineFromCapture, ResetCaptureRead,
	TIMEOUT_ERROR, DISCONNECT_ERROR1, DISCONNECT_ERROR2,
	CANT_CONNECT_ERROR

    % Exported constants
    const TIMEOUT_ERROR : int := 1001
    const DISCONNECT_ERROR1 : int := 1002
    const DISCONNECT_ERROR2 : int := 1003
    const CANT_CONNECT_ERROR : int := 1004

    % Telnet commands
    const SUBNEGOTIATE_BEGIN : char := chr (250)
    const SUBNEGOTIATE_END : char := chr (240)
    const WILLTEL : char := chr (251)
    const WONTTEL : char := chr (252)
    const DOTEL : char := chr (253)
    const DONTTEL : char := chr (254)
    const IAC : char := chr (255)

    % Telnet command options
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
    % Authentication Telnet options
    %
    const IS : char := chr (0)
    const AUTH : char := chr (0)
    const NULL_AUTH : char := chr (0)

    const CR : char := chr (13)
    const LINEFEED : char := chr (10)
    const NULL : char := chr (0)

    %
    % Types
    %
    type TelnetStatus : enum (Closed, Normal, ReceivedIAC, ReceivedDOTEL,
	ReceivedWILLTEL, ReceivedDONTTEL, ReceivedWONTTEL, ReceivedSUBNEG)

    type NewLineStatus : enum (Normal, CR1, NULL, CR2)

    %
    % Global variables
    %
    var conn : int := 0
    var rawBuffer : char (1000000)
    var rawBufferUsed : int := 0
    var bytesAvailable : int
    var debug : boolean := false
    var echo : boolean := false
    % Capture stream number
    var capture : boolean := false
    var timeOut : int := 30
    var maxCol : int := 0
    var telnetStatus : TelnetStatus := TelnetStatus.Closed
    var debugStream : int := 0
    var newLineStatus : NewLineStatus := NewLineStatus.Normal

    %
    var captureBuffer : char (10000000)
    var captureBufferSize : int := 0
    var captureBufferPos : int := 1


    % This handles some of the telnet option negotiation between the
    % telnet server and client.
    procedure DoTell (option : char, name : string, allow : boolean)
	% If allow is true, reply with WillTell (client is willing to send this)
	% otherwise reply with WontTell (client is unwilling to send this)
	if debug then
	    put : debugStream, "Telnet: Rcvd: Host is willing to receive ", name
	end if
	if allow then
	    if debug then
		put : debugStream, "Telnet: Send: Client is willing to send ", name
	    end if
	    write : conn, IAC, WILLTEL, option
	else
	    if debug then
		put : debugStream, "Telnet: Send: Client is *not* willing to send ", name
	    end if
	    write : conn, IAC, WONTTEL, option
	end if
    end DoTell


    % This handles some of the telnet option negotiation between the
    % telnet server and client.
    procedure WillTell (option : char, name : string, allow : boolean)
	% If pmAllow is true, reply with DoTell (client is willing to receive this)
	% otherwise reply with DontTell (client is unwilling to receive this)
	if debug then
	    put : debugStream, "Telnet: Rcvd: Host is willing to send ", name
	end if
	if allow then
	    if debug then
		put : debugStream, "Telnet: Send: Client is willing to receive ", name
	    end if
	    write : conn, IAC, DOTEL, option
	else
	    if debug then
		put : debugStream, "Telnet: Send: Client is *not* willing to receive ", name
	    end if
	    write : conn, IAC, DONTTEL, option
	end if
    end WillTell


    % This handles some of the telnet option negotiation between the
    % telnet server and client.
    procedure WontTell (option : char, name : string, allow : boolean)
	if debug then
	    put : debugStream, "Telnet: Rcvd: Host will not send ", name
	end if
    end WontTell


    % This handles some of the telnet option negotiation between the
    % telnet server and client.
    procedure DontTell (option : char, name : string, allow : boolean)
	if debug then
	    put : debugStream, "Telnet: Rcvd: Host must not receive ", name
	end if
    end DontTell


    % This opens a connection to the telnet server
    procedure Open (host : string)
	telnetStatus := TelnetStatus.Closed
	conn := Net.OpenConnectionBinary (host, Net.ipport_telnet)
	if conn <= 0 then
	    Error.SetMsg (1000,
		"Unable to open connection to " + host + ": " + Error.LastMsg)
	    conn := 0
	    return
	end if
	telnetStatus := TelnetStatus.Normal
    end Open


    % Close the Telnet connection
    procedure Close
	close : conn
	conn := 0
	telnetStatus := TelnetStatus.Closed
    end Close


    % Process raw data coming from the Telnet connection
    procedure ProcessRawData
	var subNegData : string := ""
	var ch : char
	var outputLine : string

	% Initialize the clean buffer
	if not capture then
	    captureBufferSize := 0
	end if

	% Echo the output, if desired
	if echo then
	    var col : int := 0
	    put : debugStream, "Echo: " ..
	    for i : 1 .. rawBufferUsed
		ch := rawBuffer (i)
		if ch < ' ' or ch > '~' then
		    put : debugStream, "<", ord (ch), ">" ..
		    if ch = chr (10) then
			put : debugStream, "\nCont: " ..
			col := 0
		    end if
		else
		    col += 1
		    if col > 80 then
			put : debugStream, "\nCont: " ..
			col := 1
		    end if
		    put : debugStream, ch ..
		end if
	    end for
	    put : debugStream, ""
	    put : debugStream, "-----"
	end if

	for i : 1 .. rawBufferUsed
	    ch := rawBuffer (i)
	    case telnetStatus of
		label TelnetStatus.Normal :
		    if ch = IAC then
			telnetStatus := TelnetStatus.ReceivedIAC
			newLineStatus := NewLineStatus.Normal
		    else
			% This is a non-option character - however Telnet
			% can use a <cr><lf> or <cr><null><lf> or
			% <cr>null<cr><lf> for the newline character.
			% Just to be safe, we also handle <cr>, <lf>,
			% <cr><null>, and <cr><null><cr>
			if newLineStatus = NewLineStatus.Normal then
			    if ch = LINEFEED then
				% <lf>
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
			    elsif ch = CR then
				% <cr> continue
				newLineStatus := NewLineStatus.CR1
			    else
				% anything else
				if ch not= NULL and ch not= chr (128) then
				    % Turing strings can't handle 0's and 128's.
				    captureBufferSize += 1
				    captureBuffer (captureBufferSize) := ch
				end if
			    end if
			elsif newLineStatus = NewLineStatus.CR1 then
			    if ch = LINEFEED then
				% <cr><lf> = one newline
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				newLineStatus := NewLineStatus.Normal
			    elsif ch = CR then
				% <cr><cr> = one new lines + <cr>
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
			    elsif ch = NULL then
				% <cr><null> continue
				newLineStatus := NewLineStatus.NULL
			    else
				% <cr>anything else = newline + anything else
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				if ch not= chr (128) then
				    % Turing strings can't handle 128's
				    captureBufferSize += 1
				    captureBuffer (captureBufferSize) := ch
				end if
				newLineStatus := NewLineStatus.Normal
			    end if
			elsif newLineStatus = NewLineStatus.NULL then
			    if ch = LINEFEED then
				% <cr><null><lf> = one newline
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				newLineStatus := NewLineStatus.Normal
			    elsif ch = CR then
				% <cr><null><cr> continue
				newLineStatus := NewLineStatus.CR2
			    else
				% <cr><null>anything else = one newline + anything else
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				if ch not= NULL and ch not= chr (128) then
				    % Turing strings can't handle 0's or 128's
				    captureBufferSize += 1
				    captureBuffer (captureBufferSize) := ch
				end if
				newLineStatus := NewLineStatus.Normal
			    end if
			elsif newLineStatus = NewLineStatus.CR2 then
			    if ch = LINEFEED then
				% <cr><null><cr><lf> = one newline
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				newLineStatus := NewLineStatus.Normal
			    elsif ch = CR then
				% <cr><null><cr><cr> = two newline + <cr>
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				newLineStatus := NewLineStatus.CR1
			    else
				% <cr><null><cr>anything else = two newline + anything else
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\r'
				captureBufferSize += 1
				captureBuffer (captureBufferSize) := '\n'
				if ch not= NULL and ch not= chr (128) then
				    % Turing strings can't handle 0's or 128's
				    captureBufferSize += 1
				    captureBuffer (captureBufferSize) := ch
				end if
				newLineStatus := NewLineStatus.Normal
			    end if
			end if
		    end if
		label TelnetStatus.ReceivedIAC :
		    if ch = DOTEL then
			telnetStatus := TelnetStatus.ReceivedDOTEL
		    elsif ch = WILLTEL then
			telnetStatus := TelnetStatus.ReceivedWILLTEL
		    elsif ch = DONTTEL then
			telnetStatus := TelnetStatus.ReceivedDONTTEL
		    elsif ch = WONTTEL then
			telnetStatus := TelnetStatus.ReceivedWONTTEL
		    elsif ch = SUBNEGOTIATE_BEGIN then
			subNegData := ""
			telnetStatus := TelnetStatus.ReceivedSUBNEG
		    else
			if debug then
			    put : debugStream, "Telnet: Confused - received",
				" a ", ord (ch), " in ReceivedIAC mode"
			end if
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
			    if debug then
				put : debugStream, "Telnet: Rcvd DOTEL ",
				    "AUTHENTICATION"
				put : debugStream, "Telnet: Sent WILLTEL ",
				    "AUTHENTICATION"
			    end if
			    write : conn, IAC, WILLTEL, AUTHENTICATION
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
		label TelnetStatus.ReceivedDONTTEL :
		    case ch of
			label ECHO :
			    DontTell (ECHO, "ECHO", false)
			label TERMINAL_TYPE :
			    DontTell (TERMINAL_TYPE, "TERMINAL TYPE", false)
			label WINDOW_SIZE :
			    DontTell (WINDOW_SIZE, "WINDOW SIZE", false)
			label TERMINAL_SPEED :
			    DontTell (TERMINAL_SPEED, "TERMINAL SPEED", false)
			label FLOW_CONTROL :
			    DontTell (FLOW_CONTROL, "FLOW CONTROL", false)
			label X_DISPLAY_LOCATION :
			    DontTell (X_DISPLAY_LOCATION,
				"X WINDOWS DISPLAY LOCATION", false)
			label TELNET_ENVIRONMENT_OPTION :
			    DontTell (TELNET_ENVIRONMENT_OPTION,
				"TELNET ENVIRONMENT OPTIONS", false)
			label AUTHENTICATION :
			    DontTell (AUTHENTICATION, "AUTHENTICATION", false)
			label ENCRYPT :
			    DontTell (ENCRYPT, "ENCRYPTION", false)
			label ENVIRONMENT_VARS :
			    DontTell (ENVIRONMENT_VARS, "ENVIRONMENT VARIABLES",
				false)
			label :
			    % We won't accept encryption
			    DontTell (ch, "Option#" + ch, false)
		    end case
		    telnetStatus := TelnetStatus.Normal
		label TelnetStatus.ReceivedWILLTEL :
		    case ch of
			label ECHO :
			    % We don't accept echo
			    WillTell (ECHO, "ECHO", false)
			label SUPPRESS_GO_AHEAD :
			    % We do accept suppress go ahead
			    WillTell (SUPPRESS_GO_AHEAD, "SUPPRESS GO AHEAD", true)
			label STATUS :
			    % We don't accept status
			    WillTell (STATUS, "STATUS", false)
			label ENCRYPT :
			    % We won't accept encryption
			    WillTell (ENCRYPT, "ENCRYPT", false)
			label :
			    % We won't accept encryption
			    WillTell (ch, "Option#" + ch, false)
		    end case
		    telnetStatus := TelnetStatus.Normal
		label TelnetStatus.ReceivedWONTTEL :
		    case ch of
			label ECHO :
			    WontTell (ECHO, "ECHO", false)
			label SUPPRESS_GO_AHEAD :
			    WontTell (SUPPRESS_GO_AHEAD, "SUPPRESS GO AHEAD", true)
			label STATUS :
			    WontTell (STATUS, "STATUS", false)
			label ENCRYPT :
			    WontTell (ENCRYPT, "ENCRYPT", false)
			label :
			    WontTell (ch, "Option#" + ch, false)
		    end case
		    telnetStatus := TelnetStatus.Normal
		label TelnetStatus.ReceivedSUBNEG :
		    case ch of
			label SUBNEGOTIATE_END :
			    % Send we don't do this.
			    if subNegData (1) = AUTHENTICATION then
				if debug then
				    put : debugStream, "Telnet: Rcvd SUBNEGOTIATE ",
					"AUTHENTICATION"
				    put : debugStream, "Telnet: Sent SUBNEGOTIATE ",
					"NULL-NULL AUTHENTICATION"
				end if
				write : conn, IAC, SUBNEGOTIATE_BEGIN,
				    AUTHENTICATION, IS, NULL_AUTH, NULL_AUTH,
				    AUTH, IAC, SUBNEGOTIATE_END
			    else
				if debug then
				    put : debugStream, "Telnet: Rcvd SB ",
					subNegData, "SE"
				end if
			    end if
			    % put : debugStream,  "Telnet: Sent DONTTEL " & "ECHO")
			    % mWinsock.SendData Chr$(IAC) & Chr$(DONTTEL) & _
			    % Chr$(ECHO)
			    telnetStatus := TelnetStatus.Normal
			label :
			    subNegData := subNegData + ch
		    end case
	    end case
	end for

	% if capture > 0 then
	%     if maxCol = 0 then
	%         write : capture, cleanBuffer : cleanBufferUsed
	%     else
	%         var startPtr, ptr, col := 1
	%         loop
	%             if cleanBuffer (ptr) = chr (13) then
	%                 outputLine := cleanBuffer (startPtr .. ptr)
	%                 write : capture, outputLine : ptr - startPtr + 1
	%                 startPtr := ptr + 1
	%             else
	%                 if ptr - startPtr = maxCol then
	%                     outputLine := cleanBuffer (startPtr .. ptr - 1)
	%                     write : capture, outputLine : maxCol
	%                     outputLine := "\r\n"
	%                     write : capture, outputLine : 2
	%                     startPtr := ptr
	%                 end if
	%             end if
	%             ptr += 1
	%             exit when ptr > cleanBufferUsed
	%         end loop
	%         outputLine := cleanBuffer (startPtr .. ptr - 1)
	%         write : capture, outputLine : ptr - startPtr
	%     end if
	% elsif capture = -1 then
	%     if maxCol = 0 then
	%         Str.CharArrayCopy (cleanBuffer, 1,
	%             captureBuffer, captureBufferSize, cleanBufferUsed)
	%         captureBufferSize += cleanBufferUsed
	%     else
	%         var startPtr, ptr, col := 1
	%         loop
	%             if cleanBuffer (ptr) = chr (10) then
	%                 Str.CharArrayCopy (cleanBuffer, startPtr,
	%                     captureBuffer, captureBufferSize,
	%                     ptr - startPtr + 1)
	%                 captureBufferSize += ptr - startPtr + 1
	%                 startPtr := ptr + 1
	%             else
	%                 if ptr - startPtr = maxCol then
	%                     Str.CharArrayCopy (cleanBuffer, startPtr,
	%                         captureBuffer, captureBufferSize, maxCol)
	%                     captureBufferSize += maxCol
	%                     Str.WriteLineToCharArray (captureBuffer, "",
	%                         captureBufferSize)
	%                     startPtr := ptr
	%                 end if
	%             end if
	%             ptr += 1
	%             exit when ptr > cleanBufferUsed
	%         end loop
	%         Str.CharArrayCopy (cleanBuffer, startPtr,
	%             captureBuffer, captureBufferSize, ptr - startPtr)
	%         captureBufferSize += ptr - startPtr
	%     end if
	% end if
    end ProcessRawData

    % Set the debugging flag
    procedure SetDebug (initDebug : boolean)
	debug := initDebug
    end SetDebug

    % Set the debugging stream
    procedure SetDebugStream (initDebugStream : int)
	debugStream := initDebugStream
    end SetDebugStream

    % Set the echo flag
    procedure SetEcho (initEcho : boolean)
	echo := initEcho
    end SetEcho

    % Set the time out size (in seconds)
    procedure SetTimeOut (initTimeOut : int)
	timeOut := initTimeOut
    end SetTimeOut

    % Will wait for a message that ends in 's', or times out after
    % timeOut seconds.  If 's' is empty, it returns after each message.
    procedure WaitFor (s : string)
	var startTime := Time.Sec
	var status : int := -99
	var lastStr : string

	if conn = 0 then
	    Error.AbortMsg (1005,
		"TelnetClass crashed attempting to read from " +
		"closed connection.")
	    assert false
	end if

	loop
	    loop
		exit when Net.ConnectionStatus (conn) not= Net.connection_open
		bytesAvailable := Net.BytesAvailable (conn)
		exit when bytesAvailable > 0 or
		    Time.Sec - startTime > timeOut
		delay (100)
	    end loop
	    if Net.ConnectionStatus (conn) not= Net.connection_open then
		Error.SetMsg (1004, "Telnet disconnect")
		return
	    end if
	    if bytesAvailable = 0 then
		put : debugStream, "Timeout started at " +
		    Time.SecDate (startTime) + " and ended at " + Time.Date
		Error.SetMsg (TIMEOUT_ERROR, "Timeout waiting to read data")
		return
	    end if
	    if debug then
		put : debugStream, "***: About to read ", bytesAvailable, " bytes"
	    end if
	    if bytesAvailable > sizeof (rawBuffer) then
		Error.AbortMsg (1002,
		    "TelnetClass crashed attempting to read " +
		    intstr (bytesAvailable) + " bytes when maximum of " +
		    intstr (sizeof (rawBuffer)) +
		    " bytes can be read at a time")
		assert false
	    else
		read : conn : status, rawBuffer : bytesAvailable : rawBufferUsed
		if debug and rawBufferUsed not= bytesAvailable then
		    put : debugStream, "Only ", rawBufferUsed, " bytes read"
		    put : debugStream, "ErrMsg: ", Error.LastMsg
		end if
	    end if

	    ProcessRawData

	    % There's trouble if we didn't read the expected number of
	    % bytes (probably because of a disconnect).
	    if rawBufferUsed not= bytesAvailable then
		if Net.ConnectionStatus (conn) not= Net.connection_open then
		    Error.SetMsg (1004, "Telnet disconnect")
		    return
		end if
		Error.AbortMsg (1003,
		    "TelnetClass crashed attempting to read " +
		    intstr (bytesAvailable) + "bytes when only " +
		    intstr (rawBufferUsed) + "bytes were actually read")
		assert false
	    end if

	    % If s is the empty string, empty immediately
	    exit when s = ""

	    % Exit when there's a match
	    if captureBufferSize >= length (s) then
		lastStr := captureBuffer (captureBufferSize - length (s) + 1 .. captureBufferSize)
		if debug then
		    put : debugStream, "Compare '", s, "' '", lastStr, "'"
		end if
		exit when Str.Lower (s) = Str.Lower (lastStr)
	    end if
	end loop

	if debug then
	    if s = "" then
		put : debugStream, "Return from WaitFor"
	    else
		put : debugStream, "Return from WaitFor on match of '", s, "'"
	    end if
	end if
    end WaitFor


    % Send text to the telnet server without an appended newline
    procedure SendNoNL (s : string)
	if conn = 0 then
	    Error.AbortMsg (1006,
		"TelnetClass crashed attempting to write to " +
		"closed connection.")
	    assert false
	end if
	if debug then
	    if s (*) = "\n" then
		put : debugStream, "Sent: ", s ..
	    else
		put : debugStream, "Sent: ", s
	    end if
	end if
	write : conn, s : length (s)
    end SendNoNL


    % Send text to the telnet server with an appended newline
    procedure Send (s : string)
	var tempStr : string := s
	if Str.Right (s, 2) not= "\r\n" then
	    if Str.Right (s, 1) not= "\n" then
		tempStr := s + "\r\n"
	    else
		tempStr := s (1 .. * -1) + "\r\n"
	    end if
	end if
	SendNoNL (tempStr)
    end Send


    % Capture all the data sent to the client
    procedure StartCapture (append : boolean)
	if not append then
	    captureBufferSize := 0
	end if
	capture := true
    end StartCapture


    procedure StopCapture
	capture := false
    end StopCapture




    % Set the maximum number of columns before folding long lines with a
    % newline.
    procedure SetMaxCol (initMaxCol : int)
	maxCol := initMaxCol
    end SetMaxCol


    % Return whether we've returned all the data capture to char array
    function AtEndOfCapture : boolean
	result captureBufferPos > captureBufferSize
    end AtEndOfCapture


    % Read a line of the data captured to the char array.
    function ReadLineFromCapture : string
	var res : string
	if captureBufferPos > captureBufferSize then
	    result ""
	end if
	Str.ReadLineFromCharArray (captureBuffer, captureBufferSize,
	    res, captureBufferPos)
	result res
    end ReadLineFromCapture


    % Reset the pointer so that ReadLineCapture starts from the beginning
    % of the data captured to the char array.
    procedure ResetCaptureRead
	captureBufferPos := 1
    end ResetCaptureRead
end TelnetClass
