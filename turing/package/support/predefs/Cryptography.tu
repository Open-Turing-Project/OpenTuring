/*
 * Cryptography module
 *
 * NB:  DO NOT IMPORT OR INCLUDE THIS FILE INTO YOUR PROGRAM.
 *      IT WILL BE IMPLICITLY IMPORTED.
 */

unit
module pervasive Cryptography
    export DESSetKey, DESEncrypt, DESDecrypt,
	DES3Set2Key, DES3Set3Key, DES3Encrypt, DES3Decrypt

    external "crypto_dessetkey" procedure DESSetKey (keyA, keyB : nat4)

    external "crypto_desencrypt" procedure DESEncrypt (buffer : char (*),
	numChars : int)

    external "crypto_desdecrypt" procedure DESDecrypt (buffer : char (*),
	numChars : int)

    external "crypto_des3set2key"
	procedure DES3Set2Key (key1A, key1B, key2A, key2B : nat4)

    external "crypto_des3set3key"
	procedure DES3Set3Key (key1A, key1B, key2A, key2B, key3A, key3B : nat4)

    external "crypto_des3encrypt" procedure DES3Encrypt (buffer : char (*),
	numChars : int)

    external "crypto_des3decrypt" procedure DES3Decrypt (buffer : char (*),
	numChars : int)
end Cryptography
