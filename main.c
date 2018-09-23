

#include "tableFile.h"

#define tab1Name "testTab1.tab"
#define tab2Name "testTab2.tab"


table *t;
char *fileName1;

int main(int argc, char *argv[])
{
	if (argc>=2)
	{
		fileName1=argv[1];
	} else {
		fileName1=tab1Name;
	}

	t=init_Tab(fileName1,"XXX Nome XXX");

	if(t==NULL)
	{
		printf("Errore nell'aprire la tabella");
		exit(-1);
	}
	if(argc>=3) //seleziona comando
	{
		if (strcmp(argv[2], "p") == 0)
		{
			tabPrintFile(t->stream);
			return 0;
		}


	}
	if(argc>=4)
	{
		if (strcmp(argv[2], "d") == 0)
		{
			delEntryTabF(t->stream, atoi(argv[3]));
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
			if(addEntryTabF(t->stream,argv[3],atoi(argv[4])))
			{
				perror("add entry take error:");
				return -1;
			}
			printf("%s [%d] succesfull write\n",argv[3],atoi(argv[4]));
			return 0;
		}
		if (strcmp(argv[2], "sf") == 0)
		{
			printf("name Search {%s} start from [%d] is at index: %d\n",argv[3],atoi(argv[4]),searchEntryBy(t,argv[3],atoi(argv[4])));
			return 0;
		}
	}

	help();

	fclose(t->stream);
	freeTable(t);
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
