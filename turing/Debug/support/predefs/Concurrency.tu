/*
 * Concurrency module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module Concurrency
    export ~.*simutime

    external "concur_simutime" function simutime : nat
end Concurrency
