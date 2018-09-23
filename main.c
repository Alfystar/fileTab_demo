

#include "tableFile.h"

#define tab1Name "testTab1.tab"
#define tab2Name "testTab2.tab"


FILE *tab1;
table *t;
char *fileName1;

int main(int argc, int *argv[])
{
	if (argc>=2)
	{
		fileName1=argv[1];
	} else {
		fileName1=tab1Name;
	}
	tab1=openTabF(fileName1);
	t=makeTable(tab1);
	if(setUpTabF(tab1,"tabella numero 1")) perror("la tabella 1 ha dato l'errore:");


	if(argc>=3) //seleziona comando
	{
		if (strcmp(argv[2], "p") == 0)
		{
			tabPrintFile(tab1);
			return 0;
		}


	}
	if(argc>=4)
	{
		if (strcmp(argv[2], "d") == 0)
		{
			delEntryTabF(tab1, atoi(argv[3]));
			printf("Element [%d] free\n",atoi(argv[3]));
			return 0;
		}
		if (strcmp(argv[2], "s") == 0)
		{
			printf("name Search {%s} is at index: %d\n",argv[3],searchFirstEntry(t,argv[3]));
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
		if (strcmp(argv[2], "sf") == 0)
		{
			printf("name Search {%s} start from [%d] is at index: %d\n",argv[3],atoi(argv[4]),searchEntryBy(t,argv[3],atoi(argv[4])));
			return 0;
		}
	}

	help();

	fclose(tab1);
	return 0;
}

void help()
{
	printf("Comandi disponibili:\n");
	printf("-> [fileName] [parameter] [argv] ...\n");
	printf("Parameter List:");
	printf("\n(0)arg\n");
	printf("\tp\t\tPrinta il file indicato\n");
	printf("(1)arg\n");
	printf("\td [index]\tElimina nel file la entry indicata\n");
	printf("\ts [Name]\tCerca a quale entry si trova il nome cercato\n");
	printf("(2)arg\n");
	printf("\ta [Name] [index]\tAggiunge una entry con questi parametri alla Tabella\n");
	printf("\tsf [Name] [Start index]\tCerca a quale entry si trova il nome cercato partendo da [Start Index]\n");

}
