/*
 * File module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module File
    export Copy, CreatePath, Delete, DiskFree, Exists, FullPath,
	Parent, Rename, SplitPath, Status,
	~.*ootAttrDir, ~.*ootAttrRead, ~.*ootAttrWrite, ~.*ootAttrExecute,
	~.*ootAttrHidden, ~.*ootAttrSystem, ~.*ootAttrVolume,
	~.*ootAttrArchive,
	~.*ootAttrOwnerRead, ~.*ootAttrOwnerWrite, ~.*ootAttrOwnerExecute,
	~.*ootAttrGroupRead, ~.*ootAttrGroupWrite, ~.*ootAttrGroupExecute,
	~.*ootAttrOtherRead, ~.*ootAttrOtherWrite, ~.*ootAttrOtherExecute

    external "file_status" procedure Status (pathname : string,
	var filesize, attributes, filetime : int)

    external "file_copy" procedure Copy (srcpathname, destpathname : string)

    external "file_rename" procedure Rename (pathname, newfilename : string)

    external "file_delete" procedure Delete (pathname : string)

    external "file_diskfree" function DiskFree (pathname : string) : int

    external "file_exists" function Exists (path : string) : boolean

    external "file_fullpath" function FullPath (pathname : string) : string

    external "file_parent" function Parent (pathname : string) : string

    % Experimentals

    external "file_splitpath"
	procedure SplitPath (pathname : string, var path, name : string)

    external "file_createpath"
	function CreatePath (path, name : string) : string

    % Constants for File.Status

    const *ootAttrDir : int := 16#1
    const *ootAttrRead : int := 16#2
    const *ootAttrWrite : int := 16#4
    const *ootAttrExecute : int := 16#8
    const *ootAttrHidden : int := 16#10
    const *ootAttrSystem : int := 16#20
    const *ootAttrVolume : int := 16#40
    const *ootAttrArchive : int := 16#80
    const *ootAttrOwnerRead : int := 16#100
    const *ootAttrOwnerWrite : int := 16#200
    const *ootAttrOwnerExecute : int := 16#400
    const *ootAttrGroupRead : int := 16#800
    const *ootAttrGroupWrite : int := 16#1000
    const *ootAttrGroupExecute : int := 16#2000
    const *ootAttrOtherRead : int := 16#4000
    const *ootAttrOtherWrite : int := 16#8000
    const *ootAttrOtherExecute : int := 16#10000
end File
