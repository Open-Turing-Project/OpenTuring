#ifndef _MIOSDLGRAPH_H_
#define _MIOSDLGRAPH_H_

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

/***********************/
/* External procedures */
/***********************/
extern void	MIOSDLGraph_InitRun ();
extern void	MIOSDLGraph_NewWin (OOTint width,OOTint height,OOTint winMode);
extern void	MIOSDLGraph_CloseWin ();
extern void	MIOSDLGraph_Update ();

extern void	MIOSDLGraph_Line (OOTint x1,OOTint y1,OOTint x2,OOTint y2,
								OOTint r, OOTint g, OOTint b, OOTint a);


#endif // #ifndef _MIOSDLGRAPH_H_
