#ifndef _AUXILIAR_H_
#define _AUXILIAR_H_

#include "base.h"

bool verifySum(Instance &I, Solution &S);
bool verifyPeso(Instance &I, Solution &S);
void init_instance(Instance &inst, int n, int m);
void read_param(Parameter &P, int k);
void init_solution(Solution &sol, int n, int m);
void delete_instance(Instance &inst, int n, int m);
void delete_solution(Solution &sol, int n, int m);
#endif
