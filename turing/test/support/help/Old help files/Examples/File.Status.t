% The "File.Status" program.
const pathName : string := "c:/winnt/notepad.exe"
var size, attribute, fileTime : int

File.Status (pathName, size, attribute, fileTime)

if Error.Last = eNoError then
    put "      Name: ", File.FullPath (pathName)
    put "   Created: ", Time.SecDate (fileTime)
    put "      Size: ", size, " bytes"
    put "Attributes: " ..
    if (attribute and ootAttrDir) not= 0 then
	put "Directory " ..
    else
	put "" ..
    end if
    if (attribute and ootAttrRead) not= 0 then
	put "Readable " ..
    else
	put "" ..
    end if
    if (attribute and ootAttrWrite) not= 0 then
	put "Writable " ..
    else
	put "" ..
    end if
    if (attribute and ootAttrExecute) not= 0 then
	put "Executable", skip
    else
	put "", skip
    end if
else
    put "Unable to get file information"
    put "Error: ", Error.LastMsg, skip
end if
