/*************/
/* miotime.h */
/*************/

#ifndef _MIOTIME_H_
#define _MIOTIME_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "miotypes.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern void	MIOTime_Date (OOTstring pmOOTimeStr);
extern OOTint	MIOTime_DateSec (OOTstring pmDateTimeStr);
extern OOTint	MIOTime_Elapsed (void);
extern OOTint	MIOTime_ElapsedCPU (void);
extern OOTint	MIOTime_PartsSec (OOTint pmDay, OOTint pmMon, OOTint pmYear, 
				  OOTint pmHour, OOTint pmMin, OOTint pmSec);
extern OOTint	MIOTime_Sec (void);
extern void	MIOTime_SecDate (OOTstring pmOOTimeStr, OOTint pmOOTTime);
extern void	MIOTime_SecParts (OOTint pmOOTTime, OOTint *pmDay, 
				  OOTint *pmMon, OOTint *pmYear, OOTint *pmDOW,
				  OOTint *pmHour, OOTint *pmMin, OOTint *pmSec);
extern void	MIOTime_SecStr (OOTstring pmString, OOTint pmOOTTime, 
			        OOTstring pmformatStr);


/***************************************/
/* External procedures for MIO library */
/***************************************/
extern void	MIOTime_Init (void);
extern void	MIOTime_Init_Run (void);

extern int	MIOTime_GetTicks (void);

#endif // #ifndef _MIOTIME_H_




