#ifndef RESTORE_H
#define RESTORE_H

/**
 * Fichier restore.h
 * 
 * Contient les entêtes des fonctions de restore.c
 * 
*/


#include "fonctions.h"
#include "chemin.h"

/**
 * 
 * Définitions de deux types pour désigner les fichiers réguliers ou les répertoires
 * 
*/
#define TYPE_FILE 1
#define TYPE_DIR 5


/**
 * 
 * Définition de 3 variables globales
 * 		- DATEFLAG : indique si on restaure les dates ou non
 * 		- ProcActifs : nombre de processus fils actifs
 * 		- GFLAG : indique si on utilise la compression et/ou décompression avec gZ
 * 
*/
static int DATEFLAG = 0;
static int ProcActifs=0;
static int GFLAG = 0;

/**
 * @brief Vérifie qu'un fichier est compressé
 *
 * Vérifie qu'un fichier du répertoire courant correspond à un format *.gz
 * 
 * @param f Nom du fichier à vérifier
 * @return int Renvoie 1 si il correspond au format, 0 sinon
*/
int estCompresse(char* f);

/**
 * @brief Décompresse un répertoire
 * 
 * Parcourt le répertoire et décompresse les fichiers au format *.gz
 * Renvoie une erreur si aucun fichier n'a été trouvé
 * 
 * @param DossierSource Dossier à parcourir
 * @return int Renvoie 1 en cas d'erreur, 0 sinon
*/
int decompresser(char* DossierSource);


/**
 * @brief Fonction qui lit une entête dans un fichier d'archive
 * 
 * Lit un fichier d'archive à la position courante (qui doit donc être
 * à la position d'une entête) et stocke les informations dans
 * une structure FICHIER
 * 
 * @param Archive Le fichier d'archive qu'on lit
 * @return FICHIER* Structure FICHIER contenant les informations de l'entête
*/
FICHIER * lireEntete(FICHIER * Archive);

/**
 * @brief Fonction qui crée un fichier
 * 
 * Crée un fichier à l'emplacement indiqué par le chemin et avec les
 * informations contenues dans la structure FICHIER
 * 
 * @param f Structure FICHIER contenant les informations
 * @param chemin Chemin utilisé pour créer le fichier
 * @return void
*/
void creerFichier(FICHIER * f,char chemin[PATH_MAX]);

/**
 * @brief Fonction qui crée un répertoire
 * 
 * Crée un répertoire à l'emplacement indiqué par le chemin et avec les
 * informations contenues dans la structure FICHIER
 * 
 * @param dir Structure FICHIER content les informations
 * @param chemin Chemin utilisé pour créer le fichier
 * @return void
*/
void creerRepertoire(FICHIER* dir,char chemin[PATH_MAX]);


/**
 * @brief Fonction qui attend la terminaison de tous les processus fils
 * 
 * Sera utilisée dans l'exécution avec fork() pour ne pas quitter sans 
 * que certain fils n'ai déjà renvoyé de réponse
 * 
 * @param pid Pid du père qui attend les fils
 * @result int Retourne 1 en cas d'erreur, 0 sinon
*/
int wait_all(pid_t pid);


/**
 * @brief Fonction récursive principale pour l'algorithme de restauration
 * 
 * Lit une archive passée en paramètre
 * Si l'entête désigne un fichier, appelle la fonction creerFichier
 * Si l'entête désigne un répertoire, appelle la fonction creerDossier et relance restore sur l'archive associée
 * 
 * @param Source Chemin vers l'archive
 * @param chemin Chemin vers la destination de restauration
 * @param FFork Flag indiquant l'utilisation ou non de processus fils
 * @param nbProc Nombre maximum de processus fils exécutables si FFork est activé
 * @return int 0 si aucune erreur, 1 sinon
*/
int restore(char * Source, char chemin[PATH_MAX],int FFork,int nbProc);

/**
 * @brief Fonction d'initialisation de l'algorithme
 * 
 * Initialise l'algorithme avec le nom du répertoire passé en paramètre du programme
 * 
 * @param Source Chemin vers l'archive principale
 * @param Repertoire Nom du répertoire dans lequel restaurer les archives
 * @param FFork Flag indiquant l'utilisation ou non de processus fils
 * @param nbProc Nombre maximum de processus fils exécutables si FFork est activé
 * @return void
*/
int init(char * Source, char * Repertoire,int FFork,int nbProc);



#endif
