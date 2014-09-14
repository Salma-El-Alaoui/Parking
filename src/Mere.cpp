/*************************************************************************
                           Mere  -  description
                             -------------------
    début                : 2014
    copyright            : (C)2014 par Salma El Alaoui
*************************************************************************/

//---------- Réalisation de la tâche <Mere> (fichier Mere.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <cstdlib>
using namespace std;
#include <signal.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>		
//------------------------------------------------------ Include personnel
#include "/public/tp/tp-multitache/Outils.h"
#include "/public/tp/tp-multitache/Menu.h"
#include "/public/tp/tp-multitache/Heure.h"
#include "Mere.h"
#include "Clavier.h"
#include "Entree.h"
#include "Configuration.h"
#include "Sortie.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
static void initialisation ()
// Mode d'emploi :
//Cree tous les objets de communication et/ou synchronisation
{
	//creation de clefs pour les ipcs:
	
	
	//creation des canaux de communication
	if(mkfifo(CANAL_P,DROITS)==-1)
	{
		cerr<< "erreur dans la creation du canal-p"<<endl;
	}
	else if(mkfifo(CANAL_A,DROITS)==-1)
	{
		cerr<< "erreur dans la creation du canal-a"<<endl;
	}
	else if(mkfifo(CANAL_AP,DROITS)==-1)
	{
		cerr<< "erreur dans la creation du canal-ap"<<endl;
	}
	else if(mkfifo(CANAL_S,DROITS)==-1)
	{
		cerr<< "erreur dans la creation du canal-s"<<endl;
	}
	int memPlaceId;
	if ((memPlaceId=shmget(memKey,sizeof(MemPlace),IPC_CREAT|IPC_EXCL|DROITS))==-1)
	{
		cerr<< "erreur dans la creation de la memoire partagée places"<<endl;
	}
	else
	{
		//initialisation de la memoire partagée:
		
		//attachement
		MemPlace * seg=(MemPlace *)shmat(memPlaceId,NULL,0);
		//modification
		Vehicule voiture={AUCUN,0,0};
		for (size_t i=0; i< NB_PLACES;i++)
		{
			seg->vehicules[i]=voiture;
			
		}
		//detachement
		shmdt(seg);
	}
	int semId;
	if ((semId=semget(memKey,5,IPC_CREAT|IPC_EXCL|DROITS))==-1)
	{
		
		cerr<< "erreur dans la creation du semaphore general"<<endl;

	}
	else
	{		
		semctl(semId,SEM_P,SETVAL,0);
		semctl(semId,SEM_A,SETVAL,0);
		semctl(semId,SEM_AP,SETVAL,0);
		semctl(semId,MUTEX_PLACE,SETVAL,1);
		semctl(semId,MUTEX_REQUETE,SETVAL,1);

	}
	int memRequeteId;
	if ((memRequeteId=shmget(memKeyR,sizeof(MemRequete),IPC_CREAT|IPC_EXCL|DROITS))==-1)
	{
		cerr<< "erreur dans la creation de la memoire partagée places"<<endl;
	}
	else
	{
		//initialisation de la memoire partagée:
		
		//attachement
		MemRequete * seg=(MemRequete *)shmat(memRequeteId,NULL,0);
		//modification
		Requete req={AUCUNE,AUCUN,0};
		for (size_t i=0; i< NB_BARRIERES_ENTREE;i++)
		{
			seg->requetes[i]=req;
		
		}
		seg->placesOc=0;
		seg->nbRequetes=0;
		//detachement
		shmdt(seg);
	}

} //----- fin de initialisation

static void destruction ()
// Mode d'emploi :
//Detruit tous les objets de communication et/ou synchronisation
{
	int memRequeteId=shmget(memKeyR, sizeof(MemRequete), IPC_EXCL);
	shmctl(memRequeteId,IPC_RMID, 0);
	
	int semId=semget(memKey,5,IPC_EXCL);
	semctl(semId,0,IPC_RMID,0);
	

	int memPlaceId=shmget(memKey, sizeof(MemPlace), IPC_EXCL);
	shmctl(memPlaceId,IPC_RMID, 0);
	
	//Suppression des canaux de communication
	unlink(CANAL_P);
	unlink(CANAL_A);
	unlink(CANAL_AP);
	unlink(CANAL_S);	

} //----- fin de destruction

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main ()
// Algorithme :
//
{
	pid_t clavierPid;
	pid_t heurePid;
	pid_t entreeAPid;
	pid_t entreePPid;
	pid_t entreeAPPid;
	pid_t sortiePid;


	//---------------------------------------------------- Initialisation
	
	initialisation();
	
	//Initialisation de l'application
	InitialiserApplication(XTERM);

	//activation de l'horloge
	heurePid=ActiverHeure();
	
	if((entreePPid=fork())==0)
	{
		Entree(PROF_BLAISE_PASCAL);
	}
	else if((entreeAPid=fork())==0)
	{
		Entree(AUTRE_BLAISE_PASCAL);
	}
	else if((entreeAPPid=fork())==0)
	{
		Entree(ENTREE_GASTON_BERGER);
	}
	else if((sortiePid=fork())==0)
	{
		Sortie();
	}
	else if((clavierPid=fork())==0)
	{
		Clavier();
	}
	else
	{
	//---------------------------------------------------- Phase moteur
							//synchro de fin avec le clavier
							waitpid(clavierPid,NULL,0);
		
		
	//---------------------------------------------------- Destruction
	
							//Signaux de fin aux taches filles 
							kill(sortiePid,SIGUSR2);
							kill(entreeAPPid,SIGUSR2);
							kill(entreeAPid,SIGUSR2);
							kill(entreePPid,SIGUSR2);
							kill(heurePid,SIGUSR2);

							//synchro de fin avec les filles
							waitpid(sortiePid,NULL,0);
							waitpid(entreeAPPid,NULL,0);
							waitpid(entreeAPid,NULL,0);
							waitpid(entreePPid,NULL,0);
							waitpid(heurePid,NULL,0);


							destruction();


							//fin de l'application
							TerminerApplication(true);

							return 0;


						
	}

		
} //----- fin de main


