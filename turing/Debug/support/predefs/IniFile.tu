unit
module pervasive IniFile
    export Open, GetStringDefault, GetString, GetBooleanDefault, GetBoolean,
	GetIntDefault, GetInt, GetRealDefault, GetReal,
	GetStringArray, GetBooleanArray, GetIntArray, GetRealArray,
	Delete, SetString, SetBoolean, SetInt, SetReal,
	SetStringArray, SetBooleanArray, SetIntArray, SetRealArray,
	SetBufferWrite, Write, StartSection, GetSection,
	StartProperty, GetProperty, GetType,
    % Constant
	BAD_VALUE

    type Property :
	record
	    name : string
	    value : string
	    comment : boolean
	    quotes : boolean
	    next : ^Property
	end record
    type Section :
	record
	    name : string
	    propHead : ^Property
	    next : ^Section
	end record

    % The head
    var head : ^Section := nil
    % Are changes buffered
    var bufferWrite : boolean := false
    % The name of the iniFile
    var iniFilePath : string := ""
    % Used for listing sections/properties
    var listSection : ^Section := nil
    var listProperty : ^Property := nil

    % Constant
    const BAD_VALUE : int := -9999
    const DEFAULT : string := "default"

    procedure FreePreviousValues
	var sectPtr, lastSectPtr : ^Section
	var propPtr, lastPropPtr : ^Property

	sectPtr := head
	loop
	    exit when sectPtr = nil
	    propPtr := sectPtr -> propHead
	    loop
		exit when propPtr = nil
		lastPropPtr := propPtr
		propPtr := propPtr -> next
		free lastPropPtr
	    end loop
	    lastSectPtr := sectPtr
	    sectPtr := sectPtr -> next
	    free sectPtr
	end loop
	head := nil
    end FreePreviousValues


    procedure Echo
	var sectPtr : ^Section := head
	var propPtr : ^Property
	loop
	    exit when sectPtr = nil
	    propPtr := sectPtr -> propHead
	    put "S: ", sectPtr -> name
	    loop
		exit when propPtr = nil
		if propPtr -> comment then
		    put "   P: ", propPtr -> name, " (C)"
		else
		    put "   P: ", propPtr -> name
		end if
		propPtr := propPtr -> next
	    end loop
	    sectPtr := sectPtr -> next
	end loop
    end Echo


    procedure Open (pathName : string)
	var iniFile : int
	var line : string
	var lineCounter : int := 0
	var pos : int
	var sectPtr, lastSectPtr : ^Section
	var propPtr, lastPropPtr : ^Property

	% Set error to 0
	Error.Set (0)

	iniFilePath := pathName
	if index (iniFilePath, ".") = 0 then
	    iniFilePath := iniFilePath + ".ini"
	end if

	FreePreviousValues

	open : iniFile, iniFilePath, get
	if iniFile <= 0 then
	    return
	end if

	% Create the blank section to start
	new sectPtr
	sectPtr -> name := ""
	sectPtr -> next := nil
	propPtr := nil
	head := sectPtr

	loop
	    exit when eof (iniFile)

	    get : iniFile, line : *
	    line := Str.Trim (line)
	    lineCounter += 1
	    if line not= "" then
		if line (1) = "[" then
		    if index (line, "]") not= length (line) then
			% Error, bad section on line lineCounter
			Error.SetMsg (eIniFileBadSection,
			    "Bad Section on line " + intstr (lineCounter) +
			    " of '" + iniFilePath + "'")
			close : iniFile
			return
		    end if
		    lastSectPtr := sectPtr
		    new sectPtr
		    lastSectPtr -> next := sectPtr
		    sectPtr -> name := line (2 .. * -1)
		    sectPtr -> next := nil
		    propPtr := nil
		else % Does not open with '['
		    % If there's a single equals sign, assume a property/value
		    % pair (first char must be alphanumeric).  If
		    % there's no equal sign, and it's got a non-alphanumeric
		    % first character, assume it's a comment
		    if ('a' <= line (1) and line (1) <= 'z') or
			    ('A' <= line (1) and line (1) <= 'Z') or
			    ('0' <= line (1) and line (1) <= '9') then
			pos := index (line, "=")
			if pos = 0 then
			    % Error, missing = on line lineCounter
			    Error.SetMsg (eIniFileMissingEquals,
				"Missing '=' on line " + intstr (lineCounter) +
				" of '" + iniFilePath + "'")
			    close : iniFile
			    return
			end if
			lastPropPtr := propPtr
			new propPtr
			if lastPropPtr = nil then
			    sectPtr -> propHead := propPtr
			else
			    lastPropPtr -> next := propPtr
			end if
			propPtr -> name := Str.Trim (line (1 .. pos - 1))
			propPtr -> value := Str.Trim (line (pos + 1 .. *))
			if length (propPtr -> value) > 0 and
				propPtr -> value (1) = "\"" and
				propPtr -> value (*) = "\"" then
			    propPtr -> value := propPtr -> value (2 .. * -1)
			    propPtr -> quotes := true
			else
			    propPtr -> quotes := false
			end if
			propPtr -> comment := false
			propPtr -> next := nil
		    else % Does not start with alphanumeric
			% Comment
			lastPropPtr := propPtr
			new propPtr
			if lastPropPtr = nil then
			    sectPtr -> propHead := propPtr
			else
			    lastPropPtr -> next := propPtr
			end if
			propPtr -> name := line
			propPtr -> value := ""
			propPtr -> comment := true
			propPtr -> next := nil
		    end if % if/else - starts with alphanumeric
		end if % if/else - starts with '['
	    end if     % if - not empty line
	end loop
	close : iniFile
    end Open


    function GetStringDefault (sectionName, propertyName, default : string) : string
	var sectPtr : ^Section := head
	var propPtr : ^Property

	% Set error to 0
	Error.Set (0)

	loop
	    exit when sectPtr = nil or
		sectPtr -> name = sectionName or
		(sectPtr -> name = "" and sectionName = DEFAULT)
	    sectPtr := sectPtr -> next
	end loop
	if sectPtr = nil then
	    % Section not found error
	    Error.SetMsg (eIniFileSectionNotFound,
		"No '" + sectionName + "' section in '" + iniFilePath + "'")
	    result default
	end if
	propPtr := sectPtr -> propHead
	loop
	    exit when propPtr = nil or
		propPtr -> name = propertyName and
		not propPtr -> comment
	    propPtr := propPtr -> next
	end loop
	if propPtr = nil then
	    % Property not found
	    Error.SetMsg (eIniFilePropertyNotFound,
		"No '" + propertyName + "' property in '" +
		sectionName + "' section of '" + iniFilePath + "'")
	    result default
	end if
	result propPtr -> value
    end GetStringDefault


    function GetString (sectionName, propertyName : string) : string
	result GetStringDefault (sectionName, propertyName, "")
    end GetString


    function GetBooleanDefault (sectionName, propertyName : string,
	    default : boolean) : boolean
	var s : string := GetString (sectionName, propertyName)

	if Error.Last not= 0 then
	    result default
	end if
	if Str.Lower (s) = "true" then
	    result true
	elsif Str.Lower (s) = "false" then
	    result false
	else
	    % s is not 'true' or 'false'
	    Error.SetMsg (eIniFilePropertyNotBoolean,
		"Value of '" + propertyName + "' property is not 'true' or " +
		"'false' (value is '" + s + "')")
	    result default
	end if
    end GetBooleanDefault


    function GetBoolean (sectionName, propertyName : string) : boolean
	result GetBooleanDefault (sectionName, propertyName, false)
    end GetBoolean


    function GetIntDefault (sectionName, propertyName : string,
	    default : int) : int
	var s : string := GetStringDefault (sectionName, propertyName, "[xxx]")

	if s = "[xxx]" then
	    result default
	end if
	if Error.Last not= 0 then
	    result IniFile.BAD_VALUE
	end if
	if strintok (s) then
	    result strint (s)
	end if
	% Error value not integer
	Error.SetMsg (eIniFilePropertyNotInt,
	    "Value of '" + propertyName + "' property is not integer " +
	    "(value is '" + s + "')")
	result IniFile.BAD_VALUE
    end GetIntDefault


    function GetInt (sectionName, propertyName : string) : int
	result GetIntDefault (sectionName, propertyName, 0)
    end GetInt


    function GetRealDefault (sectionName, propertyName : string,
	    default : real) : real
	var s : string := GetStringDefault (sectionName, propertyName, "[xxx]")

	if s = "[xxx]" then
	    result default
	end if
	if Error.Last not= 0 then
	    result IniFile.BAD_VALUE
	end if
	if strrealok (s) then
	    result strreal (s)
	end if
	% Error value not integer
	Error.SetMsg (eIniFilePropertyNotReal,
	    "Value of '" + propertyName + "' property is not real " +
	    "(value is '" + s + "')")
	result IniFile.BAD_VALUE
    end GetRealDefault


    function GetReal (sectionName, propertyName : string) : real
	result GetRealDefault (sectionName, propertyName, 0.0)
    end GetReal

    
    procedure GetStringArray (sectionName, propertyName : string,
	    var numValues : int, var values : array 1 .. * of string)
	var counter : int := 1
	var maxSize : int := upper (values)
	loop
	    exit when counter > maxSize
	    values (counter) := GetString (sectionName,
		propertyName + "." + intstr (counter))
	    exit when values (counter) = ""
	    counter += 1
	end loop
	if Error.Last = 0 and counter > maxSize then
	    Error.Set (eIniFileArrayTooSmall)
	end if
	numValues := counter - 1
    end GetStringArray


    procedure GetBooleanArray (sectionName, propertyName : string,
	    var numValues : int, var values : array 1 .. * of boolean)

	var strValues : array 1 .. upper (values) of string

	GetStringArray (sectionName, propertyName,
	    numValues, strValues)

	for i : 1 .. numValues
	    if Str.Lower (strValues (i)) = "true" then
		values (i) := true
	    elsif Str.Lower (strValues (i)) = "false" then
		values (i) := false
	    else
		% s is not 'true' or 'false'
		Error.SetMsg (eIniFilePropertyNotBoolean,
		    "Value of '" + propertyName + "." + intstr (i) +
		    "' property is not 'true' or " +
		    "'false' (value is '" + strValues (i) + "')")
		numValues := i - 1
		return
	    end if
	end for
    end GetBooleanArray


    procedure GetIntArray (sectionName, propertyName : string,
	    var numValues : int, var values : array 1 .. * of int)

	var strValues : array 1 .. upper (values) of string

	GetStringArray (sectionName, propertyName,
	    numValues, strValues)

	for i : 1 .. numValues
	    if strintok (strValues (i)) then
		values (i) := strint (strValues (i))
	    else
		% Error value not integer
		Error.SetMsg (eIniFilePropertyNotInt,
		    "Value of '" + propertyName + "." + intstr (i) +
		    "' property is not integer " +
		    "(value is '" + strValues (i) + "')")
		numValues := i - 1
		return
	    end if
	end for
    end GetIntArray


    procedure GetRealArray (sectionName, propertyName : string,
	    var numValues : int, var values : array 1 .. * of real)

	var strValues : array 1 .. upper (values) of string

	GetStringArray (sectionName, propertyName,
	    numValues, strValues)

	for i : 1 .. numValues
	    if strrealok (strValues (i)) then
		values (i) := strreal (strValues (i))
	    else
		% Error value not integer
		Error.SetMsg (eIniFilePropertyNotInt,
		    "Value of '" + propertyName + "." + intstr (i) +
		    "' property is not integer " +
		    "(value is '" + strValues (i) + "')")
		numValues := i - 1
		return
	    end if
	end for
    end GetRealArray


    % Write the ini file out
    procedure Write
	var iniFile : int
	var line : string
	var lineCounter : int := 0
	var pos : int
	var sectPtr : ^Section := head
	var propPtr : ^Property

	% Set error to 0
	Error.Set (0)

	if iniFilePath = "" then
	    % Abort with no file opened
	    Error.Abort (eIniFileNoIniFileOpen)
	    return
	end if

	open : iniFile, iniFilePath, put
	if iniFile <= 0 then
	    return
	end if

	loop
	    exit when sectPtr = nil
	    if sectPtr -> name not= "" then
		put : iniFile, "[", sectPtr -> name, "]"
	    end if
	    propPtr := sectPtr -> propHead
	    loop
		exit when propPtr = nil
		if propPtr -> comment then
		    put : iniFile, propPtr -> name
		else
		    if propPtr -> quotes then
			put : iniFile, propPtr -> name, "=\"",
			    propPtr -> value, "\""
		    else
			put : iniFile, propPtr -> name, "=", propPtr -> value
		    end if
		end if
		propPtr := propPtr -> next
	    end loop
	    put : iniFile, ""
	    sectPtr := sectPtr -> next
	end loop
	close : iniFile
    end Write


    procedure Delete (sectionName, propertyName : string)
	var sectPtr : ^Section := head
	var propPtr, lastPropPtr : ^Property := nil

	% Set error to 0
	Error.Set (0)

	loop
	    exit when sectPtr = nil or
		sectPtr -> name = sectionName or
		(sectPtr -> name = "" and sectionName = DEFAULT)
	    sectPtr := sectPtr -> next
	end loop
	if sectPtr = nil then
	    % Section not found error
	    Error.SetMsg (eIniFileSectionNotFound,
		"No '" + sectionName + "' section in '" + iniFilePath + "'")
	    return
	end if
	propPtr := sectPtr -> propHead
	loop
	    exit when propPtr = nil or
		propPtr -> name = propertyName and
		not propPtr -> comment
	    lastPropPtr := propPtr
	    propPtr := propPtr -> next
	end loop
	if propPtr = nil then

	    Error.SetMsg (eIniFilePropertyNotFound,
		"No '" + propertyName + "' property in '" +
		sectionName + "' section of '" + iniFilePath + "'")
	    return
	end if
	if lastPropPtr = nil then
	    sectPtr -> propHead := propPtr -> next
	else
	    lastPropPtr -> next := propPtr -> next
	end if
	free propPtr

	if not bufferWrite then
	    Write
	end if
    end Delete


    procedure SetString (sectionName, propertyName, propertyValue : string)
	var sectPtr : ^Section := head
	var lastSectPtr : ^Section := nil
	var propPtr, lastPropPtr : ^Property := nil

	% Set error to 0
	Error.Set (0)

	loop
	    exit when sectPtr = nil or sectPtr -> name = sectionName or
		(sectPtr -> name = "" and sectionName = DEFAULT)
	    lastSectPtr := sectPtr
	    sectPtr := sectPtr -> next
	end loop
	if sectPtr = nil then
	    new sectPtr
	    if lastSectPtr = nil then
		head := sectPtr
	    else
		lastSectPtr -> next := sectPtr
	    end if
	    if sectionName = DEFAULT then
		sectPtr -> name := ""
	    else
		sectPtr -> name := sectionName
	    end if
	    sectPtr -> propHead := nil
	    sectPtr -> next := nil
	end if

	propPtr := sectPtr -> propHead
	loop
	    exit when propPtr = nil or
		propPtr -> name = propertyName and
		not propPtr -> comment
	    lastPropPtr := propPtr
	    propPtr := propPtr -> next
	end loop
	if propPtr = nil then
	    new propPtr
	    if lastPropPtr = nil then
		sectPtr -> propHead := propPtr
	    else
		lastPropPtr -> next := propPtr
	    end if
	    propPtr -> name := propertyName
	end if
	propPtr -> value := propertyValue
	propPtr -> comment := false
	if Str.Trim (propertyValue) = propertyValue then
	    propPtr -> quotes := false
	else
	    propPtr -> quotes := true
	end if
	propPtr -> next := nil

	if not bufferWrite then
	    Write
	end if
    end SetString


    procedure SetBoolean (sectionName, propertyName : string,
	    propertyValue : boolean)
	if propertyValue = true then
	    SetString (sectionName, propertyName, "true")
	else
	    SetString (sectionName, propertyName, "false")
	end if
    end SetBoolean


    procedure SetInt (sectionName, propertyName : string, propertyValue : int)
	SetString (sectionName, propertyName, intstr (propertyValue))
    end SetInt


    procedure SetReal (sectionName, propertyName : string,
	    propertyValue : real)
	SetString (sectionName, propertyName, realstr (propertyValue, 0))
    end SetReal


    procedure SetStringArray (sectionName, propertyName : string,
	    numValues : int, propertyValues : array 1 .. * of string)
	for i : 1 .. numValues
	    SetString (sectionName, propertyName + "." + intstr (i),
		propertyValues (i))
	end for
    end SetStringArray


    procedure SetBooleanArray (sectionName, propertyName : string,
	    numValues : int, propertyValues : array 1 .. * of boolean)
	for i : 1 .. numValues
	    if propertyValues (i) then
		SetString (sectionName, propertyName + "." + intstr (i),
		    "true")
	    else
		SetString (sectionName, propertyName + "." + intstr (i),
		    "false")
	    end if
	end for
    end SetBooleanArray


    procedure SetIntArray (sectionName, propertyName : string,
	    numValues : int, propertyValues : array 1 .. * of int)
	for i : 1 .. numValues
	    SetString (sectionName, propertyName + "." + intstr (i),
		intstr (propertyValues (i)))
	end for
    end SetIntArray


    procedure SetRealArray (sectionName, propertyName : string,
	    numValues : int, propertyValues : array 1 .. * of real)
	for i : 1 .. numValues
	    SetString (sectionName, propertyName + "." + intstr (i),
		realstr (propertyValues (i), 0))
	end for
    end SetRealArray


    procedure SetBufferWrite (iniBufferWrite : boolean)
	bufferWrite := iniBufferWrite
    end SetBufferWrite


    procedure StartSection
	% Set error to 0
	Error.Set (0)
	listSection := head
    end StartSection


    function GetSection : string
	var name : string

	% Set error to 0
	Error.Set (0)

	if listSection not= nil then
	    name := listSection -> name
	    if name = "" then
		name := DEFAULT
	    end if
	    listSection := listSection -> next
	else
	    % Error at end of list
	    Error.Set (eIniFileAtEndOfList)
	    name := ""
	end if
	result name
    end GetSection


    procedure StartProperty (sectionName : string)
	var sectPtr : ^Section := head

	% Set error to 0
	Error.Set (0)

	loop
	    exit when sectPtr = nil or sectPtr -> name = sectionName or
		(sectPtr -> name = "" and sectionName = DEFAULT)
	    sectPtr := sectPtr -> next
	end loop
	if sectPtr not= nil then
	    listProperty := sectPtr -> propHead
	else
	    % Error at end of list
	    Error.SetMsg (eIniFileSectionNotFound,
		"No '" + sectionName + "' section in '" + iniFilePath + "'")
	end if
    end StartProperty


    function GetProperty : string
	var name : string

	% Set error to 0
	Error.Set (0)

	% Skip any comments
	loop
	    exit when listProperty = nil or
		(listProperty not= nil and not listProperty -> comment)
	    listProperty := listProperty -> next
	end loop
	%
	if listProperty not= nil then
	    name := listProperty -> name
	    listProperty := listProperty -> next
	else
	    % Error at end of list
	    Error.Set (eIniFileAtEndOfList)
	    name := ""
	end if
	result name
    end GetProperty


    function GetType (sectionName, propertyName : string) : string
	var s : string := GetString (sectionName, propertyName)

	if Error.Last not= 0 then
	    result ""
	end if

	if Str.Lower (s) = "true" or Str.Lower (s) = "false" then
	    result "boolean"
	elsif strintok (s) then
	    result "int"
	elsif strrealok (s) then
	    result "real"
	else
	    result "string"
	end if
    end GetType
end IniFile
