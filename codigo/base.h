#ifndef _BASE_H_
#define _BASE_H_ 

typedef enum{
	RELAX_1,
	RELAX_2,
	RELAX_3
}Relaxation;

struct Instance{
	int** c;
	int** a;
	int* b;
	int n, m;
};

struct Solution{

	int n, m;

	int primal; 
	double dual; 
	
	double mi_primal;
	int it_primal;
	double mi_dual;
	int it_dual;
	double* G_u;
	double* G_v;
	int** X;
	int** HEUR ;
	double *u;
	double *v;
	unsigned long time_primal;
	unsigned long time_dual;
};

struct Parameter{
	int REPETITIONS;
	double MULTIPL_DUAL;
	double INIT_MI;
	double INIT_V;
	double INIT_U;
	int STEPS_CHANGE_MI;
	bool ADJUST_SUBGRADIENT;
	bool NORMALIZE_SUBGRADIENT;

};


#endif
