#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "nvmas.h"


/*
 * XPLAT
 * standardni ulaz izlaz (npr. komandno preusmeravanje) je
 * osetljivo na redoslet bajtova; resenje je da se virtualni kod
 * generise na masini na kojoj se izvrsava, pomocu drugog
 * programa, ili iz vmas asemblera (TODO)
 * (PowerPC arhitektura podrzava promenu redosleda, TODO)
 */
int vmas_ctz(INT32 v) { // hardverski nezavisna "count-trailing-zeros" rutina
		int n = 0;
		int x = v;
		
		if (x == 0) return 32;
		
		if (!(x & 0x0000FFFF)) {
			n = n + 16;
			x = x >> 16;
		}
		if (!(x & 0x000000FF)) {
			n = n + 8;
			x = x >> 8;
		}
		if (!(x & 0x0000000F)) {
			n = n + 4;
			x = x >> 4;
		}
		if (!(x & 0x00000003)) {
			n += 2;
			x >>= 2;
		}
		if (!(x & 0x00000001)) {
			n++;			
		}		
			
		return n;		
	};
