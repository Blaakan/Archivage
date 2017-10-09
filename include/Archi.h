#ifndef ARCHI_H
#define ARCHI_H

/**
 * Fichier Archi.h
 * 
 * Contient les entêtes des fonctions de Archi.c
 * 
*/

#include "fonctions.h"
#include "chemin.h"

#define TYPE_FILE 1
#define TYPE_DIR 5

static int ProcActifs=0;

static int FlagI = 0;

static int GFlag = 0;

static char* AdFirst ;


/**
 * @brief Fonction de compression d'archive
 * 
 * Compresse toutes les archive d'un repertoire passé en paramètre
 * 
 * @param DossierSource Repertoire dont on veut compresser toutes archives
 * @result int 0 si tout s'est bien terminé, 1 non
*/
int compresser(char* DossierSource);

/**
 * @brief Fonction de remplissage de buffer
 * 
 * Rempli le buffer destination par nBytes bytes du contenu de source
 * 
 * @param nBytes Nombre des bytes qui doivent être lus
 * @param source Chaine de caractère à mettre dans le buffer
 * @param destination Fichier dans lequel se trouve le buffer à remplir
 * @result void
*/
void remplirBuffer(int nBytes,char* source,FICHIER* destination);

/**
 * @brief Fonction qui écrit le type du fichier dans son buffer
 * 
 * Rajoute type à la suite du buffer de destination
 * 
 * @param type Un entier représentant le type
 * @param destination Un FICHIER* de destination
 * @result void
*/
void getType(int type, FICHIER* destination);


/**
* @brief Fonction qui écrit l'entête d'un fichier dans une archive
*
* La fonction écrit le nom sur 200 octets, puis les droits sous la forme 000xxx en octal,
* puis le type (1 fichier normal, 5 repertoire), puis la taille sur 12 octets, puis les dates
* chacune sur 6 octets
*
* @param f Fichier dont on veut écrire l'entête
* @param Archive Fichier dans lequel on veut écrire l'entête
* @result void
*/
void ecrireEnteteFichier(FICHIER * f, FICHIER * Archive);

/**
* @brief Fonction qui va traiter un dossier pour archivage
*
* Remplit une structure FICHIER pour stocker les informations puis
* appelle la fonction ecrireEnteteFichier
*
* @param dossier Le nom du dossier à archiver
* @param Archive Le FICHIER dans lequel on archive le dossier
* @result void
*/
int TraiterDossier(char * dossier, FICHIER * Archive);

/**
* @brief Fonction qui va traiter un fichier pour archivage
*
* Ouvre un fichier avec my_open pour stocker les information puis
* appelle la fonction ecrireEnteteFichier
*
* @param fichier Chemin vers le fichier à archiver
* @param Archive Le FICHIER dans lequel on archive le fichier
* @result int Retourn 1 en cas d'erreur, 0 sinon
*/
int TraiterFichier(char * fichier, FICHIER * Archive);

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
 * @brief Fonction principale récursive de parcours en profondeur
 * 
 * Cette fonction crée une archive unique pour le répertoire passé
 * en paramètre
 * 
 * @param DossierSource Chemin vers le dossier dont on veut faire l'archive
 * @param FichierCible Fichier dans lequel on veut générer l'archive
 * @param FFork Un flag qui vaut 1 si on veut pouvoir lancer des processus fils, 0 sinon
 * @param nbProc Nombre actuel de processus exécutés par le père
 * @result int 0 si touvas bien 1 sinon
*/
int parcoursRec(char * DossierSource, char FichierCible[PATH_MAX],int FFork, int nbProc);

/**
 * @brief Deuxième fonction principale pour l'archivage
 * 
 * Fonction qui lancera les processus fils sur parcoursRec en cas de flag FForh
 * 
 * @param DossierSource Chemin vers le dossier que l'on souhaite archiver
 * @param FichierCible Chemin vers le fichiers dans lequel créer l'archive
 * @param FFork Un flag qui vaut 1 si on veut pourvoirlancer eds prosses
 * @param nbProc Nombre de processus fils en cour d'exécution
 * @result int i 0 si aucune erreur, 1 sinon
*/
int SuperArchive(char* DossierSource, char* FichierCible, int FFork,int nbProc);


/**
* @brief Fonction d'initialisation de l'algorithme
*
* Initialise le chemin dans l'arborescence pour qu'il prenne en compte le répertoire principal
*
* @param nom Nom du répertoire
* @param cheminArchive Le chemin vers l'archive
* @param FFork Flag pour fork (plee
* @param cheminArchive Le chemin vers l'archive
* @result void
*/
int debut(char * nom, char * cheminArchive,int FFork,int nbProc);

#endif
