/* Error numbers */

/* Error number (errno) Bases */

const *generalErrorBase             := 100
const *fsysErrorBase                := 200
const *streamErrorBase              := 300

const *fileErrorBase                := 400
const *dirErrorBase                 := 500

const *picErrorBase                 := 600
const *penErrorBase                 := 700
const *brushErrorBase               := 800
const *spriteErrorBase              := 900
const *fontErrorBase                := 1000
const *drawErrorBase                := 1100
const *timeErrorBase                := 1200
const *mouseErrorBase               := 1300
const *rgbErrorBase                 := 1400
const *musicErrorBase               := 1500
const *configErrorBase              := 1600
const *viewErrorBase                := 1700
const *windowErrorBase              := 1800
const *printerErrorBase             := 1900
const *textErrorBase                := 2000
const *guiErrorBase                 := 2100
const *lexErrorBase                 := 2200
const *netErrorBase                 := 2300
const *joystickErrorBase            := 2400
const *pcErrorBase                  := 2500
const *sysErrorBase                 := 2600
const *iniFileErrorBase             := 2700
const *strErrorBase		    := 2800


% No error
const *eNoError                     := 0


% General module

const *eUnknownError                := generalErrorBase
const *eInsufficientMemory          := generalErrorBase+1
const *eNotSupported                := generalErrorBase+2
const *eNoOperation                 := generalErrorBase+3
const *eNoOutputToScreen            := generalErrorBase+4
const *eNotAllowedInTextMode        := generalErrorBase+5
const *eNotAllowedInGraphicsMode    := generalErrorBase+6
const *eMustBeInGraphicsMode        := generalErrorBase+7
const *eUnimplementedFeature        := generalErrorBase+8
const *eEnvironWinCreateFailed      := generalErrorBase+9


% FileSys module

const *eFsysUnknownError            := fsysErrorBase
const *eFsysFileNotFound            := fsysErrorBase+1
const *eFsysPathNotFound            := fsysErrorBase+2
const *eFsysTooManyOpenFiles        := fsysErrorBase+3
const *eFsysAccessDenied            := fsysErrorBase+4
const *eFsysHandleInvalid           := fsysErrorBase+5
const *eFsysInsufficientMemory      := fsysErrorBase+6
const *eFsysEnvironmentInvalid      := fsysErrorBase+7
const *eFsysFormatInvalid           := fsysErrorBase+8
const *eFsysAccessCodeInvalid       := fsysErrorBase+9
const *eFsysDataInvalid             := fsysErrorBase+10
const *eFsysReadOnly                := fsysErrorBase+11
const *eFsysDiskDriveInvalid        := fsysErrorBase+12
const *eFsysNotSameDevice           := fsysErrorBase+13
const *eFsysNoMoreFiles             := fsysErrorBase+14
const *eFsysWriteProtectedDisk      := fsysErrorBase+15
const *eFsysDriveNotReady           := fsysErrorBase+16
const *eFsysNoDiskInDrive           := fsysErrorBase+17
const *eFsysUnknownCommand          := fsysErrorBase+18
const *eFsysDataErrorCrc            := fsysErrorBase+19
const *eFsysSeekError               := fsysErrorBase+20
const *eFsysUnknownMediaType        := fsysErrorBase+21
const *eFsysSectorNotFound          := fsysErrorBase+22
const *eFsysPrinterOutOfPaper       := fsysErrorBase+23
const *eFsysWriteError              := fsysErrorBase+24
const *eFsysReadError               := fsysErrorBase+25
const *eFsysGeneralFault            := fsysErrorBase+26
const *eFsysNoSpaceLeft             := fsysErrorBase+27
const *eFsysFileExists              := fsysErrorBase+28
const *eFsysDirExists               := fsysErrorBase+29
const *eFsysNotAFile                := fsysErrorBase+30
const *eFsysCantReadFromWdwfile     := fsysErrorBase+31
const *eFsysCantWriteToWdwfile      := fsysErrorBase+32

const *eFsysPathnameModified        := fsysErrorBase+33
const *eFsysCantGoUpFromRootDir     := fsysErrorBase+34
const *eFsysBadCharInPathname       := fsysErrorBase+35
const *eFsysMalformedPathname       := fsysErrorBase+36

const *eFsysCantReadWriteToDialogWin := fsysErrorBase+37

const *eFsysFunctionNumberInvalid   := fsysErrorBase+80
const *eFsysMcbDestroyed            := fsysErrorBase+81
const *eFsysMcbInvalid              := fsysErrorBase+82
const *eFsysBadReqLenStruct         := fsysErrorBase+83
const *eFsysUnknownUnit             := fsysErrorBase+84




/* Error numbers grouped by associated OOT predef module */

% Stream module

const *eStreamUnknownError          := streamErrorBase
const *eStreamNotAnId               := streamErrorBase+1
const *eStreamNeverOpened           := streamErrorBase+2
const *eStreamClosed                := streamErrorBase+3
const *eStreamTooManyOpen           := streamErrorBase+4
const *eStreamIllegalOperation      := streamErrorBase+5


% File module

const *eFileUnknownError            := fileErrorBase
const *eFileIsADirectory            := fileErrorBase+1
const *eFileWin95CannotHandleNetworkShare   := fileErrorBase+2
const *eFileCannotGetParentOfRoot   := fileErrorBase+3

% Dir module

const *eDirUnknownError             := dirErrorBase
const *eDirNotAnId                  := dirErrorBase+1
const *eDirNeverOpened              := dirErrorBase+2
const *eDirClosed                   := dirErrorBase+3
const *eDirTooManyStreams           := dirErrorBase+4
const *eDirReadPastEof              := dirErrorBase+5
const *eDirNotADirectory            := dirErrorBase+6
const *eDirAttemptedToRemoveCwd     := dirErrorBase+7


% Pic module

const *ePicUnknownError             := picErrorBase
const *ePicNotAnId                  := picErrorBase+1
const *ePicNeverNewed               := picErrorBase+2
const *ePicFreed                    := picErrorBase+3
const *ePicTooMany                  := picErrorBase+4
const *ePicFailure                  := picErrorBase+5
const *ePicBadBox                   := picErrorBase+6
const *ePicNotPicFile               := picErrorBase+7
const *ePicUsedBySprite             := picErrorBase+8
const *ePicNotATakepicArray         := picErrorBase+9
const *ePicNotInSameGraphicsMode    := picErrorBase+10
const *ePicModeOutOfBounds          := picErrorBase+11
const *ePicModeUnsupported          := picErrorBase+12
const *ePicCoordsOffScreen          := picErrorBase+13
const *ePicCantAllocMemForPic       := picErrorBase+14
const *ePicNoMemToConvDrawpic       := picErrorBase+15
const *ePicInsufficientMemory       := picErrorBase+16
const *ePicIOError                  := picErrorBase+17
const *ePicFileNotFound             := picErrorBase+18
const *ePicWriteOpenFailed          := picErrorBase+19
const *ePicUnknownFileFormat        := picErrorBase+20
const *ePicUnsupportedFileFormat    := picErrorBase+21
const *ePicScaleTooSmall            := picErrorBase+22
const *ePicScaleTooLarge            := picErrorBase+23
const *ePicAxisOutOfRange           := picErrorBase+24
const *ePicCantRotateThisDepth      := picErrorBase+25
const *ePicInUseBySprites           := picErrorBase+26
const *ePicBadArraySize             := picErrorBase+27
const *ePicUnknownTransition        := picErrorBase+28


% Pen module

const *ePenUnknownError             := penErrorBase
const *ePenNotAnId                  := penErrorBase+1
const *ePenNeverNewed               := penErrorBase+2
const *ePenFreed                    := penErrorBase+3


% Brush module

const *eBrushUnknownError           := brushErrorBase
const *eBrushNotAnId                := brushErrorBase+1
const *eBrushNeverNewed             := brushErrorBase+2
const *eBrushFreed                  := brushErrorBase+3


% Sprite module

const *eSpriteUnknownError          := spriteErrorBase
const *eSpriteNotAnId               := spriteErrorBase+1
const *eSpriteNeverNewed            := spriteErrorBase+2
const *eSpriteFreed                 := spriteErrorBase+3
const *eSpriteTooMany               := spriteErrorBase+4
const *eSpriteNoChange              := spriteErrorBase+5


% Font module

const *eFontUnknownError            := fontErrorBase
const *eFontNotAnId                 := fontErrorBase+1
const *eFontNeverNewed              := fontErrorBase+2
const *eFontFreed                   := fontErrorBase+3
const *eFontCantFreeDefault         := fontErrorBase+4
const *eFontTooMany                 := fontErrorBase+5


% Draw module

const *eDrawUnknownError            := drawErrorBase
const *eDrawClipped                 := drawErrorBase+1
const *eDrawFailure                 := drawErrorBase+2
const *eDrawTooComplicated          := drawErrorBase+3
const *eDrawClrNumTooSmall          := drawErrorBase+4
const *eDrawClrNumTooLarge          := drawErrorBase+5
const *eDrawCantAllocPolyPts        := drawErrorBase+6


% Time module

const *eTimeUnknownError            := timeErrorBase
const *eTimeInvalidTimeStr          := timeErrorBase+1
const *eTimeInvalidDateStr          := timeErrorBase+2
const *eTimeSecondsOutOfRange       := timeErrorBase+3
const *eTimeBadDay                  := timeErrorBase+4
const *eTimeBadMonth                := timeErrorBase+5
const *eTimeBadYear                 := timeErrorBase+6
const *eTimeBadHour                 := timeErrorBase+7
const *eTimeBadMinute               := timeErrorBase+8
const *eTimeBadSecond               := timeErrorBase+9


% Mouse module

const *eMouseUnknownError           := mouseErrorBase
const *eMouseNoneInstalled          := mouseErrorBase+1
const *eMouseBadButtonchooseStr     := mouseErrorBase+2
const *eMouseBadButtonmotionStr     := mouseErrorBase+3


% rgb module

const *eRgbUnknownError             := rgbErrorBase
const *eRgbBadNum                   := rgbErrorBase+1
const *eRgbFailure                  := rgbErrorBase+2
const *eRgbCouldNotSetClr           := rgbErrorBase+3
const *eRgbRedOutOfRange            := rgbErrorBase+4
const *eRgbGreenOutOfRange          := rgbErrorBase+5
const *eRgbBlueOutOfRange           := rgbErrorBase+6


% Music module

const *eMusicUnknownError           := musicErrorBase
const *eMusicNoNotes                := musicErrorBase+1
const *eMusicBadPlayChar            := musicErrorBase+2
const *eMusicBadNoteLength          := musicErrorBase+3
const *eMusicFrequencyOutOfRange    := musicErrorBase+4
const *eMusicDurationOutOfRange     := musicErrorBase+5
const *eMusicMidiOpenError          := musicErrorBase+6
const *eMusicMidiPlayError          := musicErrorBase+7
const *eMusicCDOpenError            := musicErrorBase+8
const *eMusicCDPlayError            := musicErrorBase+9
const *eMusicWaveOpenError          := musicErrorBase+10
const *eMusicWavePlayError          := musicErrorBase+11
const *eMusicCloserror              := musicErrorBase+12
const *eMusicGetStatusError         := musicErrorBase+13
const *eMusicNoteOpenError          := musicErrorBase+14
const *eMusicNotePlayError          := musicErrorBase+15
const *eMusicFreqOpenError          := musicErrorBase+16
const *eMusicFreqPlayError          := musicErrorBase+17
const *eMusicMP3OpenError           := musicErrorBase+18
const *eMusicMP3PlayError           := musicErrorBase+19
const *eMusicDisabled               := musicErrorBase+20
const *eMusicUnknownFileTypeError   := musicErrorBase+21
const *eMusicIOError                := musicErrorBase+22
const *eMusicBadMusicFile           := musicErrorBase+23


% Config module

const *eConfigUnknownError          := configErrorBase
const *eConfigUnknownDisplayCode    := configErrorBase+1
const *eConfigUnknownLangCode       := configErrorBase+2
const *eConfigUnknownMachineCode    := configErrorBase+3
const *eConfigCantGetInformation    := configErrorBase+4


% View module

const *eViewUnknownError            := viewErrorBase
const *eViewOutsideClippingRegion   := viewErrorBase+1
const *eViewXTooSmall               := viewErrorBase+2
const *eViewXTooLarge               := viewErrorBase+3
const *eViewYTooSmall               := viewErrorBase+4
const *eViewYTooLarge               := viewErrorBase+5
const *eViewSetBadCursorSize        := viewErrorBase+6
const *eViewSetUnknownGraphMode     := viewErrorBase+7
const *eViewSetBadGraphicsMode      := viewErrorBase+8
const *eViewSetUnknownGraphDepth    := viewErrorBase+9
const *eViewSetUnsuppGraphMode      := viewErrorBase+10
const *eViewSetGraphicsModeFailed   := viewErrorBase+11
const *eViewSetBadPosition          := viewErrorBase+12
const *eViewSetUnknownSetCommand    := viewErrorBase+13


% Window module

const *eWindowUnknownError          := windowErrorBase
const *eWindowNotAnId               := windowErrorBase+1
const *eWindowNeverOpened           := windowErrorBase+2
const *eWindowClosed                := windowErrorBase+3
const *eWindowSetBadOption          := windowErrorBase+4
const *eWindowGetFromInvisible      := windowErrorBase+5
const *eWindowObsoleteOpen          := windowErrorBase+6
const *eWindowObsoleteClose         := windowErrorBase+7
const *eWindowCantSeek              := windowErrorBase+8
const *eWindowCantTell              := windowErrorBase+9
const *eWindowCantRead              := windowErrorBase+10
const *eWindowCantWrite             := windowErrorBase+11


% Printer module

const *ePrinterUnknownError         := printerErrorBase
const *ePrinterNotAnId              := printerErrorBase+1
const *ePrinterNeverOpened          := printerErrorBase+2
const *ePrinterClosed               := printerErrorBase+3
const *ePrinterObsoleteOpen         := printerErrorBase+4
const *ePrinterObsoleteClose        := printerErrorBase+5
const *ePrinterCantSeek             := printerErrorBase+6
const *ePrinterCantTell             := printerErrorBase+7
const *ePrinterCantRead             := printerErrorBase+8
const *ePrinterCantWrite            := printerErrorBase+9


% Text module

const *eTextUnknownError            := textErrorBase
const *eTextRowTooSmall             := textErrorBase+1
const *eTextRowTooLarge             := textErrorBase+2
const *eTextColTooSmall             := textErrorBase+3
const *eTextColTooLarge             := textErrorBase+4


% gui module

const *eGuiMissingButtonText        := guiErrorBase+1


% Lexer module

const *eLexUnknownError             := lexErrorBase
const *eLexNotAnId                  := lexErrorBase+1
const *eLexNeverInitialized         := lexErrorBase+2
const *eLexTooManyStreams           := lexErrorBase+3
const *eLexStreamAlreadyOpen        := lexErrorBase+4
const *eLexTableError               := lexErrorBase+5


% Net module

const *eNetUnknownError		    := netErrorBase
const *eNetNotAnId		    := netErrorBase+1
const *eNetNeverOpened		    := netErrorBase+2
const *eNetMissingAddress	    := netErrorBase+3
const *eNetInvalidPort		    := netErrorBase+4
const *eNetBadAddress		    := netErrorBase+5
const *eNetNameResolutionFailed	    := netErrorBase+6
const *eNetReadPastEof		    := netErrorBase+7
const *eNetClosed		    := netErrorBase+8
const *eNetInitFailed		    := netErrorBase+9
const *eNetConnectionAborted	    := netErrorBase+10
const *eNetConnectionAbortedRemote  := netErrorBase+11
const *eNetBadCookieURL		    := netErrorBase+12
const *eNetNoCookieForThisURL	    := netErrorBase+13
const *eNetNoCookieForThisName	    := netErrorBase+14
const *eNetArrayTooSmall	    := netErrorBase+15
const *eNetDontUseEOF		    := netErrorBase+16


% Joystick module


% PC module


% Sys module
const *eSysUnknownError             := sysErrorBase
const *eSysEnvVarNotFound           := sysErrorBase+1
const *eSysEnvVarTooLong            := sysErrorBase+2
const *eSysNotEnoughSystemMemory    := sysErrorBase+3
const *eSysBadExeFile               := sysErrorBase+4
const *eSysAccessDenied             := sysErrorBase+5
const *eSysBadFileAssoc             := sysErrorBase+6
const *eSysNoFileAssoc              := sysErrorBase+7
const *eSysSharingViolation         := sysErrorBase+8
const *eSysSysExecNotAllowed        := sysErrorBase+9


% IniFile module
const *eIniFileUnknownError         := iniFileErrorBase
const *eIniFileBadSection           := iniFileErrorBase+1
const *eIniFileMissingEquals        := iniFileErrorBase+2
const *eIniFileSectionNotFound      := iniFileErrorBase+3
const *eIniFilePropertyNotFound     := iniFileErrorBase+4
const *eIniFilePropertyNotBoolean   := iniFileErrorBase+5
const *eIniFilePropertyNotInt       := iniFileErrorBase+6
const *eIniFilePropertyNotReal      := iniFileErrorBase+7
const *eIniFileNoIniFileOpen        := iniFileErrorBase+8
const *eIniFileAtEndOfList          := iniFileErrorBase+9
const *eIniFileArrayTooSmall        := iniFileErrorBase+10


% Str module
const *eStrUnknownError		    := strErrorBase
const *eStrArrayPosOutOfBounds	    := strErrorBase+1
const *eStrNumCharsTooLarge	    := strErrorBase+2
const *eStrPartialLine		    := strErrorBase+3
const *eStrLineTooLong		    := strErrorBase+4
const *eStrCharArrayTooSmall	    := strErrorBase+5
const *eStrSrcArrayPosOutOfBounds   := strErrorBase+6
const *eStrDestArrayPosOutOfBounds  := strErrorBase+7
const *eStrCharArrayTooSmallForCharCopy	:= strErrorBase+8


/* End of error numbers */
