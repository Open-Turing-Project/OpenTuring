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
extern void	MIOSDLGraph_Cls ();

extern void	MIOSDLGraph_SetLight (OOTint lighting);

// matrix stack
extern void	MIOSDLGraph_PushMatrix ();
extern void	MIOSDLGraph_PopMatrix ();
extern void MIOSDLGraph_ClearMatrix ();
// matrix manipulation
extern void	MIOSDLGraph_Translate (OOTreal x,OOTreal y,OOTreal z);
extern void	MIOSDLGraph_Rotate (OOTreal angle,OOTreal x,OOTreal y,OOTreal z);
extern void	MIOSDLGraph_Scale (OOTreal x,OOTreal y,OOTreal z);

extern void	MIOSDLGraph_Line (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTint r, OOTint g, OOTint b);
extern void	MIOSDLGraph_FillTriangle (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTreal x3,OOTreal y3,OOTreal z3,
								OOTint r, OOTint g, OOTint b);
extern void	MIOSDLGraph_Triangle (
								OOTreal x1,OOTreal y1,OOTreal z1, 
								OOTreal x2,OOTreal y2,OOTreal z2,
								OOTreal x3,OOTreal y3,OOTreal z3,
								OOTint r, OOTint g, OOTint b);


#endif // #ifndef _MIOSDLGRAPH_H_
