/**************/
/* miomouse.h */
/**************/

#ifndef _MIOMOUSE_H_
#define _MIOMOUSE_H_

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
extern void	MIOMouse_Init_Run (void);
extern int	MIOMouse_ButtonMoved (OOTstring pmDirectionString);
extern int	MIOMouse_ButtonMoved1 (int pmDirection);
extern void	MIOMouse_ButtonWait (OOTstring pmString, OOTint *pmX, 
				     OOTint *pmY, OOTint *pmButtonNumber, 
				     OOTint *pmButtonUpDown);
extern void	MIOMouse_ButtonChoose (OOTstring pmString);
extern void	MIOMouse_Hide (void);
extern void	MIOMouse_Show (void);
extern void	MIOMouse_SetPosition (OOTint pmX, OOTint pmY);
extern void	MIOMouse_Where (OOTint *pmX, OOTint *pmY, 
				OOTint *pmButtonUpDown);

#endif // #ifndef _MIOMOUSE_H_
