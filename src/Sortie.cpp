/*************************************************************************
                           Sortie  -  description
                             -------------------
    début                : 2014
    copyright            : (C) 2014 par Salma El Alaoui Talibi
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
#include <map>
#include <errno.h>
//----------------------------------------------------- Include personnel
#include "Entree.h"
#include "Configuration.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types
typedef map<pid_t,unsigned int> ::iterator It;
//---------------------------------------------------- Variables statiques
static map<pid_t,unsigned int> voituriersCourants;
static int DescR;
//------------------------------------------------------ Fonctions privées

static void finApplication ( int noSignal )
// Mode d'emploi : 
//-Tue les voituriers qui n'ont pas fini de sortir lors de la
//fermeture de l'application
{
	//On ignore SIGCHLD
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

static void supprimerVoiture (int place)
// Mode d'emploi : 
//-Réinitialise la voiture qui vient  de sortir dans la mémoire partagée, et
//efface la zone etat.
{
	Vehicule voiture={AUCUN,0,0};
	Vehicule old;
	MemPlace * seg=AttacheMemPlace();
	old=seg->vehicules[place-1];
	seg->vehicules[place-1]=voiture;
	DetacheMemPlace(seg);
	Effacer(TypeZone(place));
	AfficherSortie(old.usager,old.noVehicule,old.arrivee,time(NULL));
	
} //----- fin de supprimerVoiture

static int comparerPriorite (Requete req1, Requete req2)
// Mode d'emploi : 
//-valeur de retour: 
//1 si a est plus prioritaire que b
//0 si a et b ont la meme priorite
//-1 si a est moins prioritaire que b
{
	if(req1.usager== AUCUN)
	{
		return -1;
	}
	else if (req2.usager==AUCUN)
	{
		return 1;
		
	}
	else if(req1.usager!=req2.usager)
	{
		if(req1.usager==PROF)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else//meme usager, premier arrive, premier servi
	{
		if(req1.demande>req2.demande)
		{
			return 1;
		}
		if (req1.demande<req2.demande)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

} //----- fin de comparerPriorite

static void gererRequetes ( )
// Algorithme :
//-Acceder a la memoire partagée requetes trouver la requete la plus prioritaire, puis initaliser le bon semaphore. 
{
	//unsigned short sem=0;
	MemRequete* seg=AttacheMemRequete();

	//on s'assure qu'il y a une requete valide
	bool file=(seg->nbRequetes>0);
	
	/* boucle utilisée avant l'ajout de nbRequetes dans la memoire partagee
	unsigned short k=0;
	while(k<NB_BARRIERES_ENTREE && !file)
	{
		if(seg->requetes[k].usager!=AUCUN)
		{
			file=true;
			
		}
		k++;
	}
	*/

	if (file)
	{	
		unsigned short i=0;
		for (unsigned short j=1; j<NB_BARRIERES_ENTREE;j++)
		{
			if(comparerPriorite(seg->requetes[j],seg->requetes[i])>0)
			{
			
				i=j;
			}
		}
		
	//recupération du bon sémaphore
	Requete req={AUCUNE,AUCUN,0};
	//reinitialiser la requete prioritaire
	seg->requetes[i]=req;
	seg->nbRequetes--;
	DetacheMemRequete(seg);
	struct sembuf v={0,1,0};
	v.sem_num=i;
	semop(semget(memKey,5,IPC_EXCL),&v,1);
	Effacer(TypeZone(9+i));
	}
	else
	{
		DetacheMemRequete(seg);
	}
	

} //----- fin de gererRequete

static void finFils ( int noSignal )
// Mode d'emploi:
//-Efface les informations concernat la voiture qui vient de sortir
//-gere la prochaine requete

{
	int status;
	pid_t voiturier=wait(&status);
	int place= WEXITSTATUS(status);
	supprimerVoiture(place);
	voituriersCourants.erase(voiturier);
	gererRequetes();

	//Décrementer le compteur à la fin pour éviter le vol des places
	MemRequete* seg=AttacheMemRequete();
	seg->placesOc--;
	DetacheMemRequete(seg);
	
		
} //----- fin de finFils



//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Sortie ()
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
	
	DescR=open(CANAL_S,O_RDONLY);
	//---------------------------------------------------- phase moteur
	unsigned int place=-1;
	
	for(;;)
	{
		
		while((read(DescR,&place, sizeof(unsigned int)))==-1 && errno==EINTR);
		
			
			pid_t voiturier=SortirVoiture(place);
			if(voiturier !=-1)
			{
				voituriersCourants.insert(make_pair(voiturier,place));
			}
		
		
	}
		
	
} //----- fin de Sortie

