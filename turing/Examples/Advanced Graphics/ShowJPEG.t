% The "ShowJPEG" program.
var pic : int := Pic.FileNew ("lighthouse.jpg")
if pic = 0 then
    put "Unable to load JPEG: ", Error.LastMsg
    return
end if
Pic.Draw (pic, 0, 0, picCopy)
var slantedPic : int := Pic.Rotate (pic, 45, -1, -1)
Pic.Draw (slantedPic, 200, 0, picCopy)
