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
    map_t myMap;

	// The font has been free'd once for each 'new'.  Now get rid of it.
	myMap = MIO_IDGet (pmMapID, HASHMAP_ID);
	hashmap_free (myMap);
	MIO_IDRemove (pmMapID, HASHMAP_ID);
}
extern OOTint MIOHashmap_New (SRCPOS *pmSrcPos)
{
	map_t newmap;
	OOTint myID;

	newmap = hashmap_new();

	myID = MIO_IDAdd (HASHMAP_ID, newmap, pmSrcPos, "Hashmap", "Hashmap");

    // Couldn't allocate an identifier.  Return the default font ID.
    if (myID == 0)
    {
		hashmap_free(newmap);
    }
        	    
    return myID;
}

extern void	MIOHashmap_Put (OOTint pmMapID, OOTstring key,OOTint value)
{
	map_t myMap = MIO_IDGet (pmMapID, HASHMAP_ID);

	hashmap_put (myMap,key,value);
}
extern OOTint	MIOHashmap_Get (OOTint pmMapID, OOTstring key)
{
	OOTint result;
	map_t myMap = MIO_IDGet (pmMapID, HASHMAP_ID);

	hashmap_get (myMap,key,&result);

	return result; // Is 0/NULL if not found
}
extern void	MIOHashmap_Remove (OOTint pmMapID, OOTstring key)
{
	map_t myMap = MIO_IDGet (pmMapID, HASHMAP_ID);

	hashmap_remove (myMap,key);
}
