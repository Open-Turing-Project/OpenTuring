/*
 * Sys module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Sys
    export Exit, GetComputerName, GetUserName, GetEnv, GetPid, Exec,
	Nargs, FetchArg

    external "sys_exit" procedure Exit

    external "sys_getcomputername" function GetComputerName : string

    external "sys_getusername" function GetUserName : string

    external "sys_getenv" function GetEnv (symbol : string) : string

    external "sys_getpid" function GetPid : int

    external "sys_exec" function Exec (str : string) : boolean

    external "sys_nargs" function Nargs : int

    external "sys_fetcharg" function FetchArg (i : int) : string
end Sys
