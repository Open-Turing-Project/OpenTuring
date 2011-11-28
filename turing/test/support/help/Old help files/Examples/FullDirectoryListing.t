% The "FullDirectoryListing" program.
setscreen ("text")
var fileName : string
var size, attr, fileTime : int
var dir : int

dir := Dir.Open (".")

loop
    Dir.GetLong (dir, fileName, size, attr, fileTime)
    exit when fileName = ""
    put fileName : 22, " " ..
    if (attr and attrDir) not= 0 then
	put "   [DIR] " ..
    else
	put size : 8, " " ..
    end if
    put Time.SecDate (fileTime), " " ..
    if (attr and attrRead) not= 0 then
	put "r" ..
    else
	put "-" ..
    end if
    if (attr and attrWrite) not= 0 then
	put "w" ..
    else
	put "-" ..
    end if
    if (attr and attrExecute) not= 0 then
	put "x " ..
    else
	put "- " ..
    end if
    if (attr and attrHidden) not= 0 then
	put "(Hidden) " ..
    end if
    if (attr and attrSystem) not= 0 then
	put "(System) " ..
    end if
    if (attr and attrVolume) not= 0 then
	put "(Volume) " ..
    end if
    if (attr and attrArchive) not= 0 then
	put "(Archived) " ..
    end if
    put " "
end loop
put ""
Dir.Close (dir)
