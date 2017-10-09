#include "Archi.h"

/**
 * Fichier Archi.c
 * 
 * Contient les fonctions concernant l'archivage de fichiers
 * 
*/

void remplirBuffer(int nBytes,char* source,FICHIER* destination)
{
	int i = 0;
	for(i=0; i < nBytes && source[i] != '\0';i++)
	{
		my_putc(destination,source[i]);
	}
}

void getType(int type, FICHIER* destination)
{
	if (type == TYPE_FILE) { my_putc(destination,'1'); } //on rajoute dans le buffer de Archive le type du fichier (1 pour fichier normal, 5 pour répertoire)
	if (type == TYPE_DIR) { my_putc(destination,'5'); }
}

void ecrireEnteteFichier(FICHIER * f, FICHIER * Archive)
{
	int i=0;	
	remplirBuffer(taille(f->nom),f->nom,Archive);
	for(i = strlen(f->nom) ; i<100;i++)
		my_putc(Archive,0);	
	char mode[6];
	sprintf(mode,"%6o",f->droit);
	mode[0]='0';
	if(f->type == TYPE_DIR)
		mode[1]='0';
	remplirBuffer(6,mode,Archive);	
	getType(f->type,Archive);		
	char size[12];
	sprintf(size,"%012ld",f->size); //on crée un chaine pour stocker la taille du fichier
	remplirBuffer(12,size,Archive);
	char date[10];	
	sprintf(date,"%010ld",f->dA);	
	remplirBuffer(10,date,Archive);
	sprintf(date,"%010ld",f->dM);
	remplirBuffer(10,date,Archive);
	sprintf(date,"%010ld",f->dE);
	remplirBuffer(10,date,Archive);	
	for (i=Archive->position;i<TAILLE_BUFFER;i++) //on rajoute des 0 binaires dans le buffer pour avoir un total de 200 octets d'entête dans le fichier
		my_putc(Archive,0);	
	viderbuffer(Archive);
}


int TraiterDossier(char * dossier, FICHIER * Archive)
{
	if(dossier == NULL || Archive == NULL)
		return 1;
	FICHIER * D = (FICHIER*)malloc(sizeof(FICHIER));
	if(D == NULL)
		return 1;
	struct stat stdinf;
	int test = 0;
	if( (test = stat(dossier,&stdinf)) == -1)
		return 1;
	D -> droit = stdinf.st_mode;
	D -> type = 5;
	D -> size = 0;
	D -> nom = dossier;
	D -> mode_o = 0;
	D -> taille = 0;
	D -> position = 0;	
	time_t dernierAcces = stdinf.st_atime;
	time_t derniereModif = stdinf.st_mtime;
	time_t dernierEtat = stdinf.st_ctime;	
	D->dA = (int) dernierAcces;
	D->dM = (int) derniereModif;
	D->dE = (int) dernierEtat;			
	ecrireEnteteFichier(D,Archive);	
	free(D);
	return 0;
}


int TraiterFichier(char * fichier, FICHIER * Archive)
{
	FICHIER * f = my_open(fichier,"r");
	int c;
	if(f==NULL)
		return 1;
	ecrireEnteteFichier(f,Archive);
	c=my_getc(f);
	while(c != EOF)
	{ 
		my_putc(Archive,c);
		c=my_getc(f); 
	}	
	int i;
	for (i=0;i<(TAILLE_BUFFER-(f->size)%TAILLE_BUFFER);i++)
		my_putc(Archive,0);
			
	viderbuffer(Archive);	
	my_close(f);
	return 0;
}

int wait_all(pid)
{	
	int status;	
	if(pid != 0)
	{
		while(ProcActifs>0)
		{
			if((pid = wait(&status)) == -1)
			{
				return 1;
			}
			if(!WIFEXITED(status))
			{
				return 1;
			}
			ProcActifs--;
		}
	}
	return 0;	
}

int parcoursRec(char * DossierSource, char FichierCible[PATH_MAX],int FFork, int nbProc)
{
	int test;	
	FICHIER * A=NULL;
	DIR * repertoire=NULL;
	struct dirent * str=NULL;	
	if( ((repertoire=opendir(DossierSource)) == NULL) || ((str = readdir(repertoire)) == NULL )) // test de l'ouverture du repertoire
	{
		if(str == NULL)
			closedir(repertoire);
		errno = ENOENT;
		return 1;
	}
	A = my_open(FichierCible,"w");
	if( (test=TraiterDossier(DossierSource, A)))
	{
		errno = ECANCELED;
		return 1;
	}
	do
	{		
		if(str->d_type == DT_DIR)	
		{
			if( (equals(str->d_name,".")) || (equals(str->d_name, ".."))  )
					continue;			
			strcat(DossierSource, "/"); 
			strcat(DossierSource, str->d_name); 
			parcoursRec(DossierSource,FichierCible,0,nbProc);			
		}

		else
		if (str->d_type == DT_REG)
		{
			strcat(DossierSource,"/");
			strcat(DossierSource,str->d_name);
			if ((test = TraiterFichier(DossierSource, A)))
			{
				errno = ECANCELED;
				return 1;
			}
		}
		else
		if(!FlagI)
		{
			errno = ENOTSUP;
			return 1;			
		}		
		remonterChemin(DossierSource,0);
	}while( (str = readdir(repertoire)) );
	viderbuffer(A);
	my_close(A);
	closedir(repertoire);
	return 0;
}


int SuperArchive(char* DossierSource, char* FichierCible, int FFork,int nbProc)
{
	pid_t pid;
	int status,test;
	FICHIER * A=NULL;
	DIR * repertoire=NULL;
	struct dirent * str=NULL;
	int i;	
	if( ((repertoire=opendir(DossierSource)) == NULL) || ((str = readdir(repertoire)) == NULL )) // test de l'ouverture du repertoire
	{
		if(str == NULL)
			closedir(repertoire);
		errno = ENOENT;
		return 1;
	}
	char nSuperArchive[PATH_MAX];
	sprintf(nSuperArchive,"%s/%s",FichierCible,FichierCible);
	A = my_open(nSuperArchive,"w");
	if(A == NULL){	errno = EINVAL; return 1;}
	TraiterDossier(DossierSource, A);		
	do
	{		
		if(str->d_type == DT_DIR)	
		{
			if( (equals(str->d_name,".")) || (equals(str->d_name, "..")) )
				continue;			
			char ssDossier[PATH_MAX];
			sprintf(ssDossier,"%s/%s",DossierSource,str->d_name);
			TraiterDossier(ssDossier, A);
			char nvlArchive[PATH_MAX];
			sprintf(nvlArchive,"%s/%s",FichierCible,str->d_name);
			ProcActifs++;
			switch(pid = fork())
			{
				case -1 :
					return 1;
				break;
				
				case 0 :
					test = parcoursRec(ssDossier,nvlArchive,0,nbProc);
					my_close(A);
					closedir(repertoire);
					free(AdFirst);
					_exit(test);
				break;
				default :
					if(ProcActifs >= nbProc)
					{
						if((pid = wait(&status)) == -1)
						{
							return 1;
						}
						ProcActifs--;
					}
						
			}			
		}
		
		else
			if (str->d_type == DT_REG)
			{
				strcat(DossierSource,"/");
				strcat(DossierSource,str->d_name);
				TraiterFichier(DossierSource, A);
				remonterChemin(DossierSource,0);			

			}
		else
		if(!FlagI)
		{
			errno = ENOTSUP;
			return 1;
		}
	
		
	}while( (str = readdir(repertoire)) );
	wait_all(pid);
	closedir(repertoire);
	
	for (i=0;i<TAILLE_BUFFER;i++)
		my_putc(A,0);

	viderbuffer(A);
	my_close(A);
	return 0;
}

int debut(char * DossierSource, char * NomDossierCible,int FFork,int nbProc)
{
	int i=0;
	int retour;
	char *DossierOriginel = calloc(1,PATH_MAX);
	for(i = 0 ; i < strlen(NomDossierCible) && NomDossierCible[i] != '\0'; i++)
		DossierOriginel[i] = NomDossierCible[i];
	AdFirst=DossierOriginel;	
	mkdir(NomDossierCible,0764);
	char DossierCible[PATH_MAX];
	sprintf(DossierCible,"%s/",NomDossierCible);
	if(!FFork)
	{
		strcat(DossierCible,NomDossierCible);	
		creat(DossierCible,0777);
		retour = parcoursRec(DossierSource,DossierCible,FFork,nbProc);
	}	
	else
		retour = SuperArchive(DossierSource,NomDossierCible,FFork,nbProc);

	if(GFlag)
		compresser(DossierOriginel);
	free(AdFirst);
	return retour;
}

int main(int argc, char* argv[])
{
	//int DateFlag = 0; Nous ne prenons pas en compte le non archivage des dates
	int ForkFlag = 0;
	int ProcMax = 0;
	int nbropt = 0;
	int opt;
	while(( opt = getopt(argc,argv,"dizp:")) != -1){
		switch (opt){
			case 'p':
				ForkFlag = 1;
				ProcMax=atoi(optarg);
				nbropt+=2;
				break;
			case 'd':
				//DateFlag = 1; Nous ne prenons pas en compte le non archivage des dates
				nbropt+=1;				
				break;
			case 'i':
				nbropt+=1;
				FlagI = 1 ;
				break;
			case 'z':
				nbropt+=1;
				GFlag=1;
				break;
			default:
				printf("Usage : %s [-d] [-i] [-z] [-p n] <Repertoire> <Destination>\n",argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if(nbropt +2 != (argc-1)){
		printf("Mauvais Usage : \n Usage : %s [-d] [-i] [-p n] [-z] <Repertoire> <Destination>\n",argv[0]);
		exit(EXIT_FAILURE);		
	}
	
	if ( debut(argv[argc-2], argv[argc-1] , ForkFlag, ProcMax) )
	{
		perror("");
		return 1;
	}
	else
		return 0;
}

