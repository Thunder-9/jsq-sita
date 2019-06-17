#ifndef ECH_H
#define ECH_H

#define MAXEVENT 1000000 // Nombre maximum d'évènemets
#define EPSILON 1e-4 // Condition d'arrêt de la simulation


typedef struct Event{
	int type; // 0 pour une arrivée de client, 1 pour une fin de service
	double date; // date d'arrivée dans la file d'attente ou de fin de service
	int etat; // 0 pour un évènement non traité, 1 s'il est  traité
	int nfile; // numéro de la file. Utile pour les simulations des files 2 et 3.
	int associe; // 0 si une arrivée client n'est pas associé à une fin de service, 1 sinon
	int indiceEch; // indice d'un évènement dans l'échéancier
}event;

typedef struct echeancier{
	event tab[MAXEVENT];
	int taille;
}echeancier;

typedef struct Client Client;
struct Client
{
    double arrive;
    Client *suivant;
};

typedef struct File File;
struct File
{
    Client *first;
};


double Exp(double lamb);
void Ajouter_Ech(event e);
void Init_Ech();
event Extraire();
int condition_arret();
void arrive(File *file, double arrive);
double service(File *file);

#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif