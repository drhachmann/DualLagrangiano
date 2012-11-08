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
#include "auxiliar.h"

/*apenas para uso no windows */
#define GetTickCount() 0
#define DWORD int

#define EPS 0.00000005



/*resolve o problema da mochila*/
double solve_mochila(double valor[], int peso[], int n_itens, int peso_max, int saida[]){
   double mtx[n_itens+1][peso_max+1];
   for(int P=0; P<=peso_max; P++){
      mtx[0][P] = 0;
		for(int i=1; i<=n_itens; i++){
			if(peso[i-1] <=P)
				mtx[i][P] = max(mtx[i-1][P], mtx[i-1][P-peso[i-1]] + valor[i-1]);	
			else
				mtx[i][P] = mtx[i-1][P];		
		}
   }
   for(int i=n_itens, j=peso_max; i>0; i--){
		if(mtx[i][j] == mtx[i-1][j]){
			saida[i-1] = false;
		}
		else{
			saida[i-1] = true;
			j -= peso[i-1];
		}
	}
   return mtx[n_itens][peso_max];
}


//deixa viável para a restrição de uma tarefa para um agente
void heuristic_one_task(Instance I, Solution S){
	for(int i=0; i<S.m; i++){
	   double maior=-1;
	   int ind=-1;
	   for(int j=0; j<S.n; j++){
	      if(S.HEUR[i][j]){
	         if(I.c[i][j] > maior){
	            maior = I.c[i][j] ;
	            ind = j;
	         }
	         S.HEUR[i][j]=0;
	      }
	   }
	   if(ind!=-1 && maior >0)
		   S.HEUR[i][ind] = 1;
	}
}

//deixa viável para a restrição de carga dos agentes
void heuristic_bag(Instance I, Solution S){
	double value[I.m];
	int weight[I.m];
	for(int j=0; j<I.n; j++){	
		int cont=0;
		int IND[I.m];
		for(int i=0; i<I.m; i++){
			if(S.HEUR[i][j]==1){
				value[cont]= I.c[i][j];
				weight[cont] = I.a[i][j];				
				IND[cont]=i;
				cont++;
				S.HEUR[i][j]=0;		
			}
		}
		int saida[I.m];
		solve_mochila(value, weight, cont, I.b[j], saida);
		for(int i=0; i<cont; i++){
			if(saida[i]){				
		  	  S.HEUR[IND[i]][j] = saida[i];    
			}
		}
	}
}

//recebe uma solução viável e tenta melhorá-la
void heuristic_aumenta_sem_condicao(Instance &I, Solution &S){
	int B[I.n];
	int T[I.m];
	
	memset(T, 0, sizeof(T));
	memset(B, 0, sizeof(B));
	for(int j=0; j<I.n; j++){
		B[j]=0;
		for(int i=0; i<I.m; i++){
			B[j] += S.HEUR[i][j]*I.a[i][j];
			T[i] += S.HEUR[i][j];
		}
	}
	for(int i=0; i<I.m; i++){
		if(T[i])continue;
		int maior=-1;
		int ind=-1;
		for(int j=0; j<I.n; j++){
			if(I.a[i][j]<=I.b[j]-B[j] && maior < I.c[i][j]){
				maior=I.c[i][j];
				ind=j;
			}
		}
		if(ind!=-1 && maior >0){
			S.HEUR[i][ind]=1;
			B[ind]+=I.a[i][ind];
			T[i]=1;
		}
	}
	int sum=0;
	for(int i=0; i<I.m; i++){
		sum += S.HEUR[i][0]*I.a[i][0];
	}
}

/*calcula o divisor do passo*/
double calc_div_step(double v[], double G[], int n, bool adjust){
	double sum=0;
	for(int i=0; i<n; i++){//agentes
		if(adjust && v[i]<EPS && G[i] >0){
			//not
		}
		else sum += G[i]*G[i];
	}
	return sum;
}

/*atualiza o pi se necessário*/
void update_pi(double &ant, int dual, int &rep, double &val, int steps){
	if(ant-dual){
	   ant=dual;
		rep=0;
	}else{
	   rep++;
	}
	if(rep>=steps){
	   rep=0;
		val *= .5;
	}
}

/*relaxacao 1*/
void solve1(Instance &I, Solution &S, Parameter &P, int &lim_inf, double &lim_sup){
	
	for(int j=0; j<I.n; j++){
		for(int i=0; i<I.m; i++){
			if(I.c[i][j] - S.u[i] - I.a[i][j]*S.v[j] > 0){
				S.X[i][j]=1;
				S.HEUR[i][j]=1;
			}
			else{
				S.X[i][j]=0;
				S.HEUR[i][j]=0;
			}
		}
	}

	heuristic_bag(I, S);
	heuristic_one_task(I, S);
	heuristic_aumenta_sem_condicao(I, S);

	lim_inf=0;
	lim_sup=0;
	for(int i=0; i<I.m; i++){
	   for(int j=0; j<I.n; j++){
		  lim_inf += S.HEUR[i][j]*I.c[i][j]; 
		  lim_sup += S.X[i][j]*(I.c[i][j]-S.u[i]);
	   }
		lim_sup += S.u[i];
	}

	for(int j=0; j<I.n; j++)	
		lim_sup += S.v[j]*I.b[j];

	for(int i=0; i<I.m; i++){//tarefas
		S.G_u[i]=1;
	for(int j=0; j<I.n; j++)//agentes
		S.G_u[i]-=S.X[i][j];
	}

	for(int j=0; j<I.n; j++){//agentes
		S.G_v[j]=I.b[j];
		for(int i=0; i<I.m; i++){//tarefa
			S.G_v[j] -= I.a[i][j]*S.X[i][j];
		}
	}
}

/*relaxacao 2*/
void solve2(Instance &I, Solution &S, Parameter &P, int &lim_inf, double &lim_sup){
  
	int weight[I.m];
	double value[I.m];
	for(int j=0; j<I.n; j++){	
	   for(int i=0; i<I.m; i++){
		  value[i]=I.c[i][j]-S.u[i];
		  weight[i] = I.a[i][j];
	   }
	   int saida[I.m];
	   solve_mochila(value, weight, I.m, I.b[j], saida);
	   for(int i=0; i<I.m; i++){
		  S.X[i][j] = saida[i];
		  S.HEUR[i][j] = saida[i];    
	   }
	}

	heuristic_one_task(I,S);
	heuristic_aumenta_sem_condicao(I,S);
	
	lim_inf=0;
	lim_sup=0;
	for(int i=0; i<I.m; i++){
	   for(int j=0; j<I.n; j++){
		  lim_inf += S.HEUR[i][j]*I.c[i][j]; 
		  lim_sup += S.X[i][j]*(I.c[i][j]-S.u[i]);
	   }
	}

	for(int i=0; i<I.m; i++){
		lim_sup += S.u[i];
	}

	for(int i=0; i<I.m; i++){//tarefas
	   S.G_u[i]=1;
	   for(int j=0; j<I.n; j++)//agentes
			 S.G_u[i]-=S.X[i][j];
	}
}

/*relaxacao 3*/
void solve3(Instance &I, Solution &S, Parameter &P, int &lim_inf, double &lim_sup){
	
	for(int i=0; i<I.m; i++)
		for(int j=0; j<I.n; j++){
			S.X[i][j]=0;
			S.HEUR[i][j]=0;
	}

	for(int i=0; i<I.m; i++){//tarefas
		double maior=-1;
		int ind=-1;
		for(int j=0; j<I.n; j++){//agentes
			if(maior < I.c[i][j]-I.a[i][j]*S.v[j]){
				maior = I.c[i][j]-I.a[i][j]*S.v[j];
				ind=j;
			}
		}

		if(ind!=-1 && maior > 0){
			S.X[i][ind]=1;
			S.HEUR[i][ind]=1;
		}	
	}
	for(int j=0; j<I.n; j++){//agentes
		S.G_v[j]=I.b[j];
		for(int i=0; i<I.m; i++){//tarefa
			S.G_v[j] -= I.a[i][j]*S.X[i][j];
		}
	}

	heuristic_bag(I, S);
	heuristic_aumenta_sem_condicao(I,S);

	lim_inf=0; //heurística
	lim_sup=0;
	for(int j=0; j<I.n; j++){ //agentes
		for(int i=0; i<I.m; i++){ //tarefas
			lim_inf += I.c[i][j] * S.HEUR[i][j];
			lim_sup += (I.c[i][j] - I.a[i][j]*S.v[j]) * S.X[i][j];
		}
		lim_sup+=S.v[j]*I.b[j];
	}

}

/*principal*/
void core(Instance &I, Solution &S, Parameter &P, Relaxation R){
	DWORD start_time = GetTickCount();	

	for(int i=0; i<I.m; i++)
		S.u[i] = P.INIT_U;
	for(int j=0; j<I.n; j++)
		S.v[j]= P.INIT_V;
	

	double ant=-1;
	int rep=0;
	double val=P.INIT_MI;
	int BEST_PRIMAL[I.m][I.n];
	
	
	for(int q=0; q<P.REPETITIONS; q++){
		int lim_inf;
		double lim_sup;
		if(R==RELAX_1)
			solve1(I, S, P, lim_inf, lim_sup); //relax 1
		else if(R==RELAX_2)
			solve2(I, S, P, lim_inf, lim_sup); //relax 2
		else if(R==RELAX_3)
			solve3(I, S, P, lim_inf, lim_sup); //relax 3
		if(S.dual > lim_sup+EPS){
			S.it_dual = q;
			S.mi_dual = val;
			S.dual = lim_sup;
			S.time_dual = GetTickCount() - start_time;
		}

		
		if(S.primal < lim_inf-EPS){
			S.it_primal = q;
			S.mi_primal = val;
			S.primal = lim_inf;
			S.time_primal =  GetTickCount() - start_time;	
			for(int i=0; i<I.m; i++)
				for(int j=0; j<I.n; j++)
					BEST_PRIMAL[i][j] = S.HEUR[i][j];
		}	

		if(S.dual == S.primal)
			break;
		
		if(R==RELAX_1 || R==RELAX_3){//apenas estas utilizam o gradiente v
			double div_v = calc_div_step(S.v, S.G_v, I.n, P.ADJUST_SUBGRADIENT);
			div_v++;//evita possível divisão por 0
			double mi_v =  val*(P.MULTIPL_DUAL*lim_sup-S.primal)/div_v;
			for(int j=0; j<I.n; j++){
				S.v[j] = max(S.v[j] - mi_v*S.G_v[j],0.0);
			}	
		}
		if(R==RELAX_1 || R==RELAX_2){//apenas estas utilizam o gradiente u
			double div_u = calc_div_step(S.u, S.G_u, I.m, P.ADJUST_SUBGRADIENT);
			div_u++;//evita possível divisão por 0
			double mi_u =  val*(P.MULTIPL_DUAL*lim_sup-S.primal)/div_u;
			for(int j=0; j<I.m; j++){
				S.u[j] = max(S.u[j] - mi_u*S.G_u[j],0.0);
			}
		}
		
		if(R==RELAX_1 && P.NORMALIZE_SUBGRADIENT==true){//somenta a relaxacao 1 possui dois gradientes
			for(int j=0; j<I.n; j++){
				S.v[j]/=I.b[j]; 
			}
		}
	 	update_pi(ant, S.dual, rep, val, P.STEPS_CHANGE_MI);
	}
	
	for(int i=0; i<I.m; i++)
		for(int j=0; j<I.n; j++)
			S.HEUR[i][j] = BEST_PRIMAL[i][j];

}


int main(int argc, char **argv){
	if(argc!=4){
		printf("NUmero de argumentos invAlidos\n");
		printf("./relaxlag [1,2,3] input output");
		exit(-1);
	}

	
	int k;
	sscanf(argv[1],"%d", &k);
	FILE *input = fopen(argv[2], "r");
	if(input==NULL){
	   perror ("The following error occurred");
		exit(-1);
	}

	/*FILE *output = stdout;*/
	FILE *output = fopen(argv[3], "w");
	if(output==NULL){
	   perror ("The following error occurred");
		exit(-1);
	}

	Parameter P;
	read_param(P, k);
   Instance inst;
	Solution S;
	
	int n, m;

	fscanf(input, "%d %d", &n, &m);
	init_instance(inst, n, m);
	init_solution(S, n,m);
	for(int j=0; j<n; j++)
		for(int i=0; i<m; i++)
			fscanf(input,"%d", &inst.c[i][j]);

	for(int j=0; j<n; j++){
		for(int i=0; i<m; i++){
			fscanf(input,"%d", &inst.a[i][j]);		
		}
	}

	for(int j=0; j<n; j++)
		fscanf(input, "%d", &inst.b[j]);

/*	DWORD d0 = GetTickCount();	*/
	if(k==1)
		core(inst, S, P, RELAX_1);
	else if(k==2)
		core(inst, S, P, RELAX_2);
	else if(k==3)
		core(inst, S, P, RELAX_3);

	if(!verifySum(inst,S)){
		puts("PRIMAL INVaLIDO SUM");
	}
	if(!verifyPeso(inst,S)){
		puts("PRIMAL INVaIDO PESO");
	}
/*	DWORD d1 = GetTickCount();*/

	/*printf("%s %d %d %d %d %.4lf %d %d %d %d\n", argv[2], k, d1-d0, S.primal,  S.dual, 100.*(S.dual-S.primal)/(double	)S.dual, S.time_primal, 	S.time_dual, S.it_primal, S.it_dual);*/

	fprintf(output, "%.6lf\n",  S.dual);
	fprintf(output, "%d\n",  S.primal);

	for(int i=0; i<S.m; i++){
		bool atribuida=false;
		for(int j=0; j<S.n; j++){
			if(S.HEUR[i][j]==1){
				fprintf(output,"%d ", j);
				atribuida=true;
			}
		}
		
		if(atribuida==false)
			fprintf(output, "-1 ");
	}
	fprintf(output, "\n");
	delete_instance(inst, n, m);
	delete_solution(S, n,m);
	return (0);
}
