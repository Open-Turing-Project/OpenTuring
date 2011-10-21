/***************/
/* miocrypto.h */
/***************/

#ifndef _MIOCRYPTO_H_
#define _MIOCRYPTO_H_

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
#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif


/**********************/
/* External variables */
/**********************/

/*********************************************/
/* External procedures for Turing predefines */
/*********************************************/
extern void MIOCrypto_DES3Decrypt (char *pmCharArray, int pmCharArraySize, 
				   int pmNumChars);
extern void MIOCrypto_DES3Encrypt (char *pmCharArray, int pmCharArraySize, 
				   int pmNumChars);
extern void MIOCrypto_DES3Set2Key (uint32 key1A, uint32 key1B, 
				   uint32 key2A, uint32 key2B);
extern void MIOCrypto_DES3Set3Key (uint32 key1A, uint32 key1B, 
				   uint32 key2A, uint32 key2B, 
				   uint32 key3A, uint32 key3B);
extern void MIOCrypto_DESDecrypt (char *pmCharArray, int pmCharArraySize, 
				  int pmNumChars);
extern void MIOCrypto_DESEncrypt (char *pmCharArray, int pmCharArraySize, 
				  int pmNumChars);
extern void MIOCrypto_DESSetKey (uint32 key1A, uint32 key1B);


/***************************************/
/* External procedures for MIO library */
/***************************************/

#endif  // #ifndef _MIOCRYPTO_H_