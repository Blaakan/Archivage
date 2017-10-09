#ifndef FONCTIONS_H
#define FONCTIONS_H

/**
 * Fichier fonctions.h
 * 
 * Contient les entêtes des fonctions de fonctions.c
 * 
*/


#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <utime.h>
#include <regex.h>


#define TAILLE_BUFFER 200


/**
*	Structure : FICHIER
*	Contient :
*		- int fd : file descriptor
*		- int mode_o : mode d'ouverture du fichier
*		- char buffer[] : tableau de caractère servant de tampon
*		- int position : position du curseur dans le buffer
*		- int taille : quantité de données dans le buffer
* 		- int droit : droits d'accès au fichier
* 		- int type : type de fichier
* 		- long size : taille du fichier
* 		- long int : date de dernier accès
* 		- long int : date de dernière modification
* 		- long int : date de dernière modification de l'état
* 		- char * nom : nom du fichier
*/
typedef struct{
	int fd;
	int mode_o;
	char buffer[TAILLE_BUFFER];
	int position,taille;
	int droit;
	int type;
	long size;
	long int dA;
	long int dM;
	long int dE;
	char * nom;
}FICHIER;

/**
* @brief Fonction analogue à fopen
*
* Ouvre un fichier avec un certain mode d'ouverture (écriture ou lecture)
* Renvoie NULL en cas d'erreur et met errno à EINVAL, renvoie le fichier sinon
*
* @param nom Chemin du fichier depuis le répertoir courant
* @param mode Mode d'ouverture du fichier
* @result Un pointeur sur structure FICHIER
*/
FICHIER * my_open(char * nom, char * mode);

/**
* @brief Ecrit toutes les données restantes du buffer dans un fichier
*
* Appelle de cette fonction quand on ferme le fichier pendant une écriture, ou quand on
* essaye d'écrire alors que le buffer est rempli. Cette fonction va, respectivement,
* écrire les données restantes dans le fichier, ou écrire toutes les données du buffer,
* selon la situation.
* 
* @param f La structure FICHIER liée au fichier dans lequel on écrit.
* @result Renvoie 0 si réussi, EOF si l'écriture a échoué
*/
int viderbuffer(FICHIER * f);


/**
* @brief Fonction analogue à getc
*
* Fonctionne comme getc (même prototype et valeurs de retour) mais en utilisant une entrée bufferisée
*
* @param f Structure FICHIER liée au fichier à lire
* @result Le caractère courant du buffer, ou EOF en cas d'erreur
*/
int my_getc(FICHIER * f);

/**
* @brief Fonction analogue à putc
*
* Fonctionne comme putc (même prototype et valeurs de retour) mais en utilisant une sortie bufferisée
* 
* @param f Structure FICHIER liée au fichier dans lequel écrire
* @param c Caractère à placer dans le buffer
* @result Renvoie le caractère si réussi, EOF si une erreur
*/
int my_putc(FICHIER * f, int c);

/**
* @brief Fonction analogue à fclose
*
* Fonctionne comme fclose (même prototype et valeurs de retour) mais en vidant les données restantes du buffer si besoin
* 
* @param f Structure FICHIER liée au fichier à fermer et où éventuellement vider le buffer
* @result Renvoie 0 réussi, EOF si une erreur
*/
int my_close(FICHIER * f);

/**
* @brief Fonction qui compresse le contenu d'un DossierSource
*
* Fonction qui compresse tous les fichiers contenus dans un dossier
* Appel de cette fonction pour compresser les archives
* 
* @param DossierSource Dossier dont on souhaite compresser le contenu
* @result int Renvoie 1 en case d'erreur, 0 sinon
*/
int compresser(char* DossierSource);


#endif








