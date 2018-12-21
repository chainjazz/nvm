#ifndef SEC_H
	#define SEC_H

/* TODO: 
 * implementacija asimetricnog sifrata po
 * videu iz Cyber501
 */


typedef struct {
	unsigned char state[256];
	unsigned char keyloc[266];
	unsigned char *ivptr;
	unsigned char iv[10];
	short flags;
} CipherState;

/* typedef int BOOL; */
typedef unsigned char UCHAR;
typedef unsigned int UINT;

int cipherde_loadkey(char*, CipherState *, int flags);
int cipherde_setupkey(char *, CipherState *);
unsigned char cipherde_cipher(UCHAR, CipherState *);

// encryption flags
#define ENCRYPT 1
#define DECRYPT 0

#endif
