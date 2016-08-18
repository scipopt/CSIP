typedef struct csip_model CSIP_MODEL;

/* return codes */
typedef int CSIP_RETCODE;
#define CSIP_RETCODE_OK 0
#define CSIP_RETCODE_ERROR 1
#define CSIP_RETCODE_NOMEMORY 2

/* solver status */
typedef int CSIP_STATUS;
#define CSIP_STATUS_OPTIMAL 0
#define CSIP_STATUS_INFEASIBLE 1
#define CSIP_STATUS_UNBOUNDED 2
#define CSIP_STATUS_INFORUNBD 3
#define CSIP_STATUS_NODELIMIT 4
#define CSIP_STATUS_TIMELIMIT 5
#define CSIP_STATUS_MEMLIMIT 6
#define CSIP_STATUS_USERLIMIT 7
#define CSIP_STATUS_UNKNOWN 8

/* variable types */
typedef int CSIP_VARTYPE;
#define CSIP_VARTYPE_BINARY 0
#define CSIP_VARTYPE_INTEGER 1
#define CSIP_VARTYPE_IMPLINT 2
#define CSIP_VARTYPE_CONTINUOUS 3

/* model definition */

// Create a new model (and solver).
CSIP_RETCODE CSIPcreateModel(CSIP_MODEL **model);

// Free all memory of model (and solver).
CSIP_RETCODE CSIPfreeModel(CSIP_MODEL *model);

// Add new variable to model.
// To omit a bound, use (-)INFINITY.
// The variable index will be assigned to idx; pass NULL if not needed.
CSIP_RETCODE CSIPaddVar(
    CSIP_MODEL *model, double lowerbound, double upperbound,
    CSIP_VARTYPE vartype, int *idx);

// Set new lower bounds for a set of variables.
CSIP_RETCODE CSIPchgVarLB(
    CSIP_MODEL *model, int numindices, int *indices, double *lowerbounds);

// Set new lower bounds for a set of variables.
CSIP_RETCODE CSIPchgVarUB(
    CSIP_MODEL *model, int numindices, int *indices, double *upperbounds);

// Set new type for a variable.
CSIP_RETCODE CSIPchgVarType(
    CSIP_MODEL *model, int varindex, CSIP_VARTYPE vartype);

// Get type of a variable.
CSIP_VARTYPE CSIPgetVarType(CSIP_MODEL *model, int varindex);

// Add new linear constraint to the model, of the form:
//    lhs <= sum_i coefs[i] * vars[i] <= rhs
// For one-sided inequalities, use (-)INFINITY for lhs or rhs.
// The constraint index will be assigned to idx; pass NULL if not needed.
CSIP_RETCODE CSIPaddLinCons(
    CSIP_MODEL *model, int numindices, int *indices, double *coefs,
    double lhs, double rhs, int *idx);

// Add new quadratic constraint to the model, of the form:
//    lhs <= sum_i lincoefs[i] * vars[lin[i]]
//           + sum_j quadcoefs[j] * vars[row[j]] * vars[col[j]] <= rhs
// For one-sided inequalities, use (-)INFINITY for lhs or rhs.
// The constraint index will be assigned to idx; pass NULL if not needed.
CSIP_RETCODE CSIPaddQuadCons(
    CSIP_MODEL *model, int numlinindices, int *linindices, double *lincoefs,
    int numquadterms, int *quadrowindices, int *quadcolindices,
    double *quadcoefs, double lhs, double rhs, int *idx);

// Add SOS1 (special ordered set of type 1) constraint on a set of
// variables. That is, at most one variable is allowed to take on a
// nonzero value.
// Use weights to determine variable order, or NULL.
// The constraint index will be assigned to idx; pass NULL if not needed.
CSIP_RETCODE CSIPaddSOS1(
    CSIP_MODEL *model, int numindices, int *indices, double *weights, int *idx);

// Add SOS2 (special ordered set of type 2) constraint on a set of
// variables. That is, at most two consecutive variables are allowed
// to take on nonzero values.
// Use weights to determine variable order, or NULL.
// The constraint index will be assigned to idx; pass NULL if not needed.
CSIP_RETCODE CSIPaddSOS2(
    CSIP_MODEL *model, int numindices, int *indices, double *weights, int *idx);

// Set the linear objective function of the form: sum_i coefs[i] * vars[i]
CSIP_RETCODE CSIPsetObj(
    CSIP_MODEL *model, int numindices, int *indices, double *coefs);

// Set the optimization sense to minimization. This is the default setting.
CSIP_RETCODE CSIPsetSenseMinimize(CSIP_MODEL *model);

// Set the optimization sense to maximization.
CSIP_RETCODE CSIPsetSenseMaximize(CSIP_MODEL *model);

// Solve the model.
CSIP_RETCODE CSIPsolve(CSIP_MODEL *model);

// Copy the values of all variables in the best known solution into
// the output array. The user is responsible for memory allocation.
CSIP_RETCODE CSIPgetVarValues(CSIP_MODEL *model, double *output);

// Get the objective value of the best-known solution.
double CSIPgetObjValue(CSIP_MODEL *model);

// Get the best known bound on the optimal solution
double CSIPgetObjBound(CSIP_MODEL *model);

// Get the solving status.
CSIP_STATUS CSIPgetStatus(CSIP_MODEL *model);

// Set value for parameter (of any type).
CSIP_RETCODE CSIPsetParameterGeneric(
    CSIP_MODEL *model, const char *name, void *value);

// convenience macro to avoid cast to void*
#define CSIPsetParameter(model, name, value) \
    CSIPsetParameterGeneric((model), (name), (void*)(value))

// Get the number of variables added to the model.
int CSIPgetNumVars(CSIP_MODEL *model);

// Supply a solution (as a dense array) to be checked at the beginning of the
// solving process. Only complete solutions are supported.
CSIP_RETCODE CSIPsetInitialSolution(CSIP_MODEL *model, double *values);

/* lazy constraint callback functions */

typedef struct SCIP_ConshdlrData CSIP_CBDATA;

// Copy values of current (relaxation) solution to output array. Call
// this function from your lazy constraint callback.
CSIP_RETCODE CSIPcbGetVarValues(CSIP_CBDATA *cbdata, double *output);

// Add a linear constraint from a lazy constraint callback.
// With islocal, you specify whether the added constraint is only
// valid locally (in the branch-and-bound subtree).
CSIP_RETCODE CSIPcbAddLinCons(
    CSIP_CBDATA *cbdata, int numindices, int *indices, double *coefs,
    double lhs, double rhs, int islocal);

typedef CSIP_RETCODE(*CSIP_LAZYCALLBACK)(
    CSIP_MODEL *model, CSIP_CBDATA *cbdata, void *userdata);

// Add a lazy constraint callback to the model.
// With fractional == 0, the callback is only called for solution
// candidates that satisfy all integrality conditions.
// You may use userdata to pass any data.
CSIP_RETCODE CSIPaddLazyCallback(
    CSIP_MODEL *model, CSIP_LAZYCALLBACK cb, int fractional, void *userdata);

/* heuristic callback functions */

typedef struct SCIP_HeurData CSIP_HEURDATA;

// signature for heuristic callbacks.
// must only call `CSIPheur*` methods from within callback, passing `heurdata`.
typedef CSIP_RETCODE(*CSIP_HEURCALLBACK)(
    CSIP_MODEL *model, CSIP_HEURDATA *cbdata, void *userdata);

// Copy values of solution to output array. Call this function from your
// heuristic callback. Solution is LP relaxation of current node.
CSIP_RETCODE CSIPheurGetVarValues(CSIP_HEURDATA *heurdata, double *output);

// Supply a solution (as a dense array). Only complete solutions are supported.
CSIP_RETCODE CSIPheurSetSolution(CSIP_HEURDATA *heurdata, double *values);

// Add a heuristic callback to the model.
// You may use userdata to pass any data.
CSIP_RETCODE CSIPaddHeuristicCallback(
    CSIP_MODEL *model, CSIP_HEURCALLBACK heur, void *userdata);

/* advanced usage */

// Get access to the internal SCIP solver. Use at your own risk!
void *CSIPgetInternalSCIP(CSIP_MODEL *model);
