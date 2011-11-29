/*
 * Exceptions module
 *
 * NB:  DO NOT INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY INCLUDED
 */

unit
module Exceptions
    export ~.*all

    /*
     * Turing Plus Predefined exception numbers
     * User defined exception numbers should be greater than 200
     */

    /*
     * default exception number
     */
    const *excpDefaultException := 1    /* default for quit or quit < */

    /*
     * Turing Plus abort exceptions
     */
    /* uninitialized variable */
    const *excpUninitializedVariable := 45             /* not implemented yet */

    /* array exceptions */
    const *excpArraySubscriptOutOfRange := 11
    const *excpDynamicArrayUpperBoundLessThanLower := 12

    /* union tag exceptions */
    const *excpUnionFieldNotInCurrentTag := 13
    const *excpTagValueOutOfRange := 14
    const *excpUnionTagUninitialized := 34

    /* assignment exceptions */
    const *excpAssignmentValueOutOfRange := 15
    const *excpStringValueTooLarge := 35
    const *excpDynamicCharAssignedToChar := 41
    const *excpCharAssignedToDynamicChar := 42

    /* assertion failures */
    const *excpPreConditionFalse := 16
    const *excpPostConditionFalse := 17
    const *excpLoopInvariantFalse := 18
    const *excpForLoopInvariantFalse := 19
    const *excpModuleInvariantFalse := 20
    const *excpAssertConditionFalse := 21

    /* parameter and result exceptions */
    const *excpValueParameterOutOfRange := 22
    const *excpStringParameterTooLarge := 23
    const *excpResultValueOutOfRange := 24
    const *excpFunctionFailedToGiveResult := 26

    /* collection exceptions */
    const *excpCollectionElementFreed := 27
    const *excpCollectionSubscriptNil := 28

    /* set exceptions */
    const *excpSetElementOutOfRange := 29

    /* aliasing exceptions */
    const *excpBoundVariablesOverlap := 31
    const *excpReferenceParametersOverlap := 32

    /* miscellaneous exceptions */
    const *excpCaseSelectorOutOfRange := 25
    const *excpOutOfStackSpace := 30
    const *excpDivOrModByZero := 33

    /* predefined routine exceptions */
    const *excpIllegalParameterChr := 36
    const *excpIllegalParameterOrd := 37
    const *excpPredOfFirstElement := 38
    const *excpSuccOfLastElement := 39
    const *excpIllegalStringConversionBase := 106

    /* string exceptions */
    const *excpInvalidSubscriptCharString := 40
    const *excpIllegalValueInStringConversion := 43
    const *excpDynamicCharCoercedToChar := 44

    /*
     * Turing Plus Integer and Real overflow, etc.
     */
    const *excpIntegerOverflow := 50
    const *excpRealOverflow := 51
    const *excpRealUnderflow := 52


    /*
     * Turing Plus Library Aborts and Exceptions
     */

    /* library routine not * implemented */
    const *excpUnimplementedFeature := 60
    /* internal library error */
    const *excpSystemError := 61

    /* memory exceptions */
    const *excpOutOfMemory := 65
    const *excpFreeOfNilPointer := 66

    /* Conversion parameter value errors */
    const *excpNegativeFieldWidthSpecified := 67

    /* I/O exceptions */
    const *excpGetItemIllegal := 70
    const *excpIllegalArgumentNumber := 71
    const *excpArgumentDoesNotExist := 72
    const *excpIllegalStreamNumber := 73

    /* I/O illegal operations */
    const *excpGetWidthTooLargeForParameter := 74
    const *excpIoOnClosedStream := 75
    const *excpIoOnIncompatibleStream := 76
    const *excpIoOnUnopenedStream := 77
    const *excpNegativeSeekPosition := 78
    const *excpPutItemTooBig := 79
    const *excpReadPastEof := 80

    /* I/O open failed */
    const *excpIllegalOpenMode := 81
    const *excpImplicitOpenFailed := 82
    const *excpOpenOfIllegalFileName := 83
    const *excpOpenOfOpenedStream := 84

    /* I/O close errors */
    const *excpCloseOfClosedStream := 85
    const *excpCloseOfUnopenedStream := 86
    const *excpCloseOfStandardStream := 87

    /* Async I/O error */
    const *excpAsyncIo := 88

    /* string conversion exceptions */
    const *excpStringFormatIncorrect := 90
    const *excpStringReplaceLengthNegative := 91
    const *excpLeftSubstringBoundExceedsRight := 92
    const *excpResultStringTooBig := 93

    /* Math function exceptions */
    const *excpDomainError := 95
    const *excpIntegerTakenToNegativePower := 96
    const *excpNegativeValueTakenToRealPower := 97
    const *excpZeroTakenToNegativePower := 98
    const *excpZeroTakenToZeroPower := 99
    const *excpTrigArgumentTooLarge := 100

    /* Random exceptions */
    const *excpIllegalRangeForRandInt := 101
    const *excpIllegalRandSequenceNumber := 102

    /* Process exceptions */
    const *excpProcessForkFailed := 105

    /* Device Monitor exceptions */
    const *excpDevMonInvalidLevel := 110

    /* Dynamic Monitor exceptions */
    const *excpMonitorBusy := 111
    const *excpConditionBusy := 112

    /*
     * Exceptions from Unix signal numbers
     */
    const *unixSignalToException := 150
    const excpSIGBASE := unixSignalToException

    const *excpSIGHUP := excpSIGBASE + 1        /* hangup */
    const *excpSIGINT := excpSIGBASE + 2        /* interrupt */
    const *excpSIGQUIT := excpSIGBASE + 3       /* quit */
    const *excpSIGILL := excpSIGBASE + 4        /* illegal instruction */
    const *excpSIGTRAP := excpSIGBASE + 5       /* trace trap */
    const *excpSIGIOT := excpSIGBASE + 6        /* IOT instruction */
    const *excpSIGEMT := excpSIGBASE + 7        /* EMT instruction */
    const *excpSIGFPE := excpSIGBASE + 8        /* floating point exception */
    const *excpSIGKILL := excpSIGBASE + 9       /* kill */
    const *excpSIGBUS := excpSIGBASE + 10       /* bus error */
    const *excpSIGSEGV := excpSIGBASE + 11      /* segmentation violation */
    const *excpSIGSYS := excpSIGBASE + 12       /* bad argument to system call */
    const *excpSIGPIPE := excpSIGBASE + 13      /* write on a pipe with no one to read it */
    const *excpSIGALRM := excpSIGBASE + 14      /* alarm clock */
    const *excpSIGTERM := excpSIGBASE + 15      /* software termination signal from kill */
    const *excpSIGURG := excpSIGBASE + 16       /* urgent condition on IO channel */
    const *excpSIGSTOP := excpSIGBASE + 17      /* sendable stop signal not from tty */
    const *excpSIGTSTP := excpSIGBASE + 18      /* stop signal from tty */
    const *excpSIGCONT := excpSIGBASE + 19      /* continue a stopped process */
    const *excpSIGCHLD := excpSIGBASE + 20      /* to parent on child stop or exit */
    const *excpSIGTTIN := excpSIGBASE + 21      /* to readers pgrp upon background tty read */
    const *excpSIGTTOU := excpSIGBASE + 22      /* like TTIN for output */
    const *excpSIGIO := excpSIGBASE + 23        /* input/output possible signal */
    const *excpSIGXCPU := excpSIGBASE + 24      /* exceeded CPU time limit */
    const *excpSIGXFSZ := excpSIGBASE + 25      /* exceeded file size limit */
    const *excpSIGVTALRM := excpSIGBASE + 26    /* virtual time alarm */
    const *excpSIGPROF := excpSIGBASE + 27      /* profiling time alarm */
    const *excpSIGWINCH := excpSIGBASE + 28     /* window changed */
end Exceptions
