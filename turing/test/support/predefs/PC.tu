/*
 * PC module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive PC
    export InPort, InPortWord, OutPort, OutPortWord, Interrupt,
	InterruptSegs, ParallelGet, ParallelPut,
	SerialGet, SerialHasch, SerialPut

    % This procedure causes a run-time error indicating that the feature
    % is obsolete.  "obsolete_replace" indicates a suitable replacement
    % routine.
    external procedure obsolete (routine : string)
    external procedure obsolete_replace (routine, replacementRoutine : 
	string)

    %
    % Only the parallel port and port routines are still valid.  
    % All others are obsolete
    %
    external "pc_inport" function InPort (address : int) : nat1
    external "pc_outport" procedure OutPort (address : int, value : int)
    external "pc_parallelget" function ParallelGet (port : int) : nat1
    external "pc_parallelput"
	procedure ParallelPut (port : int, value : int)

    %
    % Obsolete routines
    %

    function InPortWord (address : int) : int
	obsolete ("PC.InPortWord")
	result 0
    end InPortWord

    procedure OutPortWord (address : int, value : int)
	obsolete ("PC.OutPortWord")
    end OutPortWord

    procedure Interrupt (interrupt : int,
	    var eax, ebx, ecx, edx, esi, edi, cflag : int)
	obsolete ("PC.Interrupt")
    end Interrupt

    procedure InterruptSegs (interrupt : int,
	    var eax, ebx, ecx, edx, esi, edi : int,
	    var cflag, ds, es, ss, cs : int)
	obsolete ("PC.InterruptSegs")
    end InterruptSegs

    function SerialGet (port : int) : nat1
	obsolete ("PC.SerialGet")
	result 0
    end SerialGet

    function SerialHasch (port : int) : boolean
	obsolete ("PC.SerialHasch")
	result false
    end SerialHasch

    procedure SerialPut (port : int, value : int)
	obsolete ("PC.SerialPut")
    end SerialPut
end PC
