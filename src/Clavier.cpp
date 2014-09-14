/*************************************************************************
                           Clavier  -  description
                             -------------------
    début                : 2014
    copyright            : (C)2014 par Salma El Alaoui Talibi
*************************************************************************/

//---------- Réalisation de la tâche <Clavier> (fichier Clavier.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;	
//------------------------------------------------------ Include personnel

#include "/public/tp/tp-multitache/Menu.h"
#include "Clavier.h"
#include "Configuration.h"
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
//Canaux de communication
static int DescwP;
static int DescwA;
static int DescwAP;
static int DescwS;
//Compteur des plaques de véhicule
static int Compteur=1;
//------------------------------------------------------ Fonctions privées
static Vehicule nouveauVehicule (TypeUsager unUsager)
// Mode d'emploi :
// -Cree et initialise les vehicules pour les entrees
{
	Vehicule voiture;
	voiture.usager=unUsager;
	voiture.arrivee=time(NULL);
	Compteur++;
	if(Compteur>MAX_COMPTEUR)
	{
		Compteur=1;
	}
	voiture.noVehicule=Compteur;
	return voiture;	
				
} //----- Fin de nouveauVehicule

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Clavier ()
{
	//---------------------------------------------------- Initialisation
	// ouverture des canaux en lecture
	DescwP=open(CANAL_P,O_WRONLY);
	DescwA=open(CANAL_A,O_WRONLY);
	DescwAP=open(CANAL_AP,O_WRONLY);
	DescwS=open(CANAL_S,O_WRONLY);
	
	//---------------------------------------------------- phase moteur
	for(;;)
	{
		Menu();
	}

} //----- fin de Clavier

void Commande(char code, unsigned int valeur)
{
	switch(code)
	{
		//Vehicule voiture;
		case('Q'):
		
			close(DescwP);
			close(DescwA);
			close(DescwAP);
			close(DescwS);
			exit(0);
			break;
		case('P'): //Prof
			
			if(valeur==1)//Blaise-Pascal
			{
				Vehicule voiture = nouveauVehicule(PROF);
				write(DescwP,&voiture ,sizeof(Vehicule));
			}
			if(valeur==2)//Gaston-Berger
			{
				
				Vehicule voiture = nouveauVehicule(PROF);
				write(DescwAP,& voiture,sizeof(Vehicule));
			}
			break;
		case('A'): //Autres
			
			if(valeur==1)//Blaise-Pascal
			{
				Vehicule voiture = nouveauVehicule(AUTRE);
				write(DescwA,& voiture,sizeof(Vehicule));
			}
			if(valeur==2)//Gaston Berger
			{
				Vehicule voiture = nouveauVehicule(AUTRE);
				write(DescwAP,&voiture,sizeof(Vehicule));
			}	
			break;
		case('S'):
			write(DescwS,&valeur,sizeof(unsigned int));
			break;
		default:
			break;			
	}
	
	
} //----- fin de Commande


