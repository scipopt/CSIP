#include <stdlib.h>
#include <assert.h>
#include <csip.h>
#include <math.h>

#define const  


void test_lp() {

    /*
      Small LP:
      min -x
      s.t. 2x + y <= 1.5
      x,y >= 0
      solution is (0.75,0) with objval -0.75
    */
    int numindices = 2;
    const int indices[] = {0,1};
    const double objcoef[] = {-1.0, 0.0};
    const double conscoef[] = {2.0, 1.0};
    double solution[2];
    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    int x_idx, y_idx;

    status = CSIPaddVar(m, 0.0, INFINITY, CSIP_VARTYPE_CONTINUOUS, &x_idx);
    assert(status == CSIP_RETCODE_OK);
    status = CSIPaddVar(m, 0.0, INFINITY, CSIP_VARTYPE_CONTINUOUS, &y_idx);
    assert(status == CSIP_RETCODE_OK);

    assert(x_idx == 0);
    assert(y_idx == 1);

    status = CSIPsetObj(m, numindices, indices, objcoef);
    assert(status == CSIP_RETCODE_OK);
    int cons_idx;
    status = CSIPaddLinCons(m, numindices, indices, conscoef, -INFINITY, 1.5, &cons_idx);
    assert(status == CSIP_RETCODE_OK);
    assert(cons_idx == 0);

    status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_OPTIMAL);

    double objval = CSIPgetObjValue(m);

    assert(fabs(objval - (-0.75)) <= 1e-5);

    status = CSIPgetVarValues(m, solution);

    assert(fabs(solution[0] - 0.75) <= 1e-5);
    assert(fabs(solution[1] - 0.0) <= 1e-5);

    CSIPfreeModel(m);

}

void test_mip() {

    /*
      Small MIP:
      min -5x_1 - 3x_2 - 2x_3 - 7x_4 - 4x_5
      s.t. 2x_1 + 8x_2 + 4x_3 + 2x_4 + 5x_5 <= 10
      x Bin
      solution is (1,0,0,1,1) with objval -16
    */
    int numindices = 5;
    const int indices[] = {0,1,2,3,4};
    const double objcoef[] = {-5.0, -3.0, -2.0, -7.0, -4.0};
    const double conscoef[] = {2.0, 8.0, 4.0, 2.0, 5.0};
    double solution[5];
    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    int var_idx;
    for (int i = 0; i < 5; i++) {
        //status = CSIPaddVar(m, -INFINITY, INFINITY, CSIP_VARTYPE_BINARY, &var_idx);
        status = CSIPaddVar(m, 0.0, 1.0, CSIP_VARTYPE_BINARY, &var_idx);
        assert(var_idx == i);
    }

    status = CSIPsetObj(m, numindices, indices, objcoef);
    assert(status == CSIP_RETCODE_OK);
    int cons_idx;
    status = CSIPaddLinCons(m, numindices, indices, conscoef, -INFINITY, 10.0, &cons_idx);
    assert(status == CSIP_RETCODE_OK);
    assert(cons_idx == 0);

    status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_OPTIMAL);

    double objval = CSIPgetObjValue(m);

    assert(fabs(objval - (-16.0)) <= 1e-5);

    status = CSIPgetVarValues(m, solution);

    assert(fabs(solution[0] - 1.0) <= 1e-5);
    assert(fabs(solution[1] - 0.0) <= 1e-5);
    assert(fabs(solution[2] - 0.0) <= 1e-5);
    assert(fabs(solution[3] - 1.0) <= 1e-5);
    assert(fabs(solution[4] - 1.0) <= 1e-5);

    CSIPfreeModel(m);
}

void test_mip2() {

    /*
      Small unbounded MIP:
      min  x
      x Integer
    */
    int numindices = 1;
    const int indices[] = {0};
    const double objcoef[] = {1.0};
    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    int x_idx;
    status = CSIPaddVar(m, -INFINITY, INFINITY, CSIP_VARTYPE_INTEGER, &x_idx);
    assert(status == CSIP_RETCODE_OK);
    assert(x_idx == 0);

    status = CSIPsetObj(m, numindices, indices, objcoef);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    //assert(solvestatus == CSIP_STATUS_UNBOUNDED);

    CSIPfreeModel(m);
}

void test_mip3() {

    /*
      Small infeasible MIP:
      min  x
      x >= 2
      x <= 1
    */
    int numindices = 1;
    const int     indices[] = {0};
    const double  objcoef[] = {1.0};
    const double conscoef[] = {1.0};
    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPaddVar(m, -INFINITY, INFINITY, CSIP_VARTYPE_INTEGER, NULL);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsetObj(m, numindices, indices, objcoef);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPaddLinCons(m, numindices, indices, conscoef, -INFINITY, 1.0, NULL);
    assert(status == CSIP_RETCODE_OK);
    status = CSIPaddLinCons(m, numindices, indices, conscoef, 2.0, INFINITY, NULL);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    //assert(solvestatus == CSIP_STATUS_INFEASIBLE);

    CSIPfreeModel(m);
}

void test_socp() {
    /*
      min t
      s.t. x + y >= 1
           x^2 + y^2 <= t^2
           t >= 0
     */

    const int objindices[] = {0};
    const double objcoef[] = {1.0};
    const int linindices[] = {1,2};
    const double lincoef[] = {1.0,1.0};
    const int quadi[] = {0,1,2};
    const int quadj[] = {0,1,2};
    const double quadcoef[] = {-1.0,1.0,1.0};
    double solution[3];

    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    // t
    status = CSIPaddVar(m, 0.0, INFINITY, CSIP_VARTYPE_CONTINUOUS, NULL);
    assert(status == CSIP_RETCODE_OK);

    // x
    status = CSIPaddVar(m, -INFINITY, INFINITY, CSIP_VARTYPE_CONTINUOUS, NULL);
    assert(status == CSIP_RETCODE_OK);

    // y
    status = CSIPaddVar(m, -INFINITY, INFINITY, CSIP_VARTYPE_CONTINUOUS, NULL);
    assert(status == CSIP_RETCODE_OK);

    int numvars = CSIPgetNumVars(m);
    assert(numvars == 3);

    // sparse objective
    status = CSIPsetObj(m, 1, objindices, objcoef);
    assert(status == CSIP_RETCODE_OK);

    // sparse constraint
    status = CSIPaddLinCons(m, 2, linindices, lincoef, 1.0, INFINITY, NULL);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPaddQuadCons(m, 0, NULL, NULL, 3, quadi, quadj, quadcoef, -INFINITY, 0.0, NULL);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_OPTIMAL);

    double objval = CSIPgetObjValue(m);

    assert(fabs(objval - (sqrt(0.5))) <= 1e-5);

    status = CSIPgetVarValues(m, solution);

    assert(fabs(solution[0] - sqrt(0.5)) <= 1e-5);
    //assert(fabs(solution[1] - 0.5) <= 1e-5);
    //assert(fabs(solution[2] - 0.5) <= 1e-5);
    assert(fabs(solution[1] - 0.5) <= sqrt(1e-5));
    assert(fabs(solution[2] - 0.5) <= sqrt(1e-5));

    CSIPfreeModel(m);
}

struct MyData {
    int foo;
    double *storage;
};

CSIP_RETCODE lazy_callback(CSIP_MODEL *m, CSIP_CBDATA *cb, void *userdata) {

    struct MyData *data = (struct MyData*) userdata;
    assert(data->foo == 10);
    const int indices[] = {0,1};
    const double coef[] = {1.0,1.0};

    CSIPcbGetVarValues(cb, data->storage);

    // enforce x + y <= 3, global cut
    if (data->storage[0] + data->storage[1] > 3) {
        CSIPcbAddLinCons(cb, 2, indices, coef, -INFINITY, 3.0, 0);
    }

    return CSIP_RETCODE_OK;
}

/* the problem written originally was:
 *    min 0.5x + y
 *    s.t. -inf <= x,y <= 2
 *          x + y <= 3 (lazy)
 * which is unbounded, and for some scip-bug reason, it asserted some stuff
 * I am changing the problem to reflect Miles original vision
 */
void test_lazy() {

    /*
       max 0.5x + y
       s.t. 0 <= x,y <= 2
            x + y <= 3 (lazy)
       solution is (1,2)
     */

    const int objindices[] = {0,1};
    const double objcoef[] = {0.5,1.0};
    double solution[2];

    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    // x
    status = CSIPaddVar(m, 0.0, 2.0, CSIP_VARTYPE_INTEGER, NULL);
    assert(status == CSIP_RETCODE_OK);

    // y
    status = CSIPaddVar(m, 0.0, 2.0, CSIP_VARTYPE_INTEGER, NULL);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsetObj(m, 2, objindices, objcoef);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsetSenseMaximize(m);
    assert(status == CSIP_RETCODE_OK);

    struct MyData userdata = { 10, &solution[0] };

    status = CSIPaddLazyCallback(m, lazy_callback, 1, &userdata);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_OPTIMAL);

    double objval = CSIPgetObjValue(m);

    assert(fabs(objval - 2.5) <= 1e-5);

    status = CSIPgetVarValues(m, solution);

    assert(fabs(solution[0] - 1.0) <= 1e-5);
    assert(fabs(solution[1] - 2.0) <= 1e-5);

    CSIPfreeModel(m);
}

CSIP_RETCODE lazy_callback2(CSIP_MODEL *m, CSIP_CBDATA *cb, void *userdata) {

    struct MyData *data = (struct MyData*) userdata;
    assert(data->foo == 10);
    const int indices[] = {0};
    const double coef[] = {1.0};

    CSIPcbGetVarValues(cb, data->storage);
    // make sure we didn't get a fractional solution
    assert(data->storage[0] - round(data->storage[0]) < 1e-4);

    // always add the cut x <= 10
    CSIPcbAddLinCons(cb, 1, indices, coef, -INFINITY, 10.5, 0);

    return CSIP_RETCODE_OK;
}

void test_lazy2() {

    /*
       min -x
       s.t. x <= 100.5, integer
            x <= 10.5 (lazy)
       solution is -10
     */

    const int objindices[] = {0};
    const double objcoef[] = {-1.0};
    double solution[1];

    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    // x
    status = CSIPaddVar(m, -INFINITY, 100.5, CSIP_VARTYPE_INTEGER, NULL);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsetObj(m, 1, objindices, objcoef);
    assert(status == CSIP_RETCODE_OK);

    struct MyData userdata = { 10, &solution[0] };

    // test fractional = 0
    status = CSIPaddLazyCallback(m, lazy_callback2, 0, &userdata);
    assert(status == CSIP_RETCODE_OK);

    status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_OPTIMAL);

    double objval = CSIPgetObjValue(m);

    assert(fabs(objval - (-10)) <= 1e-5);

    status = CSIPgetVarValues(m, solution);

    assert(fabs(solution[0] - 10.0) <= 1e-5);

    CSIPfreeModel(m);
}

void test_objsense() {
    // min/max  x
    // st.      lb <= x <= ub

    CSIP_MODEL *m;
    CSIP_RETCODE rc;
    const int objindices[] = {0};
    const double objcoef[] = {1.0};
    const double lb = -2.3;
    const double ub =  4.2;
    double solution[1];

    rc = CSIPcreateModel(&m);
    assert(rc == CSIP_RETCODE_OK);

    rc = CSIPaddVar(m, lb, ub, CSIP_VARTYPE_CONTINUOUS, NULL);
    assert(rc == CSIP_RETCODE_OK);

    rc = CSIPsetObj(m, 1, objindices, objcoef);
    assert(rc == CSIP_RETCODE_OK);

    // default sense is 'minimize'
    rc = CSIPsolve(m);
    assert(rc == CSIP_RETCODE_OK);
    assert(CSIPgetStatus(m) == CSIP_STATUS_OPTIMAL);
    assert(fabs(CSIPgetObjValue(m) - lb) <= 1e-5);

    // change sense to 'maximize'
    rc = CSIPsetSenseMaximize(m);
    assert(rc == CSIP_RETCODE_OK);
    rc = CSIPsolve(m);
    assert(rc == CSIP_RETCODE_OK);
    assert(CSIPgetStatus(m) == CSIP_STATUS_OPTIMAL);
    assert(fabs(CSIPgetObjValue(m) - ub) <= 1e-5);

    // change sense to 'minimize'
    rc = CSIPsetSenseMinimize(m);
    assert(rc == CSIP_RETCODE_OK);
    rc = CSIPsolve(m);
    assert(rc == CSIP_RETCODE_OK);
    assert(CSIPgetStatus(m) == CSIP_STATUS_OPTIMAL);
    assert(fabs(CSIPgetObjValue(m) - lb) <= 1e-5);

    CSIPfreeModel(m);
}

int main() {

    // run all the tests
    test_lp();
    test_mip();
    test_mip2();
    test_mip3();
    test_socp();
    test_lazy();
    test_lazy2();
    test_objsense();

    return 0;
}
