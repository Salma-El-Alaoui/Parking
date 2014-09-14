/*************************************************************************
                           Entree  -  description
                             -------------------
    début                : 2014
    copyright            : (C)2014 par Salma El Alaoui Talibi
*************************************************************************/

//---------- Réalisation de la tâche <Entree> (fichier Entree.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <cstdlib>
using namespace std;
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <errno.h>
#include <map>	
//------------------------------------------------------ Include personnel
#include "Entree.h"
#include "Configuration.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
typedef map<pid_t,Vehicule>::iterator It;
//---------------------------------------------------- Variables statiques
static map<pid_t,Vehicule> voituriersCourants;
static int DescR;
//------------------------------------------------------ Fonctions privées
static void finApplication ( int noSignal )
// Mode d'emploi : 
//-Tue les voituriers qui n'ont pas fini de se garer lors de la
//fermeture de l'application
{
	struct sigaction action;
	action.sa_handler=SIG_IGN;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;
	sigaction(SIGCHLD,&action,NULL);
	
	for(It i=voituriersCourants.begin();i!=voituriersCourants.end();i++)
	{
		kill(i->first,SIGUSR2);
	}
	
	for(It i=voituriersCourants.begin();i!=voituriersCourants.end();i++)
	{
		waitpid(i->first,NULL,0);
	}
	close(DescR);
	exit(0);
	
} //----- fin de finApplication

static void finFils ( int noSignal )
// Mode d'emploi:
//-Place les informations de la voiture ayant fini de stationner dans la memPlace(pour
//etre recuperees par la sortie)
//-affiche ces informations dans la zone etat

{
	int status;
	pid_t voiturier=wait(&status);
	Vehicule voiture=voituriersCourants.find(voiturier)->second;
	int place= WEXITSTATUS(status);

	MemPlace * seg=AttacheMemPlace();
	seg->vehicules[place-1]=voiture;
	DetacheMemPlace(seg);
			
	AfficherPlace(place,voiture.usager,voiture.noVehicule,voiture.arrivee);
	voituriersCourants.erase(voiturier);
	
		
} //----- fin de finFils

static int ouvrirCanal (TypeBarriere barriere)
// Mode d'emploi :
//retourne le descripteur du canal apres sont ouverture en lecture
// Algorithme :
//le canal ouvert est choisi en fonction du parametre d'appel d'entree
{
	
	switch(barriere)
	{
		case PROF_BLAISE_PASCAL:
			DescR=open(CANAL_P,O_RDONLY);
			return DescR;
			break;
		case AUTRE_BLAISE_PASCAL:
			DescR=open(CANAL_A,O_RDONLY);
			return DescR;
			break;
		case ENTREE_GASTON_BERGER:
			DescR=open(CANAL_AP,O_RDONLY);
			return DescR;
			break;
		default:
			return -1;
			break;
				
	}
	
} //----- fin de ouvrirCanal

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Entree (TypeBarriere barriere)
// Algorithme :
//
{
	//---------------------------------------------------- Initialisation
	extern int errno;

	//armer SIGUSR2 sur FinApplication
	struct sigaction action;
	action.sa_handler=finApplication;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;
	sigaction(SIGUSR2,&action,NULL);
	
	//armer SIGCHLD sur finFils
	struct sigaction action1;
	action1.sa_handler=finFils;
	sigemptyset(&action1.sa_mask);
	action1.sa_flags=0;
	sigaction(SIGCHLD,&action1,NULL);
	
	int canal=ouvrirCanal(barriere);
	//---------------------------------------------------- phase moteur
	Vehicule voiture;
	
	for(;;)
	{
		
		while((read(canal,&voiture, sizeof(Vehicule)))==-1 && errno==EINTR);
		
			voiture.arrivee=time(NULL);
			DessinerVoitureBarriere(barriere,voiture.usager);

			MemRequete * seg =AttacheMemRequete();
			if(seg->placesOc >= NB_PLACES)//Si le parking est complet
			{
				
				//Creer la requete
				Requete requete;
				requete.barriere=barriere;
				requete.usager=voiture.usager;
				requete.demande=voiture.arrivee;

				//la placer dans la memoire partagée
				seg->requetes[barriere-1]=requete;
				seg->nbRequetes++;
				//liberer la memoire le plus rapidement possible
				DetacheMemRequete(seg);
				
				//Affichage de la requete
				AfficherRequete(barriere,voiture.usager,voiture.arrivee);

				//choisir le bon semaphore
				struct sembuf p{0,-1,0};
				p.sem_num=barriere-1;
				while(semop(semget(memKey,5,IPC_EXCL),&p,1)==-1 && errno==EINTR);
				seg =AttacheMemRequete();
			}
		
			seg->placesOc++;
			DetacheMemRequete(seg);

			pid_t voiturier=GarerVoiture(barriere);
			if(voiturier !=-1)
			{
				voituriersCourants.insert(make_pair(voiturier,voiture));
				sleep(TEMPO);
			}
			
		
	

	}
			
	
} //----- fin de Entree

