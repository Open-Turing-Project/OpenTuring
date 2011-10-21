/*
 * Dir module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Dir
    export Change, Close, Create, Current, Delete, Exists, Get, GetLong, Open,
	~.*attrDir, ~.*attrRead, ~.*attrWrite, ~.*attrExecute,
	~.*attrHidden, ~.*attrSystem, ~.*attrVolume, ~.*attrArchive

    external "dir_change" procedure Change (dirpath : string)

    external "dir_close" procedure Close (dirstream : int)

    external "dir_create" procedure Create (dirpath : string)

    external "dir_current" function Current : string

    external "dir_delete" procedure Delete (dirpath : string)

    external "dir_get" function Get (dirstream : int) : string

    external "dir_getlong"
	procedure GetLong (dirstream : int, var filename : string,
	var size, attributes, filetime : int)

    external "dir_open" function Open (pathname : string) : int

    external "dir_exists" function Exists (pathname : string) : boolean

    %
    % Constants defining the attribute bits
    %

    const attrDir : int := 16#1
    const attrRead : int := 16#2
    const attrWrite : int := 16#4
    const attrExecute : int := 16#8
    const attrHidden : int := 16#10
    const attrSystem : int := 16#20
    const attrVolume : int := 16#40
    const attrArchive : int := 16#80
end Dir
