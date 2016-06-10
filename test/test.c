#include <csip.h>
#include <math.h>



void test_lp() {

    /*
      Small LP:
      min -x
      s.t. 2x + y <= 1.5
      x,y >= 0
      solution is (0.75,0) with objval -0.75
    */
    int numindices = 2;
    const int *indices = {0,1};
    const double *objcoef = {-1.0, 0.0};
    const double *conscoef = {2.0, 1.0};
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

    CSIP_RETCODE status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_OPTIMAL);

    double objval = CSIPgetObjValue(m);

    assert(fabs(objval - (-0.75)) <= 1e-5);

    status = CSIPgetVarValues(m, solution);

    assert(fabs(solution[0] - 0.75) <= 1e-5);
    assert(fabs(solution[1] - 0.0) <= 1e-5);

    CSIPfreemodel(m);

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
    const int *indices = {0,1,2,3,4};
    const double *objcoef = {-5.0, -3.0, -2.0, -7.0, -4.0};
    const double *conscoef = {2.0, 8.0, 4.0, 2.0, 5.0};
    double solution[5];
    CSIP_MODEL *m;
    CSIP_RETCODE status;

    status = CSIPcreateModel(&m);
    assert(status == CSIP_RETCODE_OK);

    int var_idx;
    for (int i = 0; i < 5; i++) {
        status = CSIPaddVar(m, -INFINITY, INFINITY, CSIP_VARTYPE_BINARY, &var_idx);
        assert(var_idx == i);
    }

    status = CSIPsetObj(m, numindices, indices, objcoef);
    assert(status == CSIP_RETCODE_OK);
    int cons_idx;
    status = CSIPaddLinCons(m, numindices, indices, conscoef, -INFINITY, 10.0);
    assert(status == CSIP_RETCODE_OK);
    assert(cons_idx == 0);

    CSIP_RETCODE status = CSIPsolve(m);
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

    CSIPfreemodel(m);
}

void test_mip2() {

    /*
      Small unbounded MIP:
      min  x
      x Integer
    */
    int numindices = 1;
    const int *indices = {0};
    const double *objcoef = {1.0};
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

    CSIP_RETCODE status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_UNBOUNDED);

    CSIPfreemodel(m);
}

void test_mip3() {

    /*
      Small infeasible MIP:
      min  x
      x >= 2
      x <= 1
    */
    int numindices = 1;
    const int *indices = {0};
    const double *objcoef = {1.0};
    const double *conscoef = {1.0};
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

    CSIP_RETCODE status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_INFEASIBLE);

    CSIPfreemodel(m);
}

void test_socp() {
    /*
      min t
      s.t. x + y >= 1
           x^2 + y^2 <= t^2
           t >= 0
     */

    const int *objindices = {0};
    const double *objcoef = {1.0};
    const int *linindices = {1,2};
    const int *lincoef = {1.0,1.0};
    const int *quadi = {0,1,2};
    const int *quadj = {0,1,2};
    const int *quadcoef = {-1.0,1.0,1.0};
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

    CSIP_RETCODE status = CSIPsolve(m);
    assert(status == CSIP_RETCODE_OK);

    int solvestatus = CSIPgetStatus(m);
    assert(solvestatus == CSIP_STATUS_OPTIMAL);

    double objval = CSIPgetObjValue(m);

    assert(fabs(objval - (sqrt(0.5))) <= 1e-5);

    status = CSIPgetVarValues(m, solution);

    assert(fabs(solution[0] - sqrt(0.5)) <= 1e-5);
    assert(fabs(solution[1] - 0.5) <= 1e-5);
    assert(fabs(solution[2] - 0.5) <= 1e-5);

    CSIPfreemodel(m);
}

void test_lazy() {



}
