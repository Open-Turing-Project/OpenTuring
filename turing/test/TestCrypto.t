var line : char (28) := "This is a test of the system"

var key1a : nat4 := 16#01234567
var key1b : nat4 := 16#89ABCDEF
var key2a : nat4 := 16#FEDCBA98
var key2b : nat4 := 16#76543210

Cryptography.DES3Set2Key (key1a, key1b, key2a, key2b)

Cryptography.DES3Encrypt (line, 16)
for i : 1 .. 16
    put intstr (ord (line (i)), 0, 16), " " ..
end for
put ""
Cryptography.DES3Decrypt (line, 16)
put line (1 .. 16)
