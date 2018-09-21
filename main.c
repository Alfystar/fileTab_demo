

#include "tableFile.h"

#define tab1Name "testTab1.tab"
#define tab2Name "testTab2.tab"


FILE *tab1;
char *fileName1;

int main(int argc, int *argv[]) {
	if (argc>=2)
	{
		fileName1=argv[1];
	} else{
		fileName1=tab1Name;
	}
	tab1=openTabF(fileName1);
	if(setUpTabF(tab1,"tabella numero 1")) perror("la tabella 1 ha dato l'errore:");


	if(argc>=3) //seleziona comando
	{
		if (strcmp(argv[2], "p") == 0)
		{
			tabPrintFile(tab1);
			return 0;
		}
	}
	if(argc>=4) {
		if (strcmp(argv[2], "d") == 0) {
			delEntryTabF(tab1, atoi(argv[3]));
			printf("Element [%d] free\n",atoi(argv[3]));
			return 0;
		}
	}
	if(argc>=5)
	{
		if(strcmp(argv[2],"a")==0)
		{
			if(addEntryTabF(tab1,argv[3],atoi(argv[4])))
			{
				perror("add entry take error:");
				return -1;
			}
			printf("%s [%d] succesful write\n",argv[3],atoi(argv[4]));
			return 0;
		}
	}

	fclose(tab1);
	return 0;
}
