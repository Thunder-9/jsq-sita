#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "echeancier.h"

double lambda; // Valeur de lambda
double temps; // Temps dans la simuation
long int n; // Nombre de clients dans la file
int N;
echeancier Ech; // Tableau d'évènements
File** files;
/* Retourne un variable aléatoire suivant une loi exponentielle de paramètre lamb */
double Exp(double lamb){
	double r = (double)random()/RAND_MAX;

	while(r==0 || r==1){
		r = (double)random()/RAND_MAX;

	}
	return -(log(r)/(lamb));

}

/* Ajoute un évènement dans l'échéancier */
void Ajouter_Ech(event e){

	if(Ech.taille<MAXEVENT){
		e.indiceEch=Ech.taille;
		Ech.tab[Ech.taille] = e;
		Ech.taille++;

	}
	else {printf("echeancier plein");exit(0);}	
}

/* Initialise l'échéancier */
void Init_Ech(){
	event e;
	e.date=0;
	e.etat=0;
	e.type=0;
	e.associe=0;
	Ech.taille=0;
	e.indiceEch=0;
	Ajouter_Ech(e);
}

/* Extrait le premier évènement non traité (celui dont la date est la plus petite)
 *	et le retourne */
event Extraire(){
	int imin;
	event min;
    if(Ech.taille%100000==0 && Ech.taille!=0){
        Ech.spindice=Ech.taille-50000;
    }
	for(int i=Ech.spindice;i<Ech.taille;i++){
		if(Ech.tab[i].etat == 0){
			min = Ech.tab[i];
			imin=i;
			break;
		}

	}

	for(int i=Ech.spindice;i<Ech.taille;i++){
		if(min.date>Ech.tab[i].date && Ech.tab[i].etat == 0){
			min=Ech.tab[i];
			imin=i;
		}
	}

	Ech.tab[imin].etat = 1;
	return min;
}
/* Condition d'arrêt de la simulation */
int condition_arret(long int n){
	if(n>eventfile*N){return 0;}
	return 1;	
}

int condition_arret_sita(){
    if(n>nmax-1){return 0;}
    return 1;   
}



void arrive(File *file, double arrive,double stime)
{
    Client *nouveau = malloc(sizeof(Client));
    if (file == NULL || nouveau == NULL)
    {
    	printf("fail arrive\n");
        exit(EXIT_FAILURE);
    }

    nouveau->arrive = arrive;
    nouveau->stime=stime;
    nouveau->suivant = NULL;

    if (file->first != NULL) /* La file n'est pas vide */
    {
        /* On se positionne à la fin de la file */
        file->last->suivant=nouveau;
        file->last=nouveau;

        // while (elementActuel->suivant != NULL)
        // {
        //     elementActuel = elementActuel->suivant;
        // }
        // elementActuel->suivant = nouveau;
    }
    else /* La file est vide, notre élément est le premier */
    {
        file->last = nouveau;
        file->first = nouveau;
    }
}	



Client* service(File *file)
{
    if (file == NULL)
    {
    	printf("fail service\n");
        exit(EXIT_FAILURE);
    }

    double arrive = -1;
    Client* elementDefile=NULL;
    /* On vérifie s'il y a quelque chose à défiler */
    if (file->first != NULL)
    {
        elementDefile = file->first;

        arrive = elementDefile->arrive;
    	// if(file->first->suivant == NULL){
    	// 	arrive= file->first->arrive;
    	// 	free(file->first);
    	// 	return arrive;
    	// }
        file->first = elementDefile->suivant;
        //free(elementDefile);
    }
   // if(file->first == NULL){printf("first inex\n");exit(1);}
    return elementDefile;
}

	




