#ifndef CHEMIN_H
#define CHEMIN_H

/**
 * Fichier chemin.h
 * 
 * Contient les entêtes des fonctions de chemin.c
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
#define separateur '/'

/** 
 * @brief Fonction qui calcule la taille d'une chaine de caractères
 * 
 * 
 * 
 * @param s Chaine dont on calcule la taille
 * @return int Nombre de caractères dans la chaine
*/
int taille(char * s);

/** 
 * @brief Fonction qui vérifie si deux chaines sont identiques en taille et en contenu
 * 
 * Renvoie un booléen
 * 
 * @param s1 Première chaine à comparer
 * @param s2 Seconde chaine à comparer
 * @return int Renvoie 0 si c'est faux, 1 si c'est vrai
*/
int equals(char* s1, char* s2);

/** 
 * @brief Fonction qui remonte dans un chemin
 * 
 * Place un caractère '\0' juste après ou sur le dernier caractère '/' 
 * trouvé selon si slash vaut 1 ou 0.
 * 1 : Le caractère est conservé
 * 0 : Le caractère n'est pas conservé
 * 
 * @param chemin Chemin dans lequel on souhaite remonter
 * @param slash Flag déterminant si on souhaite écraser le dernier '/' ou non
 * @return void
*/
void remonterChemin(char *chemin, int slash);

/** 
 * @brief Fonction qui remonte au répertoire racine dans un chemin
 * 
 * Place un caractère '\0' juste après ou sur le premier '/'
 * trouvé selon si slashvaut 1 ou 0.
 * 1 : le caractère est conservé
 * 0 : le caractère n'est pas conservé
 * 
 * @param chem Chemin dans lequel on souhaite remonter
 * @param slash Flag déterminant si on souhaite écraser le dernier '/' ou non
 * @return void
*/
void remonterCheminTotal(char * chem, int slash);

/** 
 * @brief Fonction qui calcule le nombre de sous chaines dans une chaine
 * 
 * Incrémente un compteur quand on détecte un caractère égale au séparateur
 * défini dans chemin.h
 * 
 * @param Str Chaine dont on compte les sous chaines
 * @return int Le nombre de sous chaines
*/
int nSubString(char * Str);

/** 
 * @brief Divise une chaine de caractères en un tableau de chaines de caractères
 * 
 * Le tableau alloué dynamiquement contiendra chaque sous chaines de Str
 * avec comme séparateur défini dans chemin.h
 * La chaine d'origine n'est pas modifiée
 * 
 * @param Str Chaine que l'on divise dans un tableau
 * @return char** Le tableau de sous chaines
*/
char** split(char* Str);

/** 
 * @brief Desalloue un tableau de chaines de caractères
 * 
 * Il faut seulement connaitre le nombre de chaines contenues dans le tableau
 * pour pouvoir  faire appel à cette fonction
 * 
 * @param Tab2D Tableau que l'on souhaite désaouller
 * @param taille Nombre de chaines dans le tableau
 * @return void
*/
void free2D(char ** Tab2D,int taille);


#endif
