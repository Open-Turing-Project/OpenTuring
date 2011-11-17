/*************/
/* miofont.c */
/*************/

/*******************/
/* System includes */
/*******************/
#include <malloc.h>
#include <string.h>

/****************/
/* Self include */
/****************/
#include "miohashmap.h"

/******************/
/* Other includes */
/******************/

#include "mio.h"
#include "mdio.h"

#include "hashmap.h"

#include "mioerr.h"

// Test to make certain we're not accidentally including <windows.h> which
// might allow for windows contamination of platform independent code.
#ifdef _INC_WINDOWS
xxx
#endif

/**********/
/* Macros */
/**********/

/*************/
/* Constants */
/*************/


/********************/
/* Global variables */
/********************/

/*********/
/* Types */
/*********/

/**********************/
/* External variables */
/**********************/

/********************/
/* Static constants */
/********************/

/********************/
/* Static variables */
/********************/

/******************************/
/* Static callback procedures */
/******************************/

/*********************/
/* Static procedures */
/*********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/

extern void	MIOHashmap_Free (OOTint pmMapID)
{
    hashmap *myMap;

	if (MIO_IDGetCount (pmMapID, HASHMAP_ID) > 1)
    {
		// The font has been 'new'ed several times,  Decrement the use count
		// by one.
		MIO_IDDecrement (pmMapID);
    }
    else
    {
		// The font has been free'd once for each 'new'.  Now get rid of it.
		myMap = (hashmap*)MIO_IDGet (pmMapID, HASHMAP_ID);
		deleteHashmap (myMap);
		MIO_IDRemove (pmMapID, HASHMAP_ID);
    }
}
extern OOTint MIOHashmap_New (SRCPOS *pmSrcPos)
{
	hashmap *newmap;
	OOTint myID;

	newmap = newHashmap(32); // arbitrary size hint

	myID = MIO_IDAdd (HASHMAP_ID, newmap, pmSrcPos, "Hashmap", NULL); //comparison bit is set to "Hasmap" just because

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myID == 0)
    {
		deleteHashmap(newmap);
    }
        	    
    return myID;
}

extern void	MIOHashmap_Put (OOTint pmMapID, OOTstring key,OOTint value)
{
	hashmap *myMap;
	OOTint *putval;

	putval	= (OOTint*)malloc(sizeof(OOTint));
	*putval = value;

	myMap = (hashmap *)MIO_IDGet (pmMapID, HASHMAP_ID);

	hashmapSet(myMap,putval,key);
}
extern OOTint	MIOHashmap_Get (OOTint pmMapID, OOTstring key, OOTint *result)
{
	hashmap *myMap = (hashmap*)MIO_IDGet (pmMapID, HASHMAP_ID);

	OOTint *retrieved;
	retrieved = (OOTint*)hashmapGet (myMap,key);

	if(retrieved == NULL) {
		*result = 0;
		return 0; // 0 if not found
	} else {
		*result = *retrieved;
		return 1; 
	}

	
}
extern void	MIOHashmap_Remove (OOTint pmMapID, OOTstring key)
{
	hashmap *myMap = (hashmap*)MIO_IDGet (pmMapID, HASHMAP_ID);
	OOTint *retrieved;

	retrieved = (OOTint*)hashmapRemove (myMap,key);

	if(retrieved != NULL) {
		free(retrieved);
	}
	
}
