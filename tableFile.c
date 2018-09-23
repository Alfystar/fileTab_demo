//
// Created by alfylinux on 20/09/18.
//

#include "tableFile.h"

/// Funzioni di supporto operanti sul file
FILE *openTabF(char* path)
{
	/**
	 tab1=fopen(tab1Name,"a+");   // a+ ==│ O_RDWR | O_CREAT | O_APPEND
	 l'interfaccia standard non funziona a causa dell' =_APPEND, che
	 fa SEEEEMPRE scrivere alla fine, ed è un problema, di consegueza creaiamo
	 il fd e succeddivamente il file* per creare una libreria il più possibile
	 standard C
	 Fatto ciò possediamo il nostro file R/W in standard C
	 **/
	int tabFd=open(path,O_RDWR|O_CREAT,0666);
	if(tabFd==-1)
	{
		perror("open FD for Tab take error:");
		exit(-1);
	}
	FILE *f= fdopen(tabFd,"r+");
	if(f==NULL)
	{
		perror("tab open error:");
		exit(-1);
	}
	return f;

}

int setUpTabF(FILE *tab, char *name)
{
	struct stat tabInfo;
	fstat(fileno(tab), &tabInfo);
	if(tabInfo.st_size!=0)     //il file era già esistente e contiene dei dati
	{
#ifdef DEB_STR
		printf("tabInfo.st_size=%d\n",tabInfo.st_size);
#endif
		errno=EEXIST; //file descriptor non valido
		return -1;
	}

	/// setup di firstFree
	firstFree head;
	strncpy(head.name,name,nameFirstFreeSize);
	head.counter=1;
	head.len=1;
	head.nf_id=0;
	/// setup di last
	entry last;
	memset(last.name,0,nameEntrySize);
	last.point=NULL;

	///File write
	flockfile(tab);
	if(fileWrite(tab,sizeof(firstFree),1,&head))
	{
		perror("Write FirstFree setup take error:");
		return -1;
	}
	printf("first-free setup\n");
	if(fileWrite(tab,sizeof(entry),1,&last))
	{
		perror("Write last-entry setup take error:");
		return -1;
	}
	printf("last-entry setup\n");
	funlockfile(tab);
	return 0; //all ok
}


int addEntryTabF(FILE *tab, char *name, int data)
{
	firstFree first;
	entry freeData;

	flockfile(tab);
	rewind(tab);        //posiziono il seek all'inizio per leggere la prima entry libera
	fflush(tab);    //garantisco che tutto quello che va scritto venga scritto nel file e quindi letto
	fread(&first,1, sizeof(firstFree),tab);
	funlockfile(tab);
	int enSeek = entrySeekF(tab, first.nf_id);
	if(enSeek==-1){
		perror("in addEntryTabF entrySeekF take error:");
		printf("index richiesto:%d\n",first.nf_id);
		exit(-1);
	}

	flockfile(tab);
	fseek(tab,enSeek,SEEK_SET);
	fread(&freeData,1, sizeof(freeData),tab);
	funlockfile(tab);


	if(isLastEntry(&freeData))
	{
		//se è la fine si cambiano i valori e si crea un nuovo last-entry

		/// last free diventa un dato
		freeData.point=data;
		strncpy(freeData.name,name,nameEntrySize);

		/// viene generato un nuovo last-free a fine file
		/// setup di last
		entry last;
		memset(last.name,0,nameEntrySize);
		last.point=-1;

		///first-free viene cambiato il luo next-free
		first.nf_id++;
		first.len++;
		first.counter=1;

		///File write
		flockfile(tab);
		fseek(tab,enSeek,SEEK_SET);
		if(fileWrite(tab,sizeof(entry),1,&freeData))
		{
			perror("Write entry empty take error:");
			return -1;
		}
		if(fileWrite(tab,sizeof(entry),1,&last))
		{
			perror("Write new-last-entry  take error:");
			return -1;
		}
		fseek(tab,0,SEEK_SET);
		if(fileWrite(tab,sizeof(firstFree),1,&first))
		{
			perror("Override FirstFree take error:");
			return -1;
		}
		funlockfile(tab);

	}
	else
	{
		//si trasforma in una cella dati e first-free punta la successiva
		first.nf_id=freeData.point;   //first free ora punta una nuova casella libera
		first.counter--;
		/// la prima casella libera diventa un dato
		freeData.point=data;
		strncpy(freeData.name,name,nameEntrySize);
		flockfile(tab);
		fseek(tab,enSeek,SEEK_SET);
		if(fileWrite(tab,sizeof(entry),1,&freeData))
		{
			perror("Override entry take error:");
			return -1;
		}
		rewind(tab);
		if(fileWrite(tab,sizeof(firstFree),1,&first))
		{
			perror("Override FirstFree take error:");
			return -1;
		}
		funlockfile(tab);
	}
	return 0;
}

int delEntryTabF(FILE *tab, int index)
{
	firstFree first;
	entry delData;

	flockfile(tab);
	fflush(tab);    //garantisco che tutto quello che va scritto venga scritto
	rewind(tab);        //posiziono il seek all'inizio per leggere la prima entry libera
	fread(&first,1, sizeof(first),tab);
	funlockfile(tab);

	int enDelSeek = entrySeekF(tab, index);
	if(enDelSeek==-1){
		perror("in delEntryTabF entrySeekF take error:");
		printf("index richiesto:%d\n",index);
		exit(-1);
	}
	delData.name[0]=0;  //metto la stringa a ""
	delData.point=first.nf_id;
	first.nf_id=index;
	first.counter++;

	flockfile(tab);
	fseek(tab,enDelSeek,SEEK_SET);
	if(fileWrite(tab,sizeof(entry),1,&delData))
	{
		perror("Override entry on delete take error:");
		return -1;
	}
	rewind(tab);
	if(fileWrite(tab,sizeof(firstFree),1,&first))
	{
		perror("Override FirstFree on delete take error:");
		return -1;
	}
	funlockfile(tab);
}

int entrySeekF(FILE *tab, int i)
{
	struct stat tabInfo;
	fstat(fileno(tab), &tabInfo);

	int seek=sizeof(firstFree)+ sizeof(entry) *i; // se i=0 fseek viene posizionato al byte successivo a first free
	if(seek>tabInfo.st_size- sizeof(entry)) //se seek va oltre la posizione dell'ultima entry DEVE essere errato
	{
		errno = ELNRNG;
		return -1;
	}
#ifdef DEB_STR
	printf("Entry seek:\nsize=%d\nindex=%d\nseek=%d\n",tabInfo.st_size,i,seek);
#endif
	return seek;
}

size_t lenTabF(FILE *tab)
{
	struct stat tabInfo;
	fstat(fileno(tab), &tabInfo);
	return (tabInfo.st_size-sizeof(firstFree))/sizeof(entry); // nbyte di tipo entry, diviso la dimensione ritorna il numero di elementi
}

int fileWrite(FILE *f,size_t sizeElem, int nelem,void *dat)
{
	fflush(f);   /// NECESSARIO SE I USA LA MODALITA +, serve a garantire la sincronia tra R/W
	size_t cont=0;
	while(cont != sizeElem*nelem)
	{
		if(ferror(f)!=0)    // testo solo per errori perchè in scrittura l'endOfFile Cresce
		{
			// è presente un errore in scrittura
			errno=EBADFD;   //file descriptor in bad state
			return -1;
		}
		cont += fwrite(dat, 1, sizeElem*nelem, f);
	}
	return 0;
}

/// Funzioni di supporto operanti su Tabella in Ram

int searchFirstEntry(table *t, char* search)
{
	for(int i =0;i<t->head.len;i++)
	{
		if(strcmp(t->data[i].name,search)==0)
		{
			return i;
		}
	}
	return -1;
}

int searchEntryBy(table *t, char* search, int idStart)
{
	for(int i =idStart;i<t->head.len;i++)
	{
		if(strcmp(t->data[i].name,search)==0)
		{
			return i;
		}
	}
	return -1;
}

table *makeTable(FILE *tab)
{
	fflush(tab);    //garantisco che tutto quello che va scritto venga scritto
	struct stat tabInfo;
	fstat(fileno(tab), &tabInfo);
	if(tabInfo.st_size==0)
	{
		printf("File Vuoto, o Inesistente\n");
		return;
	}

	size_t len =lenTabF(tab);

	table *t=(table *)malloc(sizeof(table));
	t->data=(entry *)calloc(len, sizeof(entry));


	flockfile(tab);
	rewind(tab);
	fread(&t->head,1, sizeof(firstFree),tab);
	fread(t->data,1, sizeof(entry)*len,tab);
	funlockfile(tab);
	t->stream=tab;
	return t;
}

void freeTable(table *t)
{
	free(t->data);
	free(t);
}


///Show funciton
void firstPrint(firstFree *f){
	printf("#1\tfirstFree data Store:\nname\t\t-> %s\ncouterFree\t-> %d\nLen\t\t-> %d\nnextFree\t-> %d\n",f->name,f->counter,f->len,f->nf_id);
	return;
}

void entryPrint(entry *e){
	printf("Entry data Store:\n??-Last-Free -> %s\tempty  -> %s\nname\t\t-> %s\npoint\t\t-> %d\n",booleanPrint(isLastEntry(e)),booleanPrint(isEmptyEntry(e)),e->name,e->point);
	return;
}

void tabPrint(table *tab)
{
	struct stat tabInfo;
	fstat(fileno(tab->stream), &tabInfo);
	if(tabInfo.st_size==0)
	{
		printf("File Vuoto, o Inesistente\n");
		return;
	}

	size_t lenFile =lenTabF(tab->stream);

	printf("-------------------------------------------------------------\n");
	printf("\tIl file ha le seguenti caratteristiche:\n\tsize=%d\n\tlenFile=%d\tlenFirst=%d\n",tabInfo.st_size,lenFile,tab->head.len);
	printf("\tsizeof(entry)=%d\tsizeof(firstFree)=%d\n",sizeof(entry),sizeof(firstFree));
	printf("\n\t[][]La tabella contenuta nel file contiene:[][]\n\n");
	firstPrint(&tab->head);
	printf("##########\n\n");
	for(int i=0; i<tab->head.len;i++)
	{
		printf("--->entry[%d]:",i);
		entryPrint((&tab->data[i]));
		printf("**********\n");
	}
	printf("-------------------------------------------------------------\n");
	return;
}

void tabPrintFile(FILE *tab)
{
	table *t=makeTable(tab);
	tabPrint(t);
	freeTable(t);
	return;
}

///funzioni di supporto operanti in ram


int isLastEntry(entry *e)
{
	if(e->name[0]==0 && e->point==-1)	return 1;
	return 0;
}

int isEmptyEntry(entry *e)
{
	if(e->name[0]==0 && e->point!=-1)	return 1;
	return 0;
}

char *booleanPrint(int i)
{
	if(i) return "True";
	else return "False";
}