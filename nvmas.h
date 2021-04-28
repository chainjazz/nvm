/********************|
|* COMPILER OPTIONS */
#define PLATFORM_NIX 0
#define PLATFORM_WIN 1
#define REALARCH_PPC 0
#define REALARCH_X86 1
/* ================ */
#define	CCPLATFORM 	PLATFORM_NIX
#define CCARCH		REALARCH_X86
/* ================ *|
\********************/
	
#ifndef VMAS_H
	#define VMAS_H
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <errno.h>
	#include <sys/types.h>
	#if CCPLATFORM == PLATFORM_WIN
		#include <Winsock2.h>
	#elif CCPLATFORM == PLATFORM_NIX
		#include <unistd.h>
		#include <sys/socket.h>
		#include <sys/wait.h>
	#endif

	#if CCARCH == REALARCH_X86
		#define vmas_drn(n, f) vmas_drn_x86(n, f)
	#elif CCARCH == REALARCH_PPC
		#define vmas_drn(n, f) vmas_drn_generic(n, f)
	#endif
 
	typedef int BOOL;
	// typedef long INT64;
	typedef short INT16;
	typedef int INT32;

	typedef struct {
		// Memorijski organ (MO)
		
		INT32 mem[65535]; //memorija MO
		INT16 arm; //akumulacioni registar memorije MO
		INT32 prm; //prihvatni registar memorije MO
		INT32 *memory; // program loading point MO
		
		// Komandni organ(KO)
		
		INT32 rn; //registar naredbe KO
		INT16 xn[15]; //registar indeksa KO
		BOOL rf; // registar faze KO
		INT16 bn; //brojac naredbi KO
		
		// Aritmeticki organ (AO)
		
		INT32 ak; //akumulator AO	
	} Vmas;

	// mapa memorije (fiksna za sada)
	#define RESVSEG 0
	#define CODESEG 10
	#define DATASEG 100

	// mapirani ofseti segmenata (za virtuelne uredjaje)

	// bit-maske registra naredbi KO
	#define RNA 0x0000FFFF
	#define RNP 0x00010000
	#define RNR 0x00020000
	#define RNI	0x00040000
	#define RNXN 0x00F00000
	#define RNKO 0xFF000000

	// masinski kodovi instrukcija
	#define OP_SAB 0x1
	#define OP_ODU 0x2
	#define OP_MNO 0x3
	#define OP_DEL 0x4
	#define OP_SAF 0x5
	#define OP_ODF 0x6
	#define OP_MUA 0x7
	#define OP_AUM 0x8
	#define OP_NUA 0x9
	#define OP_PZA 0xA
	#define OP_NES 0xB
	#define OP_NUS 0xC
	#define OP_BES 0xD
	#define OP_PIR 0xE
	#define OP_PPR 0xF
	#define OP_HLT 0x10 // by rastko: HALT instruction

#endif
