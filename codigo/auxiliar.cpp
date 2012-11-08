#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

using namespace std;

#include "base.h"

bool verifySum(Instance &I, Solution &S){
	int sum[I.n];
	memset(sum, 0, sizeof(sum));
	for(int i=0; i<S.m; i++){
		int add=0;
		for(int j=0; j<S.n; j++){
			add += S.HEUR[i][j];
		}
		if(add>1)return false;
	}
	return true;
}

bool verifyPeso(Instance &I, Solution &S){
	int sum[I.m];
	memset(sum, 0, sizeof(sum));
	for(int i=0; i<I.m; i++){
		for(int j=0; j<I.n; j++){
			sum[j] += I.a[i][j]*S.HEUR[i][j];
		}
	}
	return true;
}



void init_instance(Instance &inst, int n, int m){
	inst.n=n;
	inst.m=m;
	inst.c = (int **)malloc(m*sizeof(int *));
	inst.a = (int **)malloc(m*sizeof(int *));
	inst.b = (int *) malloc(m*sizeof(int ));
	for(int i=0; i<m; i++){
		inst.c[i]= (int *)malloc(n*sizeof(int));
		inst.a[i]= (int *)malloc(n*sizeof(int));
	}
}

void delete_instance(Instance &inst, int n, int m){
	for(int i=0; i<m; i++){
		free(inst.c[i]);
		free(inst.a[i]);
	}
	free(inst.c);
	free(inst.a);
	free(inst.b);
	
}

void init_solution(Solution &sol, int n, int m){
	sol.n=n;
	sol.m=m;
	sol.X = (int **)malloc(m*sizeof(int *));
	sol.HEUR = (int **)malloc(m*sizeof(int *));
	sol.dual = 0xFFFFFF;
	sol.primal = 0;
	sol.u = (double *)malloc(m*sizeof(double));
	sol.v = (double *)malloc(n*sizeof(double));
	sol.G_u = (double *)malloc(m*sizeof(double));
	sol.G_v = (double *)malloc(n*sizeof(double));
	for(int i=0; i<m; i++){
		sol.X[i]= (int *)malloc(n*sizeof(int));
		sol.HEUR[i]= (int *)malloc(n*sizeof(int));
	}
}

void delete_solution(Solution &sol, int n, int m){
	for(int i=0; i<m; i++){
		free(sol.X[i]);
		free(sol.HEUR[i]);
	}
	free(sol.X);
	free(sol.HEUR);
	free(sol.u);
	free(sol.v);
	free(sol.G_u);
	free(sol.G_v);

}

void read_param(Parameter &P, int k){
	
	char file[20]; sprintf(file, "param%d", k);
	FILE *fp = fopen(file, "r");
	char buff[100];
	while(fgets(buff, 99, fp)){
		
		if(buff[0]=='#')continue;
		char cmd[100], value[100];
		sscanf(buff, "%s = %s", cmd,value);
		if(!strcmp(cmd, "END"))
			break;
		else if(!strcmp(cmd, "REPETITIONS")){
			sscanf(value, "%d", &P.REPETITIONS);
		}else if(!strcmp(cmd, "MULTIPL_DUAL")){
			sscanf(value, "%lf", &P.MULTIPL_DUAL);
		}else if(!strcmp(cmd, "INIT_MI")){
			sscanf(value, "%lf", &P.INIT_MI);
		}else if(!strcmp(cmd, "INIT_U")){
			sscanf(value, "%lf", &P.INIT_U);
		}else if(!strcmp(cmd, "INIT_V")){
			sscanf(value, "%lf", &P.INIT_V);
		}else if(!strcmp(cmd, "STEPS_CHANGE_MI")){
			sscanf(value, "%d", &P.STEPS_CHANGE_MI);
		}else if(!strcmp(cmd, "ADJUST_SUBGRADIENT")){
			if(!strcmp(value, "YES"))		
				P.ADJUST_SUBGRADIENT = true;
			else if(!strcmp(value, "NO"))	
				P.ADJUST_SUBGRADIENT = false;	
			else
				printf("Comando inválido %s\n", buff);
		}else if(!strcmp(cmd, "NORMALIZE_SUBGRADIENT")){
			if(!strcmp(value, "YES"))		
				P.NORMALIZE_SUBGRADIENT = true;
			else if(!strcmp(value, "NO"))	
				P.NORMALIZE_SUBGRADIENT = false;	
			else
				printf("Comando inválido %s\n", buff);
		}else{
				printf("Comando inválido %s\n", buff);
		}
	}

}
