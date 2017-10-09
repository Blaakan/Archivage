#include "fonctions.h"

/**
 * Fichier fonctions.c
 * 
 * Contient des fonctions de gestion de fichier ainsi que de compression
 * 
*/

FICHIER * my_open(char * nom, char * mode)
{
	FICHIER * f ;
	f = (FICHIER*)calloc(1,sizeof(FICHIER));
	f->position = 0 ;
	f->taille = 0 ;
	struct stat stdinf;
	
	stat(nom,&stdinf);
	f->nom = nom;
	f->droit = stdinf.st_mode;
	f->type = 1;
	f->size = stdinf.st_size;
	
	//DATE
	time_t dernierAcces = stdinf.st_atime;
	time_t derniereModif = stdinf.st_mtime;
	time_t dernierEtat = stdinf.st_ctime;
	
	f->dA = (int) dernierAcces;
	f->dM = (int) derniereModif;
	f->dE = (int) dernierEtat;
	
	if (strcmp(mode,"r") == 0)
	{  
		f->fd = open(nom, O_RDONLY, 0777) ;
      	f->mode_o = 0;
	}
	else
		if (strcmp(mode,"w") == 0)
		{
			f->fd = open(nom, O_WRONLY | O_APPEND | O_CREAT, 0777) ;
			f->mode_o = 1;
        }
        else
            { 
				errno=EINVAL; 
				free(f); 
				return NULL; 
			}
	if (f->fd == -1)
	{
		errno=EINVAL; 
		free(f);		
		return NULL;
	}

	return f;
}


int viderbuffer(FICHIER * f)
{	
	int i;
	int test = write(f->fd,f->buffer,f->position);
	if (f->position != TAILLE_BUFFER-1)
	{
		for (i=0;i<TAILLE_BUFFER;i++)
		{
			f->buffer[i] = '\0';
		}
	}
	if (test == -1) { return EOF; }	
	f->position=0;
	return 0;
}


int my_getc(FICHIER * f)
{
	if (f->position==0 || f->position==f->taille) 
	{
		f->position=0;
		f->taille=read(f->fd,f->buffer,TAILLE_BUFFER);
	}
	if (f->taille>0) 
	{   
		return (int) f->buffer[f->position++];
	}
	else
	{
		errno = EBADF;
		return EOF ;    
	}
}


int my_putc(FICHIER * f, int c)
{
	if (f->mode_o==0)
	{
		return EOF;
	}		
	if (f->position==TAILLE_BUFFER)
	{
		viderbuffer(f);
	}
	f->buffer[f->position]=c;
	f->position++;
	return c;
}


int my_close(FICHIER * f)
{
	if (f==NULL)
	{
		return 0;
	}
	if (f->mode_o==1 && f->position != 0)
	{
		viderbuffer(f);
	}
	int test = close(f -> fd);
	free(f);
	if (test==-1) {return EOF;}
	return 0;
}

int compresser(char* DossierSource)
{
	pid_t pid;
	DIR * repertoire=NULL;
	struct dirent * str=NULL;
	if( ((repertoire=opendir(DossierSource)) == NULL) || ((str = readdir(repertoire)) == NULL )) // test de l'ouverture du repertoire
	{
		if(str == NULL)
			closedir(repertoire);	 	
		return 1;
	}
	do
	{
		if( str->d_type == DT_REG)
		{
			switch(pid=fork())
			{
				case -1 :
					return 1;
				break;
				case 0 :					
					strcat(DossierSource,"/");
					strcat(DossierSource,str->d_name);
					execlp("gzip","gzip",DossierSource,(char*)NULL);
					return 1;
				break;
				default :
					wait(NULL);
					
			}
		}
	}while( (str = readdir(repertoire)) );
	closedir(repertoire);
	return 0;
}





