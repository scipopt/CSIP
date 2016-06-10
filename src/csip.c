#include "csip.h"
#include "scip/scip.h"
#include "scip/pub_misc.h"

#define CSIP_CALL(x)  do{                               \
                           CSIP_RETCODE _retcode;       \
                           _retcode = (x);              \
                           if( _retcode != SCIP_OKAY )  \
                           {                            \
                              return CSIP_RETCODE_FAIL; \
                           }                            \
                        }while(0)

#define TODO 1000

struct csip_model
{
   SCIP* scip;
   int nvars;
   SCIP_VAR* vars[TODO];
   int nconss;
   SCIP_CONS* conss[TODO];
};

/*
 * local methods
 */
//static
//CSIP_RETCODE addPtrarrayVal(SCIP* scip, SCIP_PTRARRAY* ptrarray, void* val)
//{
//}

static
CSIP_RETCODE createLinCons(CSIP_MODEL* model, int numindices, int *indices, double *coefs, double lhs, double rhs, SCIP_CONS** cons)
{
   SCIP* scip;
   SCIP_VAR* var;
   int i;

   scip = model->scip;

   CSIP_CALL( SCIPcreateConsBasicLinear(scip, cons, "lincons", 0, NULL, NULL, lhs, rhs) );

   for( i = 0; i < numindices; ++i )
   {
      //var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      var = model->vars[indices[i]];
      CSIP_CALL( SCIPaddCoefLinear(scip, *cons, var, coefs[i]) );
   }

   return CSIP_RETCODE_OK;
}

static
CSIP_RETCODE addCons(CSIP_MODEL* model, SCIP_CONS* cons, int* idx)
{
   SCIP* scip;

   scip = model->scip;

   CSIP_CALL( SCIPaddCons(scip, cons) );

   //CSIP_CALL( addPtrarrayVal(scip, model->conss, (void *)cons, idx) );
   *idx = model->nconss;
   model->conss[*idx] = cons;
   ++(model->nconss);

   return CSIP_RETCODE_OK;
}

/*
 * interface methods
 */
CSIP_RETCODE CSIPcreateModel(CSIP_MODEL** modelptr)
{
   CSIP_RETCODE retcode;
   CSIP_MODEL* model;

   *modelptr = (CSIP_MODEL *)malloc(sizeof(CSIP_MODEL));
   if( *modelptr == NULL )
      return CSIP_RETCODE_FAIL;

   model = *modelptr;

   CSIP_CALL( SCIPcreate(&model->scip) );
   //CSIP_CALL( SCIPcreatePtrarray(model->scip, &model->vars) );
   //CSIP_CALL( SCIPcreatePtrarray(model->scip, &model->conss) );

   model->nvars = 0;
   model->nconss = 0;

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPfreeModel(CSIP_MODEL* model)
{
   CSIP_RETCODE retcode;

   //CSIP_CALL( SCIPfreePtrarray(model->scip, &model->conss) );
   //CSIP_CALL( SCIPfreePtrarray(model->scip, &model->vars) );
   CSIP_CALL( SCIPfree(&model->scip) );

   free(model);

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPaddVar(CSIP_MODEL* model, double lowerbound, double upperbound, int vartype, int* idx)
{
   SCIP* scip;
   SCIP_VAR* var;

   scip = model->scip;

   CSIP_CALL( SCIPcreateVarBasic(scip, &var, NULL, lowerbound, upperbound, 0.0, vartype) );

   //CSIP_CALL( addPtrarrayVal(scip, model->vars, (void *)var, idx) );
   *idx = model->nvars;
   model->vars[*idx] = var;

   ++(model->nvars);

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPchgVarLB(CSIP_MODEL* model, int numindices, int *indices, double *lowerbounds)
{
   int i;
   SCIP* scip;
   SCIP_VAR* var;

   scip = model->scip;

   for( i = 0; i < numindices; ++i )
   {
      //var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      var = model->vars[indices[i]];
      CSIP_CALL( SCIPchgVarLb(scip, var, lowerbounds[i]) );
   }
}

CSIP_RETCODE CSIPchgVarUB(CSIP_MODEL* model, int numindices, int *indices, double *upperbounds)
{
   int i;
   SCIP* scip;
   SCIP_VAR* var;

   scip = model->scip;

   for( i = 0; i < numindices; ++i )
   {
      //var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      var = model->vars[indices[i]];
      CSIP_CALL( SCIPchgVarUb(scip, var, upperbounds[i]) );
   }

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPaddLinCons(CSIP_MODEL* model, int numindices, int *indices, double *coefs, double lhs, double rhs, int* idx)
{
   SCIP_CONS* cons;

   CSIP_CALL( createLinCons(model, numindices, indices, coefs, lhs, rhs, &cons) );

   CSIP_CALL( addCons(model, cons, idx) );

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPaddQuadCons(CSIP_MODEL* model, int numlinindices, int *linindices,
                        double *lincoefs, int numquadterms,
                        int *quadrowindices, int *quadcolindices,
                        double *quadcoefs, double lhs, double rhs, int* idx)
{
   int i;
   SCIP* scip;
   SCIP_VAR* linvar;
   SCIP_VAR* var1;
   SCIP_VAR* var2;
   SCIP_CONS* cons;

   scip = model->scip;

   CSIP_CALL( SCIPcreateConsBasicQuadratic(scip, &cons, "quadcons", 0, NULL, NULL, 0, NULL, NULL, NULL, lhs, rhs) );

   for( i = 0; i < numlinindices; ++i )
   {
      //linvar = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, linindices[i]);
      linvar = model->vars[linindices[i]];
      CSIP_CALL( SCIPaddLinearVarQuadratic(scip, cons, linvar, lincoefs[i]) );
   }

   for( i = 0; i < numquadterms; ++i )
   {
      //var1 = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, quadrowindices[i]);
      var1 = model->vars[quadrowindices[i]];
      //var2 = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, quadcolindices[i]);
      var2 = model->vars[quadcolindices[i]];

      CSIP_CALL( SCIPaddBilinTermQuadratic(scip, cons, var1, var2, quadcoefs[i]) );
   }

   CSIP_CALL( addCons(model, cons, idx) );

   return CSIP_RETCODE_OK;
}

/* TODO */
CSIP_RETCODE CSIPaddSOS1(CSIP_MODEL* model, int numindices, int *indices, double *weights, int* idx)
{
   return CSIP_RETCODE_OK;
}

/* TODO */
CSIP_RETCODE CSIPaddSOS2(CSIP_MODEL* model, int numindices, int *indices, double *weights, int* idx)
{
   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPsetObj(CSIP_MODEL* model, int numindices, int *indices, double *coefs)
{
   int i;
   SCIP* scip;
   SCIP_VAR* var;

   scip = model->scip;

   for( i = 0; i < numindices; ++i )
   {
      //var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      var = model->vars[indices[i]];
      CSIP_CALL( SCIPchgVarObj(scip, var, coefs[i]) );
   }

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPsolve(CSIP_MODEL* model)
{
   CSIP_CALL( SCIPsolve(model->scip) );

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPgetVarValues(CSIP_MODEL* model, double *output)
{
   int i;
   SCIP* scip;
   SCIP_VAR* var;

   scip = model->scip;

   if( SCIPgetBestSol(scip) == NULL )
      return CSIP_RETCODE_FAIL;

   for( i = 0; i < model->nvars; ++i )
   {
      //var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      var = model->vars[i];
      output[i] = SCIPgetSolVal(scip, SCIPgetBestSol(scip), var);
   }

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPsetIntParam(CSIP_MODEL* model, const char *name, int value)
{
   return CSIP_RETCODE_OK;
}
CSIP_RETCODE CSIPsetDoubleParam(CSIP_MODEL* model, const char *name, double value)
{
   return CSIP_RETCODE_OK;
}
CSIP_RETCODE CSIPsetBoolParam(CSIP_MODEL* model, const char *name, int value)
{
   return CSIP_RETCODE_OK;
}
CSIP_RETCODE CSIPsetStringParam(CSIP_MODEL* model, const char *name, const char *value)
{
   return CSIP_RETCODE_OK;
}
CSIP_RETCODE CSIPsetLongIntParam(CSIP_MODEL* model, const char *name, long long value)
{
   return CSIP_RETCODE_OK;
}
CSIP_RETCODE CSIPsetCharParam(CSIP_MODEL* model, const char *name, char value)
{
   return CSIP_RETCODE_OK;
}

int CSIPgetNumVars(CSIP_MODEL* model)
{
   return model->nvars;
}

void *CSIPgetInternalSCIP(CSIP_MODEL* model)
{
   return model->scip;
}


/*
 * Constraint Handler
 */

/* constraint handler data */
struct SCIP_ConshdlrData
{
   CSIP_MODEL* model;
   CSIP_LAZYCALLBACK callback;
   void* userdata;
   SCIP_Bool checkonly;
   SCIP_Bool feasible;
   SCIP_SOL* sol;

   //CSIP_CBDATA* cbdata;
};



SCIP_DECL_CONSENFOLP(consEnfolpLazy)
{
   int nconsadded;
   SCIP_CONSHDLRDATA* conshdlrdata;

   *result = SCIP_FEASIBLE;

   conshdlrdata = SCIPconshdlrGetData(conshdlr);
   conshdlrdata->checkonly = FALSE;
   conshdlrdata->feasible = TRUE;

   CSIP_CALL( conshdlrdata->callback(conshdlrdata->model, conshdlrdata, conshdlrdata->userdata) );

   if( !conshdlrdata->feasible )
      *result = SCIP_CONSADDED;

   return CSIP_RETCODE_OK;
}

/* enfo pseudo solution just call enfo lp solution */
SCIP_DECL_CONSENFOPS(consEnfopsLazy)
{
   CSIP_CALL( consEnfolpLazy(scip, conshdlr, conss, nconss, nusefulconss, solinfeasible, result) );
   return CSIP_RETCODE_OK;
}

/* check callback */
SCIP_DECL_CONSCHECK(consCheckLazy)
{
   SCIP_CONSHDLRDATA* conshdlrdata;

   *result = SCIP_FEASIBLE;

   conshdlrdata = SCIPconshdlrGetData(conshdlr);
   conshdlrdata->checkonly = TRUE;
   conshdlrdata->feasible = TRUE;
   conshdlrdata->sol = sol;

   CSIP_CALL( conshdlrdata->callback(conshdlrdata->model, conshdlrdata, conshdlrdata->userdata) );

   if( !conshdlrdata->feasible )
      *result = SCIP_INFEASIBLE;

   return CSIP_RETCODE_OK;
}

/* locks callback */
SCIP_DECL_CONSLOCK(consLockLazy)
{
   int i;
   SCIP_VAR* var;
   SCIP_CONSHDLRDATA* conshdlrdata;

   assert(scip == conshdlrdata->model->scip);

   for( i = 0; i < conshdlrdata->model->nvars; ++i )
   {
      //var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      var = conshdlrdata->model->vars[i];
      CSIP_CALL( SCIPaddVarLocks(scip, var, nlockspos + nlocksneg, nlockspos + nlocksneg) );
   }

   return CSIP_RETCODE_OK;
}

/*
 * callback methods
 */

CSIP_RETCODE CSIPaddLazyCallback(CSIP_MODEL* model, CSIP_LAZYCALLBACK callback, int fractional, void *userdata)
{
   SCIP_CONSHDLRDATA* conshdlrdata;
   SCIP_CONSHDLR* conshdlr;
   SCIP* scip;
   int priority;
   char name[SCIP_MAXSTRLEN];

   scip = model->scip;

   /* it is -1 or 1 because cons_integral has priority 0 */
   priority = fractional ? -1 : 1;

   SCIP_CALL( SCIPallocMemory(scip, &conshdlrdata) );

   conshdlrdata->model = model;
   conshdlrdata->callback = callback;
   conshdlrdata->userdata = userdata;

   //SCIPsnprintf(name, SCIP_MAXSTRLEN, "lazycons_%d", model->nlazyconss);
   SCIPsnprintf(name, SCIP_MAXSTRLEN, "lazycons_");
   SCIP_CALL( SCIPincludeConshdlrBasic(scip, &conshdlr, name, "lazy constraint callback",
         priority, -1, -1, FALSE,
         consEnfolpLazy, consEnfopsLazy, consCheckLazy, consLockLazy,
         conshdlrdata) );

   return CSIP_RETCODE_OK;
}

/* returns LP or given solution depending whether we are called from check or enfo */
CSIP_RETCODE CSIPcbGetVarValues(CSIP_CBDATA* cbdata, double *output)
{
   int i;
   SCIP* scip;
   SCIP_VAR* var;
   SCIP_SOL* sol;

   scip = cbdata->model->scip;

   if( cbdata->checkonly )
      sol = cbdata->sol;
   else
      sol = NULL;

   for( i = 0; i < cbdata->model->nvars; ++i )
   {
      //var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      var = cbdata->model->vars[i];
      output[i] = SCIPgetSolVal(scip, sol, var);
   }

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPcbAddLinCons(CSIP_CBDATA* cbdata, int numindices, int *indices, double *coefs, double lhs, double rhs, int islocal)
{
   SCIP* scip;
   SCIP_CONS* cons;
   SCIP_SOL* sol;
   SCIP_RESULT result;

   scip = cbdata->model->scip;

   if( cbdata->checkonly )
      sol = cbdata->sol;
   else
      sol = NULL;

   CSIP_CALL( createLinCons(cbdata->model, numindices, indices, coefs, lhs, rhs, &cons) );

   SCIP_CALL( SCIPcheckCons(scip, cons, sol, FALSE, FALSE, FALSE, &result) );

   if( result == SCIP_INFEASIBLE )
   {
      cbdata->feasible = FALSE;
   }

   if( !cbdata->checkonly )
   {
      CSIP_CALL( addLinCons(cbdata->model, cons) );
   }

   return CSIP_RETCODE_OK;
}

