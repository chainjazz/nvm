// csaph.cpp : Written by Rastko P
// dj dash stkora at hotmail dot com
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>
#include "sec.h"
//nfscheck
//TODO: add key-check
//TODO: cipherng ith and jth state should be part of CS struct
//	so we don't need to declare them static; do we need these
//	to be static for any other functions? Probably not, since
//	they're one-time

int cipherde_loadkey(char *key, CipherState *cs, int flags) {
	int len;
	
	len = strlen(key);
	strncpy((UCHAR *)cs->keyloc, key, len);	
	cs->ivptr += len;	
	
	if (flags & 0x01){ //if ciphering generate IV	
		for (int i = 0; i < 10; i++)
			cs->iv[i] = rand() % 256;	
	}

	return 0;
}

int cipherde_setupkey(char *key, CipherState *cs) {
	
	int i = 0, j = 0, len = 0;
	unsigned char tmp = 0;	
	
	// copy IV to key
	if (len <= 256 && len > 0)
		strncat(cs->ivptr, (const UCHAR *)cs->iv, 10);
	
	len = strlen(cs->keyloc);

	// initalize state
	for (i = 0; i < 256; i++)
		cs->state[i] = (UCHAR)i;

	// 256 mixing operations
	for (i = 0; i < 256; i++) {
		tmp = (UCHAR)0;
		
		// add to "j" contents of state"ith" and key"nth", "n" = "i" % keylen
		j = (j + ((UINT)(cs->state[i]) + 
			(UINT)(cs->keyloc[(UINT)(i % len)])));
		j = j % 256;
		
		// swap state"ith" and state"jth"
		tmp = cs->state[j];
		cs->state[j] = cs->state[i];
		cs->state[i] = tmp;		
	}
	
	return 0;
}

unsigned char cipherde_cipher(UCHAR c, CipherState *cs) {	
	int n = 0;
	static int i = 0;
	static int j = 0;
	unsigned char	tmp = 0;

	// increment "i" index
	i += 1;
	i = i % 256; // all additions modulo 256
	// add state"ith" to "j"
	j += cs->state[i];
	j = j % 256;
	// swap state"jth" and state"ith"
	tmp = cs->state[j];
	cs->state[j] = cs->state[i];
	cs->state[i] = tmp;
	n = cs->state[j] + cs->state[i];
	n = n % 256;
	
	return c ^ (cs->state[n]);	
}