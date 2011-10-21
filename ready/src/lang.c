/************************************************************************/
/* lang.c								*/
/*									*/
/* This file is used as the basis for Language.dll, a DLL that contains	*/
/* only a few resources uses to specify which variant of Ready is to	*/
/* be run.  (To be clear, Language.dll is a complete misnomer meant to	*/
/* stop curious hackers from guessing what this DLL does.		*/
/************************************************************************/
#include <windows.h>

/************************************************************************/
/* DllMain								*/
/************************************************************************/
int WINAPI DllMain (HINSTANCE pmInstance, DWORD pmReason, LPVOID pmReserved)
{
    return TRUE;
} // DllMain
