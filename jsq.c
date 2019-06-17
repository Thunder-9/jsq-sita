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
extern echeancier Ech;
extern File ** files;

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

/* Retourne l'indice du serveur ayant la plus petite queue parmis les jsq_n tiré */
int getmin(){
	int* tempnf=malloc(sizeof(int)*N);
	int*  draw = malloc(sizeof(int)*jsq_n);
	int Ntemp=N-1;  
	for(int i=0;i<N;i++){
		tempnf[i]=i;
	} 

	for(int i=0;i<jsq_n;i++){
		int dr=random()%(Ntemp+1)	;
		draw[i]= tempnf[dr];
		tempnf[dr]=tempnf[Ntemp];
		Ntemp--;
	}

	int imin=0;
	int min=Nf[draw[0]];
	for(int i=1;i<jsq_n;i++){
		if(Nf[draw[i]]<min){
			min=Nf[draw[i]];
			imin=i;
		}
	}
	free(tempnf);
	imin=draw[imin];
	free(draw);

	return imin;
}

/* Traite l'arrivée d'un client.
 * Si le serveur gérant la file du client traité est libre, une fin de service pour ce client 
 * est rajoutée dans l'échéancier.
 * Ajoute une nouvelle arrivée de client (non traitée) dans la plus petite file. */
void Arrive_Event(event e){
	n++;
	int r = getmin();
	Nf[r]++;
	if(r==0)
		fprintf(f6,"%lf %ld\n",e.date,Nf[r]);
	arrive(files[r],e.date);
	//Ech.tab[e.indiceEch].nfile=r;
	event e1;
	e1.type = 0;
	e1.date = e.date + Exp(lambda);
	e1.etat = 0;
	e1.associe=0;
	Ajouter_Ech(e1);

	if(Nf[r]==1){
		event e2;
		e2.type = 1;
		e2.date=e.date+Exp(mu);
		e2.etat=0;
		e2.nfile=r;
		Ajouter_Ech(e2);
		service(files[r]);
		Ech.tab[e.indiceEch].associe=1;
		fprintf(f1,"%lf %lf %d\n",e.date,e2.date,e2.nfile);
		//ajoutWt((double)0);
	}
	temps = e.date;
}

/* Traite une fin de service.
 * S'il y a encore des clients dans la file d'attente, récupère le premier et
 * ajoute sa fin de service dans l'échéancier. */
void Service_Event(event e){
	if(Nf[e.nfile]>0){
		Nf[e.nfile]--;
		if(e.nfile==0)
			fprintf(f6,"%lf %ld\n",e.date,Nf[e.nfile]);
		n--;
		if(Nf[e.nfile]>0){
			event e1;
			double arrive=service(files[e.nfile]);
			//event tmp=Get_Client(e.nfile);
			e1.type = 1;
			e1.date = e.date + Exp(mu);
			e1.etat = 0;
			e1.nfile=e.nfile;
			fprintf(f1,"%lf %lf %d\n",arrive,e1.date,e1.nfile);
			Ajouter_Ech(e1);
			//ajoutWt(e.date-tmp.date);
	}
	temps=e.date;
	}
}


/* Simule la file d'attente et calcule la condition d'arrêt en fonction
 * des temps d'attente des clients */
int simulateur(FILE *f1){
	temps=0;
	event e;
	Init_Ech();

	for(int i=0;i<N;i++){
		Nf[i]=0;
	}
	while(condition_arret()){
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



/* Fonction main. Pour chaque valeur de lambda dans le fichier lambda.txt,
 * simule la file d'attente et sauvegarde les temps moyens d'attente et les 
 * 90-percentiles dans le fichier simulation_file3.data */
int main(int argc,char const *argv[]){
	clock_t d;
	clock_t f;
	d=clock();
	srandom(getpid()+time(NULL));
	FILE* f2 = fopen("lambda.txt", "r");
	FILE* f3 = fopen("mu.txt", "r");
	FILE* f4 = fopen("n.txt", "r");
	FILE* f5 = fopen("N.txt", "r");
	
	if(f2 == NULL)
		return fprintf(stderr, "lambda.txt n'existe pas\n"), -1;
	if(f3 == NULL)
		return fprintf(stderr, "mu.txt n'existe pas\n"), -1;
	if(f4 == NULL)
		return fprintf(stderr, "n.txt n'existe pas\n"), -1;
	if(f5 == NULL)
		return fprintf(stderr, "N.txt n'existe pas\n"), -1;
	
	char *res;
	char* resf;
	//int converge;
	while(fscanf(f2, "%lf\n", &lambda) != EOF){
		fseek(f3,0,SEEK_SET);
		while(fscanf(f3, "%lf\n", &mu) != EOF){
			fseek(f4,0,SEEK_SET);
			while(fscanf(f4, "%d\n", &jsq_n) != EOF){
				fseek(f5,0,SEEK_SET);
				while(fscanf(f5, "%d\n", &N) != EOF){
					printf("%lf %lf %d %d\n",lambda,mu,jsq_n,N);
					//if(&jsq_n ==NULL || &N==NULL || &lambda==NULL || &mu ==NULL){printf("erreur lecture\n");exit(1);}
					if(jsq_n<=N){
						files=malloc(sizeof(File*)*N);
						for(int i=0;i<N;i++){
							files[i]=malloc(sizeof(File));
							files[i]->first=malloc(sizeof(Client));
							files[i]->first = NULL;
						}
						asprintf(&res,"./res/simulation_JSQ_n%d_N%d_lambda%.1lf_mu%.1lf.data",jsq_n,N,lambda,mu);
						f1 =fopen(res,"w");
						asprintf(&resf,"./resf/simulation_JSQF_n%d_N%d_lambda%.1lf_mu%.1lf.data",jsq_n,N,lambda,mu);
						f6 =fopen(resf,"w");
						if(f1 == NULL)
							return fprintf(stderr, "impossible de créer ou ouvrir le fichier de résultat pour les clients\n"),-1;
						if(f6 == NULL)
							return fprintf(stderr, "impossible de créer ou ouvrir le fichier de résultat pour les files\n"),-1;
						free(res);
						free(resf);
						n=0;
						Nf=malloc(sizeof(long int)*N);
						simulateur(f1);
						free(Nf);
						fclose(f1);
						fclose(f6);
						for(int i=0;i<N;i++){
							//if(files[i]!=NULL){
								while(files[i]->first!=NULL){
									service(files[i]);
								}
								//free(files[i]->first);
								//printf("%f\n",files[i]->first->arrive);
							free(files[i]);
							
						}
						free(files);
					}

				}
			}
		}
	}
	
	fclose(f2);
	fclose(f3);
	fclose(f4);
	fclose(f5);
	f=clock();
	printf("temps de simulation : %lf\n",(f-d)/(double)CLOCKS_PER_SEC);
	exit(0);


}