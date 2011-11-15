type point3D:
    record
	x,y,z : real
    end record
type objLine :
    record
	p1 : int
	p2 : int
    end record
type rawLine :
    record
	p1 : point3D
	p2 : point3D
    end record
    
function initP3D(x,y,z : real) : point3D
    var p : point3D
    p.x := x
    p.y := y
    p.z := z
    result p
end initP3D    

class Parser3D
    export construct, parseFile, DrawLines
    deferred proc construct (filename : string)
    deferred proc DrawLines ()
    deferred proc parseFile ()
end Parser3D
class ObjParser
    inherit Parser3D
    import point3D, objLine, initP3D
    const BASE_SIZE := 10
    var verts : flexible array 1 .. 0 of point3D
    var curVert : int := 1

    var lines : flexible array 1 .. 0 of objLine
    var curLine : int := 1

    var input : flexible array 1 .. 0 of string

    %files
    function getLines (fileName : string) : int
	var f, numLines : int
	var line : string
	open : f, fileName, get
	numLines := 0
	loop
	    exit when eof (f)
	    get : f, line : *
	    numLines += 1
	end loop
	close : f
	result numLines
    end getLines
    fcn getLine (i : int) : string
	result input (i) + '\n'
    end getLine

    procedure readFile (fileName : string)
	var f : int
	var line : string
	open : f, fileName, get
	for i : 1 .. upper (input)
	    get : f, input (i) : *
	end for
	close : f
    end readFile

    body proc construct (filename : string)
	new verts, BASE_SIZE
	new lines, BASE_SIZE

	new input, getLines (filename)
	readFile (filename)
	
	var success := GL.NewWin(640,480)
    end construct
    % arrays
    proc addVert (x, y, z : real)
	if (curVert > upper (verts)) then       % Expand the array where necessary
	    new verts, upper (verts) + 10
	end if
	verts (curVert) := initP3D (x, y, z)
	curVert += 1
    end addVert
    proc addLine (p1, p2 : int)
	if (curLine > upper (lines)) then       % Expand the array where necessary
	    new lines, upper (lines) + 10
	end if
	lines (curLine).p1 := p1
	lines (curLine).p2 := p2
	curLine += 1
    end addLine
    %parsing
    proc parseVert (line : string)
	var parts : array 1 .. 6 of string
	var curpart : int := 1
	var part : string := ""
	for j : 1 .. length (line)
	    if line (j) = ' ' then
		if part not= "" and part not= " " then
		    %put "split part:" + part + ";"
		    parts (curpart) := part
		    part := ""
		    curpart += 1
		end if
	    else
		%put "found char: " + line (j)
		part := part + line (j)
	    end if
	end for
	if part not= "" and part not= " " then
	    %put "split part:" + part + ";"
	    parts (curpart) := part
	    part := ""
	    curpart += 1
	end if
	addVert (strreal (parts (2)), strreal (parts (3)), strreal (parts (4)))
    end parseVert
    proc parseFace (line : string)
	var parts : flexible array 1 .. 0 of string
	var curpart : int := 1
	var part : string := ""
	for j : 1 .. length (line)
	    if line (j) = ' ' then
		if part not= "" and part not= " " then
		    %put "split part:" + part + ";"
		    if curpart > upper (parts) then
			new parts, upper (parts) + 2
		    end if
		    parts (curpart) := part
		    part := ""
		    curpart += 1
		end if
	    else
		%put "found char: " + line (j)
		part := part + line (j)
	    end if
	end for
	if part not= "" and part not= " " then
	    %put "split part:" + part + ";"
	    if curpart > upper (parts) then
		new parts, upper (parts) + 1
	    end if
	    parts (curpart) := part
	    part := ""
	    curpart += 1
	end if
	for j : 2 .. (upper (parts) - 1)
	    addLine (strint (parts (j)), strint (parts (j + 1)))
	end for
	addLine (strint (parts (upper (parts))), strint (parts (2)))
    end parseFace


    body proc parseFile ()
	for i : 1 .. upper (input)
	    var line : string := input (i)
	    if length (line) > 0 then
		var fchar : char := line (1)

		if fchar = 'v' then
		    parseVert (line)
		elsif fchar = 'f' then
		    %put "parsing face on line " + intstr(i)
		    parseFace (line)
		end if
	    end if
	end for
    end parseFile
    %drawing
    body proc DrawLines ()
	GL.Cls()
	for i : 1 .. (curLine - 1)
	    %drawLine3D (lines (i).p1, lines (i).p2)
	    var p1 : point3D := verts (lines (i).p1)
	    var p2 : point3D := verts (lines (i).p2)
	    GL.DrawLine(p1.x,p1.y,p1.z,p2.x,p2.y,p2.z,0,0,250)
	end for
	GL.Update()
    end DrawLines
end ObjParser
% ========================================================
class RawParser
    inherit Parser3D
    import point3D, initP3D, rawLine
    const BASE_SIZE := 10
    var lines : flexible array 1 .. 0 of rawLine
    var curLine : int := 1

    var input : flexible array 1 .. 0 of string

    %files
    function getLines (fileName : string) : int
	var f, numLines : int
	var line : string
	open : f, fileName, get
	numLines := 0
	loop
	    exit when eof (f)
	    get : f, line : *
	    numLines += 1
	end loop
	close : f
	result numLines
    end getLines
    fcn getLine (i : int) : string
	result input (i) + '\n'
    end getLine

    procedure readFile (fileName : string)
	var f : int
	var line : string
	open : f, fileName, get
	for i : 1 .. upper (input)
	    get : f, input (i) : *
	end for
	close : f
    end readFile

    body proc construct (filename : string)
	new lines, BASE_SIZE

	new input, getLines (filename)
	readFile (filename)

	var success := GL.NewWin(640,480)
    end construct
    % arrays
    proc addLine (p1, p2 : point3D)
	if (curLine > upper (lines)) then       % Expand the array where necessary
	    new lines, upper (lines) + 10
	end if
	lines (curLine).p1 := p1
	lines (curLine).p2 := p2
	curLine += 1
    end addLine
    %parsing
    proc parseFace (line : string)
	var parts : flexible array 1 .. 9 of string
	var curpart : int := 1
	var part : string := ""
	for j : 1 .. length (line)
	    if line (j) = ' ' then
		if part not= "" and part not= " " then
		    %put "split part:" + part + ";"
		    parts (curpart) := part
		    part := ""
		    curpart += 1
		end if
	    else
		%put "found char: " + line (j)
		part := part + line (j)
	    end if
	end for
	if part not= "" and part not= " " then
	    %put "split part:" + part + ";"
	    if curpart > upper (parts) then
		new parts, upper (parts) + 1
	    end if
	    parts (curpart) := part
	    part := ""
	    curpart += 1
	end if
	var p1 := initP3D (strreal (parts (1)), strreal (parts (2)), strreal (parts (3)))
	var p2 := initP3D (strreal (parts (4)), strreal (parts (5)), strreal (parts (6)))
	var p3 := initP3D (strreal (parts (7)), strreal (parts (8)), strreal (parts (9)))

	addLine (p1, p2)
	addLine (p2, p3)
	addLine (p3, p1)
    end parseFace


    body proc parseFile ()
	for i : 1 .. upper (input)
	    var line : string := input (i)
	    if length (line) > 0 then
		var fchar : char := line (1)
		parseFace (line)
	    end if
	end for
    end parseFile
    %drawing
    body proc DrawLines ()
	GL.Cls()
	for i : 1 .. (curLine - 1)
	    %drawLine3D (lines (i).p1, lines (i).p2)
	    var p1 := lines (i).p1
	    var p2 := lines (i).p2
	    GL.DrawLine(p1.x,p1.y,p1.z,p2.x,p2.y,p2.z,0,0,250)
	end for
	GL.Update()
    end DrawLines
end RawParser
