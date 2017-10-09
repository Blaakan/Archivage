#include "chemin.h"

/**
 * Fichier chemin.c
 * 
 * Contient des fonctions de gestions de chaînes de caractères et 
 * indirectement de déplacements dans une arborescence
 * 
*/

int taille(char * s)
{
	int i;
	for(i = 1 ; s[i] != '\0';i++);
	return i;
}


int equals(char* s1, char* s2)
{
	int i;
	if(taille(s1) != taille(s2))
	{
		return 0;
	}
	else
	{
		for(i=0; s1[i] != '\0';i++)
		{
			if(s1[i] != s2[i])
				return 0;
		}
	}
	return 1;
}

void remonterChemin(char *chemin, int slash)
{
	if(chemin==NULL)
		return ;
	int i=0;	
	for (i=strlen(chemin); chemin[i] !='/'&& i>=0;i--)
		;
	if(!slash)
		chemin[i]='\0';
	else
		chemin[i+1] = '\0';
}

void remonterCheminTotal(char * chem, int slash)
{
	int i;
	for(i=0; chem[i]!='/';i++)
		;
	if(!slash)
		chem[i]='\0';
	else
		chem[i+1] = '\0';
}

int nSubString(char * Str)
{
    int nSubStr = 0;
    int i;
    for( i = 0; Str[i] != '\0' ;i++)
	{
		if (Str[i] == separateur)
			nSubStr++;
	}
   return nSubStr;
}

char** split(char* Str)
{	
	int i=0;
	int nSubStr = nSubString(Str);
	int nbChaine=0;
	int nbCar=0;
	int nblet=0;
	int TailleStr = strlen(Str);
	char** TabSubStr = (char**) calloc(nSubStr + 1,sizeof(char*));	
	for( i = 0 ;  i < TailleStr && Str[i] != '\0' ; i+=(nblet+1))
	{
		nblet=0;
		int j;
		for( j = i ; Str[j] != separateur && Str[j] != '\0' ; j++,nblet++)
			;
		TabSubStr[nbChaine] =(char*) calloc((nblet+2),sizeof(char));
		for(j = i,nbCar =0 ; Str[j] != separateur && Str[j] != '\0' && j < TailleStr;j++,nbCar++)
		{
			TabSubStr[nbChaine][nbCar] = Str[j];
			TabSubStr[nbChaine][nbCar+1]='\0';
		}
		nbChaine++;
	}
	return TabSubStr;
}


void free2D(char ** Tab2D,int taille)
{
	int i=0;
	for (i=0;i<=taille;i++)
	{
		free(Tab2D[i]);
	}
	free(Tab2D);
}
