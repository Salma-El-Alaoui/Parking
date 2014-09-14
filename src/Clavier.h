/*************************************************************************
                           Clavier  -  description
                             -------------------
    début                : 2014
    copyright            : (C)2014 par Salma El Alaoui
*************************************************************************/

//---------- Interface de la tâche <Clavier> (fichier Clavier.h) -------
#if ! defined (CLAVIER_H)
#define CLAVIER_H

//------------------------------------------------------------------------
// Rôle de la tâche <Clavier>
//La tache Clavier s'occupe de gérer les actions de l'utilisateur
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "/public/tp/tp-multitache/Outils.h"
//------------------------------------------------------------- Constantes
//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Clavier();
// Mode d'emploi :
//Gère ce que tape l'utilisateur sur le clavier et envoie la bonne commande
//aux taches qui conviennent.


void Commande(char code, unsigned int valeur);
// Mode d'emploi :
//Execute la commande qui correspond a l'action de l'utilisateur
// Contrat :
//

#endif // CLAVIER_H
