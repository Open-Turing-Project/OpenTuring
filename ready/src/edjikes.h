/*************/
/* edjikes.h */
/*************/

#ifndef _EDJIKES_H_
#define _EDJIKES_H_

/*******************/
/* System includes */
/*******************/
#include <windows.h>

/******************/
/* Other includes */
/******************/
#include "jikesapi.h"

class ReadyJikesAPI : public JikesAPI
{
public:
    ReadyJikesAPI ();			// Constructor
    
    virtual int stat (const char *filename, struct stat *status);
    virtual void reportError (JikesError *error);
        
    virtual FileReader *read (const char *fileName);
}; // ReadyJikesAPI

#endif // #ifndef _EDJIKES_H_