/*
 * Config module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED
 */

unit
module pervasive Config
    export Display, Lang, Machine,
	~.*cdScreenHeight, ~.*cdScreenWidth, ~.*cdMaxNumColors,
	~.*cdMaxNumColours, ~.*cdMaxPages,
	~.*clLanguageVersion, ~.*clMaxNumDirStreams, ~.*clMaxNumStreams,
	~.*clRelease, ~.*clMaxRunTimeArgs,
	~.*cmFPU, ~.*cmOS, ~.*cmProcessor

    external "config_display" function Display (dispInfoCode : int) : int

    external "config_lang" function Lang (langInfoCode : int) : int

    external "config_machine" function Machine (machInfoCode : int) : int

    %
    % Constant codes used by Config module
    %

    % For Config.Display

    const cdScreenHeight := 1000
    const cdScreenWidth := 1010
    const cdMaxNumColours := 1020
    const cdMaxNumColors := cdMaxNumColours
    const cdMaxPages := 1030

    % For Config.Lang

    const clRelease := 2000
    const clLanguageVersion := 2001
    const clMaxNumStreams := 2002
    const clMaxNumDirStreams := 2003
    const clMaxRunTimeArgs := 2004

    % For Config.Machine

    const cmProcessor := 3000
    const cmFPU := 3001
    const cmOS := 3002
end Config
