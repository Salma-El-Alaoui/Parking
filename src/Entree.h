/*************************************************************************
                           Entree  -  description
                             -------------------
    début                : 2014
    copyright            : (C) 2014 par Salma El Alaoui
*************************************************************************/

//---------- Interface de la tâche <Entree> (fichier Entree.h) -------
#if ! defined (ENTREE_H )
#define ENTREE_H

//------------------------------------------------------------------------
// Rôle de la tâche <Entree>
//Cette tache gere une barriere d'entree (fait rentrer une voiture par 
//la bonne porte en fonction des parametres d'appel).
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "/public/tp/tp-multitache/Outils.h"

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions puœbliques
void Entree (TypeBarriere entree);
// Mode d'emploi :
//lance une tache qui gere la barriere d'entree entree


#endif // ENTREE_H

