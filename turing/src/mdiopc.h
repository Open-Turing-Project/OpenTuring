/************/
/* mdiopc.h */
/************/

#ifndef _MDIOPC_H_
#define _MDIOPC_H_

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
extern int	MDIOPC_InPort (int pmPortAddress);
extern void	MDIOPC_OutPort (int pmPortAddress, unsigned char pmValue);
extern int	MDIOPC_ParallelGet (int pmPort);
extern void	MDIOPC_ParallelPut (int pmPort, unsigned char pmValue);

#endif // #ifndef _MDIOPC_H_
