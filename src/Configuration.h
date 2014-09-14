/*************************************************************************
                          Configuration -  description
                             -------------------
    début                : 2014
    copyright            : (C) 2014 par Salma El Alaoui Talibi
*************************************************************************/

//---------- Interface du module <Configuration> (fichier Configuration.h) ---------
#if ! defined ( CONFIGURATION_H )
#define CONFIGURATION_H 

//------------------------------------------------------------------------
// Rôle du module <Configuration>
//Ce module regroupe les types, ainsi que que les fonctions utilisées par toutes les taches 
//du programme
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "/public/tp/tp-multitache/Outils.h"
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
//------------------------------------------------------------- Constantes
#define CANAL_P  "CANAL_P"
#define CANAL_A  "CANAL_A"
#define CANAL_AP "CANAL_AP"
#define CANAL_S  "CANAL_S"

#define TEMPO 1
#define DROITS 0660
#define MAX_COMPTEUR 999

#define REFERENCE "./Parking"
#define CLEFS 3


extern key_t const memKey;
extern key_t const memKeyR;

//------------------------------------------------------------------ Types
struct Requete
{
	TypeBarriere barriere;
	TypeUsager usager;
	time_t demande;
};

struct Vehicule
{
	TypeUsager usager;
	int noVehicule;
	time_t arrivee;
		
};

struct MemPlace
{//Les voiture garées dans le parking.
	Vehicule vehicules[NB_PLACES];

};

struct MemRequete
{
	Requete requetes[NB_BARRIERES_ENTREE];
	unsigned int placesOc;//nombre de places occupees
	unsigned int nbRequetes;//nbRequete, redondant mais accelere le traitement
};

enum Semaphore
{
	
	 SEM_P,
	 SEM_A, 
	 SEM_AP, 
	 MUTEX_PLACE, 
	 MUTEX_REQUETE 

};

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

MemPlace * AttacheMemPlace();
// Mode d'emploi :
// reserve la memoire partagee memPlace avec le bon mutex, l'attache, 
//renvoie un pointeur sur un segment de memoire


void DetacheMemPlace(MemPlace * seg);
// detache le segment de memoire partagee memPlace precedemment attache et libere le mutex
// Contrat :
//toujours utiliser apres la fonction AttacheMemPlace()

MemRequete * AttacheMemRequete();
// reserve la memoire partagee memRequete avec le bon mutex, l'attache, 
//renvoie un pointeur sur un segment de memoire

void DetacheMemRequete(MemRequete * seg);
// detache le segment de memoire partagee memRequete precedemment attache et libere le mutex
// Contrat :
//toujours utiliser apres la fonction AttacheMemPlace()

#endif //CONFIGURATION_H

