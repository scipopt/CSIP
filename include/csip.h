
typedef struct model CSIP_MODEL;
typedef int CSIP_RETCODE;

/* return codes */
#define CSIP_RETCODE_OK 0
#define CSIP_RETCODE_FAIL 1

CSIP_RETCODE CSIPcreateModel(CSIP_MODEL**);
CSIP_RETCODE CSIPfreeModel(CSIP_MODEL*);

/* variable types */
#define CSIP_VARTYPE_BINARY 1
#define CSIP_VARTYPE_INTEGER 2
#define CSIP_VARTYPE_IMPLINT 3
#define CSIP_VARTYPE_CONTINUOUS 4


int CSIPaddVar(CSIP_MODEL*, double lowerbound, double upperbound, int vartype);
CSIP_RETCODE CSIPchgVarLB(CSIP_MODEL*, int numindices, int *indices, double *coefs);
CSIP_RETCODE CSIPchgVarUB(CSIP_MODEL*, int numindices, int *indices, double *coefs);

int CSIPaddLinCons(CSIP_MODEL*, int numindices, int *indices, double *coefs, double lhs, double rhs);

CSIP_RETCODE CSIPaddQuadCons(CSIP_MODEL*, int numlinindices, int *linindices,
                        double *lincoefs, int numquadterms,
                        int *quadrowindices, int *quadcolindices,
                        double *quadcoefs, double lhs, double rhs);

CSIP_RETCODE CSIPaddSOS1(CSIP_MODEL*, int numindices, int *indices, double *weights);

CSIP_RETCODE CSIPaddSOS2(CSIP_MODEL*, int numindices, int *indices, double *weights);

CSIP_RETCODE CSIPsetObj(CSIP_MODEL*, int numindices, int *indices, double *coefs);

CSIP_RETCODE CSIPsolve(CSIP_MODEL*);

CSIP_RETCODE CSIPgetVarValues(CSIP_MODEL*, double *output);
double CSIPgetObjValue(CSIP_MODEL*);

CSIP_RETCODE CSIPsetIntParam(CSIP_MODEL*, const char *name, int value);
CSIP_RETCODE CSIPsetDoubleParam(CSIP_MODEL*, const char *name, double value);
CSIP_RETCODE CSIPsetBoolParam(CSIP_MODEL*, const char *name, int value);
CSIP_RETCODE CSIPsetStringParam(CSIP_MODEL*, const char *name, const char *value);
CSIP_RETCODE CSIPsetLongIntParam(CSIP_MODEL*, const char *name, long long value);
CSIP_RETCODE CSIPsetCharParam(CSIP_MODEL*, const char *name, char value);

int CSIPgetNumVars(CSIP_MODEL*);

void *CSIPgetInternalSCIP(CSIP_MODEL*);

/* callback functions */
typedef struct cbdata CSIP_CBDATA;
CSIP_RETCODE CSIPcbGetVarValues(CSIP_CBDATA*, double *output);
CSIP_RETCODE CSIPcbAddLinCons(CSIP_CBDATA*, int numindices, int *indices, double *coefs, double lhs, double rhs, int islocal);

typedef CSIP_RETCODE (*CSIP_LAZYCALLBACK)(CSIP_MODEL*, CSIP_CBDATA*, void *userdata);

CSIP_RETCODE CSIPaddLazyCallback(CSIP_MODEL*, CSIP_LAZYCALLBACK, int fractional, void *userdata);
