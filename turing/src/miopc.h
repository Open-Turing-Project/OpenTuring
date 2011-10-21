/***********/
/* miopc.h */
/***********/

#ifndef _MIOPC_H_
#define _MIOPC_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/

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

/***********************/
/* External procedures */
/***********************/
extern int	MIOPC_InPort (int pmPortAddress);
extern void	MIOPC_OutPort (int pmPortAddress, int pmValue);
extern int	MIOPC_ParallelGet (int pmPort);
extern void	MIOPC_ParallelPut (int pmPort, int pmValue);

#endif // #ifndef _MIOPC_H_
