#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nvmas.h"

int indeksNiske(const char *, const char *, unsigned int);
int ispisiBinarniKod(int, char *, char *, char *, char *, unsigned long *);
unsigned long kodInstrukcije(unsigned int, unsigned int);


char instable[][2][4] = {
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


int main(int argc, char *argv[]) {
	
		/*
		 * VAZNO: uvek incijalizovati znakovne niske
		 * koje se stampaju, inace ce ispis najverovatnije
		 * biti poremecen
		 */
	// u C, ovo je skracenica za iteraciju
	// TODO: odrediti dimenzije tabela i podataka pp-definicijama
	char 	ivarid[100][24] = {0}; 		// tabela promenljivih
	char 	iassok[100][4] = {0};		// polutabela koda - mnemonici
	char	iassax[100][24] = {0};		// polutabela koda - adrese		
	int 			fd = 0, fb = 0;		// opisne reci datoteka
	unsigned int 	varidCounter = 0U; 	// brojac promenljivih
	unsigned int 	iassCounter = 0U;	// brojac linija koda	
	unsigned long	omasbk[100] = {0};	// listing masinskog koda
	
	printf("VMASsembler\n-----------\n\n");
	
	do {
		printf("0x%04X : ", varidCounter + DATASEG);
		scanf("%s", ivarid + varidCounter * 24);
		varidCounter ++;			
	} while (strstr((const char*)(ivarid + varidCounter * 24 - 24), "EOF") == NULL);
		
	do {
		printf("0x%04X : ", iassCounter + CODESEG);
		scanf("%3s %s", iassok + iassCounter * 4, iassax + iassCounter * 24);		
		iassCounter ++;
	} while (strstr((const char*)(iassok + iassCounter * 4 - 4), "EOF") == NULL);
	
	// iassCounter--;
	
	ispisiBinarniKod(iassCounter, 
					(char *)instable, 
					(char *)ivarid, 
					(char *)iassok, 
					(char *)iassax,
					omasbk);
	
	// dodajemo HALT na kraju
	omasbk[iassCounter++] = kodInstrukcije(OP_HLT, 0);
	
	fb = creat(argv[argc - 1],  0666);
	write(fb, (char *)omasbk, iassCounter * 4);
	close(fb);
}

int indeksNiske(const char *tabela, const char *kljuc, unsigned int n) {
	int i = 0;
	
	for (i = 0; i < n; i++) {
		if (strstr(tabela + i, kljuc))
			return i;
	}
}

int ispisiBinarniKod(
					int duz, 
					char *mtable, 
					char *vtable, 
					char *iKod, 
					char *aKod,
					unsigned long *outKod) {
	int 			i = 0; 		// iterator
		
	for (i = 0; i < duz; i++) {
		outKod[i] = kodInstrukcije(
						(indeksNiske((const char *)mtable, 
							(const char *)(iKod + i * 4), 
							24 * 8)
						- 4) / 8,
						indeksNiske((const char *)vtable, 
							(const char *)(aKod + i * 24), 
							100 * 24)
						/ 24
					);
		printf("[0x%08X]\n", outKod[i]);
	}

	return 0;
}

unsigned long kodInstrukcije(unsigned int opkod, unsigned int adresa) {
	return opkod * 0x1000000 + adresa + DATASEG;
}
