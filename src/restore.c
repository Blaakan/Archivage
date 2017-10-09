#include "restore.h"

/**
 * Fichier restore.c
 * 
 * Contient les fonctions concernant la restauration d'archives
 * 
*/

int estCompresse(char* f)
{
	int err;
	regex_t p;
	const char *str_req  = f;
	const char *str_regex = ".gz$";
	
	err=regcomp(&p, str_regex, REG_NOSUB | REG_EXTENDED);
	if(err == 0)
	{
		int reconnu;
		reconnu = regexec(&p,str_req,0,NULL,0);
		regfree(&p);
		
		if(reconnu == 0)
			return 1;
		
		else if(reconnu == REG_NOMATCH )
			return 0;
	}
	
	return 1;	
}


int decompresser(char* DossierSource)
{
	pid_t pid;
	DIR * repertoire=NULL;
	struct dirent * str=NULL;
	if( ((repertoire=opendir(DossierSource)) == NULL) || ((str = readdir(repertoire)) == NULL )) // test de l'ouverture du repertoire
	{
		if(str == NULL)
			closedir(repertoire);	
		errno = ENOENT;
		return 1;
	}
	do
	{
		if( str->d_type == DT_REG)
		{
			
			if(estCompresse(str->d_name)==0)
				continue;

			switch(pid=fork())
			{
				case -1 :
					return 1;
				break;
				case 0 :					
					strcat(DossierSource,"/");
					strcat(DossierSource,str->d_name);
					execlp("gzip","gzip","-d",DossierSource,(char*)NULL);
					return 1;
				break;
				default :
					wait(NULL);
					
			}
		}
	}while( (str = readdir(repertoire)) );
	
	if(str == NULL)
	{
		closedir(repertoire);
		errno = ENOENT;
		return 1;
	}
	
	closedir(repertoire);
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

FICHIER * lireEntete(FICHIER * Archive)
{
	FICHIER * f = (FICHIER *)calloc(1,sizeof(FICHIER));
	if(f == NULL)
		return NULL;
		
	f->nom = (char *)calloc(100,sizeof(char));
		
	if (my_getc(Archive) == EOF)
	{
		free(f->nom);
		free(f);
		return NULL;
	}
	int i=0;
	while(Archive->buffer[i] != 0 && i<100)
	{
		f->nom[i] = Archive->buffer[i];
		i++;
	}
	//recupération droit et type
	char droitypetaille[19];
	for(i=100;i<119;i++)
	{
		droitypetaille[i-100]= Archive->buffer[i]; // 6premiers = droits | 7è type | 12 derniers taille
	}
	//Récupération dates
	char deA[10];
	char deM[10];
	char deE[10];
	
	while(i<129)
	{
		deA[i-119] = Archive->buffer[i];
		i++;
	}
	
	while(i<139)
	{
		deM[i-129] = Archive->buffer[i];
		i++;
	}
	
	while(i<149)
	{
		deE[i-139] = Archive->buffer[i];
		i++;
	}
	//conversion droit
	int j;
	for(j=1,i=5;i>2;i--,j*=8)
		f->droit += (droitypetaille[i]-48) * j;
		
	f->type = droitypetaille[6]-48;
	
	//conversion taille
	for(j=1,i=18;i>6;i--,j*=10)
		f->size += (droitypetaille[i]-48) * j;
		
	//Conversion dates 
	for(j=1,i=9;i>=0;i--,j*=10)
		f->dA += (deA[i]-48)*j;
		
	for(j=1,i=9;i>=0;i--,j*=10)
		f->dM += (deM[i]-48)*j;
		
	for(j=1,i=9;i>=0;i--,j*=10)
		f->dE += (deE[i]-48)*j;
	
	Archive -> position = 0;	
	return f;
}


void creerFichier(FICHIER * f,char chemin[PATH_MAX])
{
	strcat(chemin,f->nom);	
	f-> fd = creat(chemin,f->droit);
	if (DATEFLAG)
	{
		struct utimbuf dates;
		dates.actime=f->dA;
		dates.modtime=f->dM;
		utime(chemin,&dates);
	}
	else
		{
			utime(chemin,NULL);
		}
}


void creerRepertoire(FICHIER* dir,char chemin[PATH_MAX])
{
	strcat(chemin,dir->nom);
	mkdir(chemin,dir->droit);
	if (DATEFLAG)
	{
		struct utimbuf dates;
		dates.actime=dir->dA;
		dates.modtime=dir->dM;
		utime(chemin,&dates);
	}
	else
		{
			utime(chemin,NULL);
		}
}


int restore(char * Source, char chemin[PATH_MAX],int FFork,int nbProc)
{
	pid_t pid = 0;
	FICHIER * A = my_open(Source,"r"); 
	if (A == NULL)
	{
		errno = EINVAL;
		return 1;
	}
	int i;
	while(1)
	{ 	
		FICHIER * toRestore = lireEntete(A);
		if(toRestore == NULL || toRestore->nom == NULL || toRestore->nom[0] == 0)
		{
			if(toRestore != NULL)
			{
				if(toRestore->nom != NULL)
					free(toRestore->nom);					
				free(toRestore);
			}			
			break;
		}
		if(toRestore->type == TYPE_FILE)
		{
			creerFichier(toRestore,chemin);
			FICHIER* extract = my_open(chemin,"w");
			i = 0;
			char c;
			
			do	
			{	
				c=my_getc(A);
				if (c != 0)
					my_putc(extract, c); 
				i++;
			}while(i < toRestore->size);
			
			if (A->position != TAILLE_BUFFER-1)
			{
				for (i=0;i<TAILLE_BUFFER;i++)
				{
					A->buffer[i] = '\0';
				}
			}
			A->position = 0;
			my_close(extract);
			remonterCheminTotal(chemin,1);
		}//FIN IF TYPEFILE
		else 
			if(toRestore -> type == TYPE_DIR)
			{
				strcat(toRestore->nom,"/");
				char test[PATH_MAX];
				sprintf(test,"%s%s",chemin,toRestore->nom);
				if( access(test,F_OK) != -1)
				{
					free(toRestore->nom);
					free(toRestore);
					continue;
				}
				creerRepertoire(toRestore,chemin);
				remonterCheminTotal(chemin,1);				
				char** splitsource2 = split(Source);
				char* source2 = splitsource2[0];
				char** splitdossacree = split(toRestore->nom);
				char* dossacree = splitdossacree[nSubString(toRestore->nom)-1];
				char verifex[PATH_MAX] ;
				sprintf(verifex,"%s/%s",source2,dossacree);
				if( access( verifex, F_OK) != -1  ) 
				{
					if(FFork)
					{
						ProcActifs++;
						switch(pid = fork())
						{
							int status;
							case -1:
								return 1;
							break;
							case 0 :
								restore(verifex,chemin,0,0);
								free2D(splitdossacree,nSubString(toRestore->nom));
								free2D(splitsource2,nSubString(Source));
								free(toRestore->nom);
								free(toRestore);
								my_close(A);
								_exit(0);
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
						restore(verifex,chemin,0,0);
				}
				free2D(splitdossacree,nSubString(toRestore->nom));
				free2D(splitsource2,nSubString(Source));
			}//FIN IF TYPE DIR
		if (toRestore != NULL)
		{
			if(toRestore->nom != NULL )
				free(toRestore->nom);
			free(toRestore);
		}
	}//FIN WHILE
	if(FFork)
	{
		int t = wait_all(pid);
		if (t)
			return 1;
	}
	my_close(A);
	return 0;
}


int init(char * Source, char * Repertoire,int FFork,int nbProc)
{	
	int t;
	if (GFLAG)
	{
		t=decompresser(Source);
		if (t) 
			return 1;
	}
	mkdir(Repertoire,511);
	char chemin[PATH_MAX];
	sprintf(chemin,"%s/",Repertoire);
	char Source2[PATH_MAX];
	sprintf(Source2,"%s/%s",Source,Source);
	int retour = restore(Source2,chemin,FFork,nbProc);
	if (GFLAG)
	{
		t=compresser(Source);
		if (t) 
			return 1;
	}
	return retour;
}


int main(int argc, char* argv[])
{	
	int ForkFlag = 0;
	int ProcMax = 0;
	int nbropt = 0;
	int opt;
	while(( opt = getopt(argc,argv,"dzp:")) != -1){
		switch (opt){
			case 'p':
				ForkFlag = 1;
				ProcMax=atoi(optarg);
				nbropt+=2;
				break;
			case 'z':
				GFLAG = 1;
				nbropt++;
				break;
			case 'd':
				DATEFLAG = 1;
				nbropt+=1;				
				break;
			default:
				printf("Usage : %s [-d] [-z] [-p n] <Source> <Repertoire> \n",argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if(nbropt +2 != (argc-1)){
		printf("Mauvais Usage : \n Usage : %s [-d] [-z] [-p n] <Source> <Repertoire>\n",argv[0]);
		exit(EXIT_FAILURE);		
	}	
	if (init(argv[argc-2], argv[argc-1] , ForkFlag, ProcMax) ) 
	{
		perror("");
		return 1;
	}
	else 
		return 0;
}
