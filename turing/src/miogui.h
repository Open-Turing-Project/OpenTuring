/************/
/* miogui.h */
/************/

#ifndef _MIOGUI_H_
#define _MIOGUI_H_

/*******************/
/* System includes */
/*******************/

/******************/
/* Other includes */
/******************/
#include "turing.h"

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/

/*********/
/* Types */
/*********/
struct ArrayDimension
{
    int		lower, range;
};

struct	Language_ArrayDescriptor 
{
    unsigned int		sizeArray;
    unsigned int		sizeElement;
    unsigned int		elementCount;
    unsigned int		numDimensions;
    struct ArrayDimension	dim [255];
};

/**********************/
/* External variables */
/**********************/

/***********************/
/* External procedures */
/***********************/
extern void MIOGUI_Alert (OOTstring pmTitle, void *pmStringArray, 
			  struct Language_ArrayDescriptor *pmArrayDescriptor, 
			  OOTstring pmButton);
extern void MIOGUI_Choose (OOTstring pmTitle, void *pmStringArray, 
			   struct Language_ArrayDescriptor *pmArrayDescriptor, 
			   OOTstring pmButton1, OOTstring pmButton2, 
			   OOTstring pmButton3, OOTint pmDefaultButton);
#endif // #ifndef _MIOGUI_H_
