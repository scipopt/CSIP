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

struct
{
   SCIP* scip;
   int nvars;
   SCIP_PTRARRAY* vars;
   int nconss;
   SCIP_PTRARRAY* conss;
} model;

/*
 * local methods
 */
static
CSIP_RETCODE addPtrarrayVal(SCIP* scip, SCIP_PTRARRAY* ptrarray, void* val)
{
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
   CSIP_CALL( SCIPcreatePtrarray(model->scip, &model->vars) );
   CSIP_CALL( SCIPcreatePtrarray(model->scip, &model->conss) );

   model->nvars = 0;
   model->nconss = 0;

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPfreeModel(CSIP_MODEL* model)
{
   CSIP_RETCODE retcode;
   CSIP_MODEL* model;

   CSIP_CALL( SCIPfreePtrarray(model->scip, &model->conss) );
   CSIP_CALL( SCIPfreePtrarray(model->scip, &model->vars) );
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
   CSIP_CALL( addPtrarrayVal(scip, model->vars, (void *)var, idx) );

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
      var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
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
      var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      CSIP_CALL( SCIPchgVarUb(scip, var, upperbounds[i]) );
   }

   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPaddLinCons(CSIP_MODEL* model, int numindices, int *indices, double *coefs, double lhs, double rhs, int* idx)
{
   int i;
   SCIP* scip;
   SCIP_VAR* var;
   SCIP_CONS* cons;

   scip = model->scip;

   CSIP_CALL( SCIPcreateConsBasicLinear(scip, &cons, "lincons", 0, NULL, NULL, lhs, rhs) );

   for( i = 0; i < numindices; ++i )
   {
      var = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, indices[i]);
      CSIP_CALL( SCIPaddCoefLinear(scip, cons, var, coefs[i]) );
   }

   CSIP_CALL( SCIPaddCons(scip, cons) );

   CSIP_CALL( addPtrarrayVal(scip, model->conss, (void *)cons, idx) );

   ++(model->nconss);

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
      linvar = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, linindices[i]);
      CSIP_CALL( SCIPaddLinearVarQuadratic(scip, cons, linvar, lincoefs[i]) );
   }

   for( i = 0; i < numquadterms; ++i )
   {
      var1 = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, quadrowindices[i]);
      var2 = (SCIP_VAR *)SCIPgetPtrarrayVal(scip, model->vars, quadcolindices[i]);

      CSIP_CALL( SCIPaddBilinTermQuadratic(scip, cons, var1, var2, quadcoefs[j]) );
   }

   CSIP_CALL( SCIPaddCons(scip, cons) );

   CSIP_CALL( addPtrarrayVal(scip, model->conss, (void *)cons, idx) );

   ++(model->nconss);

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
   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPsolve(CSIP_MODEL* model)
{
   return CSIP_RETCODE_OK;
}

CSIP_RETCODE CSIPgetVarValues(CSIP_MODEL* model, double *output)
{
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

/* callback functions */
typedef struct cbdata CSIP_CBDATA;
CSIP_RETCODE CSIPcbGetVarValues(CSIP_CBDATA*, double *output)
{
   return CSIP_RETCODE_OK;
}
CSIP_RETCODE CSIPcbAddLinCons(CSIP_CBDATA*, int numindices, int *indices, double *coefs, double lhs, double rhs, int islocal)
{
   return CSIP_RETCODE_OK;
}

typedef CSIP_RETCODE (*CSIP_LAZYCALLBACK)(CSIP_MODEL* model, CSIP_CBDATA*, void *userdata)

CSIP_RETCODE CSIPaddLazyCallback(CSIP_MODEL* model, CSIP_LAZYCALLBACK, int fractional, void *userdata)
{
   return CSIP_RETCODE_OK;
}
