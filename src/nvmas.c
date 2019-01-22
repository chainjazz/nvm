#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "nvmas.h"
#include "nsec.h"

#define CIPHERKEY "VMAS"

int vmas_drn(INT32, INT32); // dekoder registra naredbi
int vmas_drn_generic(INT32, INT32); // dekoder registra naredbi (genericki CTZ)
int vmas_ctz(INT32); // genericki CTZ
int vmas_fpn(Vmas *); // faza pripreme naredbe
int vmas_fin(Vmas *); // faza izvrsenja naredbe
int vmas_dump(Vmas *, INT16); // prikaz sadrzaja memorije (debug monitor)
int vmas_dumpreg(Vmas *); // prikaz sadrzaja registra (debug monitor)
int vmas_instostring(char *, INT16, BOOL); // pretty-print listing
int vmas_bios(Vmas *); // BIOS rutina

static const char *instable[255][2] = {
		{ " ", "???" },
		{ "+", "SAB" },
		{ "-", "ODU" },
		{ "*", "MNO" },
		{ "/", "DEL" },
		{ "+", "SAF" },
		{ "-", "ODF" },		
		{ " ", "MUA" },
		{ "=", "AUM" },
		{ "~", "NUA" },
		{ "!", "PZA" },
		{ "F", "NES" },
		{ "T", "NUS" },
		{ ">", "BES" },
		{ "'", "PIR" },
		{ "f", "PPR" },
		{ "_", "HLT" },		
	};

int main() {	
	// int sockets[2];
	Vmas vm = {{0}, 0, 0, 0, 0, {0}, 0, 0, 0};
	Vmas *vmptr = &vm;
	// CipherState cs;
	// CipherState *csptr = &cs;
	
	// TODO: All print operations should be implemented as instructions
	// putting data into memory segment mapped to the screen; we can treat
	// this as a terminal device, a graphics card, or even an SVG canvas
	// The recommended segment is 65000-65535, size times 4 (8-bit)
	// and perhaps dividing by 3 if we need more color (24-bit)
	//		machine code driver
	//			*get MMIO input register (MUA)
	//			*divide it into decimal digits (DEL, AUM...)
	//			*for each digit, compare it with the abcdefg 7seg table (ODU, NUS)
	//			*put resulting numbers into a private memory array for display
	//			-- virtual "hardware" part
	//			*write private memory array to file/socket
	//			*interpret the file by PHP/JS as SVG data
	
	// TODO: implement all this in JavaScript using OO; later add sockets
	// so we have a JS assembler and an SVG output that communicate with
	// the binary via sockets
	
	////////
	// setup vm initial state (should we delegate this to BIOS routine?)
	
	vm.memory = vm.mem + CODESEG; //point memory member past reserved memory
	
	////////
	// setup cipher data
	
	//bytes = read(0, csptr->iv, 10); //TODO: from socket?
	//cipherde_loadkey(CIPHERKEY, csptr, DECRYPT);
	// ... write IV if ciphering (ENCRYPT flag) ...
	//cipherde_setupkey(CIPHERKEY, csptr);
	
	// start BIOS routine
	vmas_bios(vmptr);		

	////////
	// start execution	
	
			
	
	printf("{Final register state}\n");
	vmas_dumpreg(vmptr);	
	
	return 0;
}

int vmas_fpn(Vmas *vm) { // faza pripreme naredbe (modifikacija adresnog dela)
	vm->arm = vm->bn; // premesti sadrzaj brojaca naredbi u adresni reg. memorije
	vm->prm = vm->memory[vm->arm]; // ocitaj sadrzaj memorije[arm] u prihvatni reg.
	vm->rn = vm->prm; // premesti sadrzaj prihvatnog reg. memorije u registar naredbe
	
	if (vmas_drn(vm->rn, RNP)) // ako imamo priznak indeksne modifikacije
		vm->rn += vm->xn[vmas_drn(vm->rn, RNXN)]; // uvecaj adresni deo za indeks
	else if (vmas_drn(vm->rn, RNR)) // ako imamo relativni priznak modifikacije
		vm->rn += vm->arm; // uvecaj adresni deo za sadrzaj adresnog reg. memorije
	
	while (vmas_drn(vm->rn, RNI)) { // sve dok imamo priznak indirektne modifikacije
		vm->arm = vmas_drn(vm->rn, RNA); // premesti sadrzaj adresnog dela u adresni reg.
		vm->prm = vm->memory[vm->arm]; // ocitaj sadrzaj memorije[arm]
		vm->rn = vm->rn & RNKO; // zadrzi kod operacije u registru naredbe
		vm->rn += vm->prm & 0x00FFFFFF; // ocitaj ostatak naredbe iz prihvatnog reg.
		
		if (vmas_drn(vm->rn, RNP)) // ako imamo priznak indeksne modifikacije...
			vm->rn += vm->xn[vmas_drn(vm->rn, RNXN)];
		else if (vmas_drn(vm->rn, RNR)) // ako imamo relativni priznak modifikacije...
			vm->rn += vm->arm;
	}
	
	vm->bn += 1; // uvecaj brojac naredbi
	vm->rf = 1; // postavi fazu na izvrsenje (rezervisano za implementaciju kloka)
	
	return 0;
}

int vmas_fin(Vmas *vm) { // faza izvrsenja naredbe
	INT16 naredba;
	
	naredba = vmas_drn(vm->rn, RNKO);
	
	switch(naredba) {
		case OP_HLT:
			return 0;
			break;
		case OP_SAB:
			vm->ak = vm->ak + vm->memory[vmas_drn(vm->rn, RNA)];
			break;
		case OP_ODU:
			vm->ak = vm->ak - vm->memory[vmas_drn(vm->rn, RNA)];
			break;
		case OP_MNO:
			vm->ak = vm->ak * vm->memory[vmas_drn(vm->rn, RNA)];
			break;
		case OP_DEL:
			vm->ak = vm->ak / vm->memory[vmas_drn(vm->rn, RNA)];
			break;
		case OP_MUA:
			vm->ak = vm->memory[vmas_drn(vm->rn, RNA)];
			break;
		case OP_AUM:
			vm->memory[vmas_drn(vm->rn, RNA)] = vm->ak;
			break;
		case OP_NUA:
			vm->ak = 0;
			break;
		case OP_PZA:
			vm->ak = ~(vm->ak);
			break;
		case OP_NES:
			if (vm->ak < 0)
				vm->bn = vmas_drn(vm->rn, RNA);
			break;
		case OP_NUS:
			if (vm->ak == 0)
				vm->bn = vmas_drn(vm->rn, RNA);
			break;
		case OP_BES:
			vm->bn = vmas_drn(vm->rn, RNA);
			break;
		case OP_PIR:
			vm->xn[vmas_drn(vm->rn, RNXN)] = vmas_drn(vm->rn, RNA);
			break;
		case OP_PPR:
			vm->rn = vm->rn & 0xFFFF0000 + vm->bn;
			vm->bn = vmas_drn(vm->rn, RNA) + 1;
			break;		
		default:
			printf("Illegal instruction @ 0x%04X\n", vm->bn - 1 + CODESEG);
			exit(1); // ilegalna naredba (rezervisano za implementaciju nastavka rada)
			break;
	}
	
	vm->rf = 0; // postavi fazu na pripremu (rezervisano za implementaciju kloka)
	
	return 1;
}

int vmas_drn(INT32 rn, INT32 f) {
	vmas_drn_generic(rn, f);
}

int vmas_drn_x86(INT32 rn, INT32 f) { 
	return (rn & f) >> (__builtin_ctz(f)); // hardware- or compiler-specific; re-implement for each distinct platform
}	

int vmas_drn_generic(INT32 rn, INT32 f) {
	return (rn & f) >> vmas_ctz(f);
}

int vmas_dump(Vmas *vm, INT16 l) {
	// TODO: create a DUMP instruction instead
	
	int i;
	INT16 naredba, adresa;		
	
	i = 0;
	naredba = adresa = 0;
	
	printf(" BEGIN DUMP\n");
	
	printf("{Data segment list}\n");
	
	for (i = DATASEG; i < DATASEG + l; i++)		
			printf("0x%04X %08x\t\n", i, vm->memory[i]);
	
	printf("\n");
	printf("{Disassembly}\n");
	
	for (i = 0; i < l; i++) {
		naredba = vmas_drn(vm->memory[i], RNKO);
		adresa = vmas_drn(vm->memory[i], RNA);	
	
		printf("0x%04X: %s 0x%04X\t\n", CODESEG + i, 
			instable[naredba][1], 
			adresa);						
	}
	
	printf(" END DUMP\n\n");
	
	return 0;
	
}

int vmas_dumpreg(Vmas *vm) {	
	printf("[AK:%4d]\n", vm->ak);	
	
	return 0;
}

int vmas_load(Vmas *vm, int fd) {
	int i, j;
	INT32 buf[4096];
	INT32 *bufptr = buf;
	// TODO: BIOS bi trebalo da bude ROM sa instrukcijama, a ne funkcija

	i = j = 0;
	
	// decipher and load program
	while ( ( read(fd, bufptr, sizeof(INT32)) ) ) {
		if ( vmas_drn(*((INT32 *)bufptr), RNKO) == 0 )
			// kako v.masina zna granicu izmedju podataka i koda?
			// odgovor je podaci idu na fiksnu adresu DATASEG
			*(vm->memory + DATASEG + i++) = *((INT32*)bufptr);
		else
			*(vm->memory + j++) = *((INT32*)bufptr); // memory already points to CODESEG
		
		bufptr++;	
		
	} //TODO: from socket?	
	
	return i + j;
}

int vmas_exec(Vmas *vm) {
	vmas_fpn(vm);
	
	while(vmas_fin(vm)) {
		printf("(tick)");
		vmas_dumpreg(vm);
		vmas_fpn(vm);
	}
	
	printf("!halt!\n");
		
	return 0;
}

int vmas_bios(Vmas *vm) {
	int dumpsize;
	
	dumpsize = 0;

	printf("\n\n============\n=== VMAS ===\n============\n\n");		
	printf("\n\n Loading program... \n\n");
	
	dumpsize = vmas_load(vm, 0); // load (REDIREKCIJA STD ULAZA)
	vmas_dump(vm, dumpsize); // show assembly
	vmas_exec(vm); // execute
	
	printf("\n\n Program done. No more code, exiting...\n");	
	
	// TODO: create a DUMP instruction instead	
		
	return 0;
}
