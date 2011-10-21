/*
 * Errno module - Exports unqualified pervasive the errno constants.
 *                Errno management routines are define in the
 *                predef module Error.
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Errno
    export ~.*all

    include "error.inc"
end Errno
