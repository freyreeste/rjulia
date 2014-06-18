/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "Julia_R.h"
#include "R_Julia.h"
#define pkgdebug
static int jlrunning=0;
SEXP Julia_is_running()
{
  SEXP ans;
  PROTECT(ans = allocVector(LGLSXP, 1));
  LOGICAL(ans)[0]=jlrunning;
  UNPROTECT(1);
  return ans; 
}

SEXP initJulia(SEXP julia_home,SEXP DisableGC)
{
  if (jl_is_initialized())
    return R_NilValue;
  char *s = CHAR(STRING_ELT(julia_home, 0));
  if (strlen(s) == 0)
    jl_init(NULL);
  else
    jl_init(s);
  JL_SET_STACK_BASE;
  jlrunning=1;
  if (jl_exception_occurred())
  {
    error("Julia not initialized");
  }
  if (LOGICAL(DisableGC))
    jl_gc_disable();
  return R_NilValue;
}

//Convert Julia Type To R
SEXP Julia_R(jl_value_t* Var)
{  
  SEXP ans;
  JL_GC_PUSH1(&Var);
  ans=R_NilValue;
  //Array To Vector
  if (jl_is_array(Var))
  {
   ans=Julia_R_MD(Var); 
   return ans;
 }
//Value to Vector
 ans=Julia_R_Scalar(Var);
 JL_GC_POP();
 return ans;
}
//Convert R Type To Julia
SEXP R_Julia(SEXP Var,SEXP VarNam)
{
  int n;
  jl_value_t* ret;
  char *VarName = CHAR(STRING_ELT(VarNam, 0));
  R_Julia_MD(Var,ret,VarName);
  return R_NilValue;
}
//eval but not return val
SEXP jl_void_eval(SEXP cmd)
{
  char *s = CHAR(STRING_ELT(cmd, 0));
  jl_value_t* ret = jl_eval_string(s);
  return R_NilValue;
}
//eval julia script and retrun
SEXP jl_eval(SEXP cmd)
{
  char *s = CHAR(STRING_ELT(cmd, 0));
  jl_value_t* ret= jl_eval_string(s);
  return Julia_R(ret);
}
