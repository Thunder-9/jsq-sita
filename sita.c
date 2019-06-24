#include "echeancier.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

long int *Nf; // Nombre de clients dans les files de chaque serveur
extern double lambda;

double mu;
extern int N;
int jsq_n;
extern double temps;
extern long int n;
FILE* f1;
FILE* f6;
FILE* dist;
extern echeancier Ech;
extern File ** files;
int i_inter;
int* inter;

//à opt
/* Récupère le premier évènement "arrivée client" traité et non associé à une fin de service */
event Get_Client(int nfile){
	double min;
	int imin=-1;

	for(int i=0;i<Ech.taille;i++){
		if(Ech.tab[i].etat == 1 && Ech.tab[i].type == 0 && Ech.tab[i].associe == 0 && Ech.tab[i].nfile==nfile){
			min=Ech.tab[i].date;
			imin=i;
			break;
		}
	}

	for(int i=imin;i<Ech.taille;i++){
		if(Ech.tab[i].date<min && Ech.tab[i].etat == 1 && Ech.tab[i].type == 0 && Ech.tab[i].associe == 0 && Ech.tab[i].nfile==nfile){
			imin=i;
			min=Ech.tab[i].date;
			
		}
	}
	Ech.tab[imin].associe=1;
	return Ech.tab[imin];
}

/* Retourne l'indice de la file dans laquelle le client doit être envoyé */
// nclient | temps | ...
int getFile(){
	if(n>inter[i_inter] && n<nmax)
		i_inter++;
	if(i_inter>9)
		printf("%d\n",i_inter);
	return i_inter;
}

/* Traite l'arrivée d'un client.
 * Si le serveur gérant la file du client traité est libre, une fin de service pour ce client 
 * est rajoutée dans l'échéancier.
 * Ajoute une nouvelle arrivée de client (non traitée) dans la plus petite file. */
void Arrive_Event(event e){
	int r = getFile();
	Nf[r]++;
	if(r==0)
		fprintf(f6,"%lf %ld\n",e.date,Nf[r]);
	arrive(files[r],e.date);
	if(n<nmax){
	event e1;
	e1.type = 0;
	e1.date = e.date + Exp(lambda);
	e1.etat = 0;
	e1.associe=0;
	Ajouter_Ech(e1);
	}

	if(Nf[r]==1){
		event e2;
		e2.type = 1;
		float stime;
			int t= fscanf(dist,"%f\n",&stime);
			if(t==EOF){
				fprintf(stderr, "Erreur : EOF atteint dans pareto-r.txt \n");
				exit(1);
			}

		e2.date = e.date + stime;
		e2.etat=0;
		e2.nfile=r;
		Ajouter_Ech(e2);
		service(files[r]);
		Ech.tab[e.indiceEch].associe=1;
		fprintf(f1,"%lf %lf %d\n",e.date,e2.date,e2.nfile);
	}
	temps = e.date;
}

/* Traite une fin de service.
 * S'il y a encore des clients dans la file d'attente, récupère le premier et
 * ajoute sa fin de service dans l'échéancier. */
void Service_Event(event e){
	n++;
	if(Nf[e.nfile]>0){
		Nf[e.nfile]--;
		if(e.nfile==0)
			fprintf(f6,"%lf %ld\n",e.date,Nf[e.nfile]);
		if(Nf[e.nfile]>0){
			event e1;
			double arrive=service(files[e.nfile]);
			e1.type = 1;
			float stime;
			int t= fscanf(dist,"%f\n",&stime);
			if(t==EOF){
				fprintf(stderr, "Erreur : EOF atteint dans pareto-r.txt \n");
				exit(1);
			}

			e1.date = e.date + stime;
			e1.etat = 0;
			e1.nfile=e.nfile;
			fprintf(f1,"%lf %lf %d\n",arrive,e1.date,e1.nfile);
			Ajouter_Ech(e1);
	}
	temps=e.date;
	}
}


/* Simule la file d'attente et calcule la condition d'arrêt en fonction
 * des temps d'attente des clients */
int simulateur(FILE *f1){
	temps=0;
	event e;
	i_inter=0;
	n=0;
	Init_Ech();

	for(int i=0;i<N;i++){
		Nf[i]=0;
	}
	while(condition_arret_sita()){
		e =Extraire();
		if( e.type ==0){
			Arrive_Event(e);
		}
		if(e.type ==1){
			Service_Event(e);
		}
	}
	return 0;
}


void get_inter(int* inter){
	FILE *intervalle=fopen("inter.data","r");
	if(intervalle == NULL){
		fprintf(stderr, "impossible d'ouvrir le fichier inter.data\n");exit(EXIT_FAILURE);
	}
	
	int r;
	int somme=0;
	int tmp;
	for(int i=0;i<N;i++){
		r=fscanf(intervalle,"%d\n",&tmp);
		somme+=tmp;
		inter[i]=somme;
		if(r==EOF){
			 fprintf(stderr, "Erreur : EOF atteint dans intervalle.txt \n");
			 exit(1);
		}
	}
	fclose(intervalle);
}



/* Fonction main. Pour chaque valeur de lambda dans le fichier lambda.txt,
 * simule la file d'attente et sauvegarde les temps moyens d'attente et les 
 * 90-percentiles dans le fichier simulation_file3.data */
int main(int argc,char const *argv[]){
	clock_t d;
	clock_t f;
	srandom(getpid()+time(NULL));
	dist=fopen("pareto-r.data","r");
	if(dist == NULL)
		return fprintf(stderr, "impossible d'ouvrir le fichier pareto-r.data\n"),-1;
	lambda=atof(argv[1]);
	N=atoi(argv[2]);
	printf("N %d\n",N);
	inter=malloc(sizeof(int)*N+1);
	get_inter(inter);
	for(int i = 0;i<N;i++){
		printf("inter %d\n",inter[i]);
	}
	Nf=malloc(sizeof(long int)*N);
	files=malloc(sizeof(File*)*N);
	for(int i=0;i<N;i++){
		files[i]=malloc(sizeof(File));
		files[i]->first = NULL;
		}
	char* res;
	char* resf;
	asprintf(&res,"./sres/simulation_SITA_N%d_lambda%.1lf.data",N,lambda);
	f1 =fopen(res,"w");
	asprintf(&resf,"./sresf/simulation_SITAF_N%d_lambda%.1lf.data",N,lambda);
	f6 =fopen(resf,"w");
	if(f1 == NULL)
		return fprintf(stderr, "impossible de créer ou ouvrir le fichier de résultat pour les clients\n"),-1;
	if(f6 == NULL)
		return fprintf(stderr, "impossible de créer ou ouvrir le fichier de résultat pour les files\n"),-1;
	d=clock();
	simulateur(f1);
	f=clock();


	free(res);
	free(resf);
	free(inter);
	free(Nf);
	fclose(f1);
	fclose(f6);
	fclose(dist);
	for(int i=0;i<N;i++){
		while(files[i]->first!=NULL){
			service(files[i]);
		}
		free(files[i]->first);
		free(files[i]);
	}
	free(files);
	
	printf("temps de simulation : %lf\n",(f-d)/(double)CLOCKS_PER_SEC);
	exit(0);


}