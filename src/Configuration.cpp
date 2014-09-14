/*************************************************************************
                           Configuration -  description
                             -------------------
    début                : XXX
    copyright            : (C) XXX par XXX
    e-mail               : XXX
*************************************************************************/

//---------- Réalisation du module <Configuration> (fichier Configuration.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <iostream>
#include <cstdlib>
using namespace std;
#include <signal.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
//------------------------------------------------------ Include personnel
#include "Configuration.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
key_t const memKey=ftok(REFERENCE, CLEFS);
key_t const memKeyR=ftok(REFERENCE, 4);
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

MemPlace * AttacheMemPlace()

{
	//reservation de la memoire 
	int semId=semget(memKey,5,IPC_EXCL);
	struct sembuf p={MUTEX_PLACE,-1,0};
	while(semop(semId,&p,1)==-1&& errno==EINTR);
	int memPlaceId=shmget(memKey,sizeof(MemPlace),IPC_EXCL);
	return (MemPlace *)shmat(memPlaceId,NULL,0);
	
}//----- fin de AttachMemPlace

void DetacheMemPlace(MemPlace * seg)

{
	shmdt(seg);
	int semId=semget(memKey,5,IPC_EXCL);
	struct sembuf v={MUTEX_PLACE,1,0};
	semop(semId,&v,1);

}//----- fin de DetacheMemPlace

MemRequete * AttacheMemRequete()

{

	//reservation de la memoire 
	int semId=semget(memKey,5,IPC_EXCL);
	int memRequeteId=shmget(memKeyR,sizeof(MemRequete),IPC_EXCL);
	struct sembuf p={MUTEX_REQUETE,-1,0};
	while(semop(semId,&p,1)==-1&& errno==EINTR);
	return (MemRequete *)shmat(memRequeteId,NULL,0);
	
}//----- fin de AttachMemRequete

void DetacheMemRequete(MemRequete * seg)

{
	shmdt(seg);
	int semId=semget(memKey,5,IPC_EXCL);
	struct sembuf v={MUTEX_REQUETE,1,0};
	semop(semId,&v,1);

}//----- fin de DetacheMemRequete


