// use MinUnit from http://www.jera.com/techinfo/jtns/jtn002.html
// but change it to use failing assert, instead of just returning the message

#define TOL 1e-5

#define mu_assert(message, test) do {        \
    if (!(test))                             \
    {                                        \
        printf("    failed: %s\n", message); \
        assert(test);                        \
    }                                        \
} while (0)

#define mu_assert_near(message, test, expect)          \
    mu_assert(message, fabs((test) - (expect) <= TOL))

#define mu_run_test(test) do { \
    printf("  %s\n", #test);   \
    test();                    \
} while (0)

// with our additions
#define CHECK(x) mu_assert("CSIP error!", (x) == CSIP_RETCODE_OK)
