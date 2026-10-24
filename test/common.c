#include <assert.h>

#include <template/common.h>

static void test_join_direct_tokens(void) {
    assert(JOIN(1, 3) == 13);

    int variable = 12345;
    assert(JOIN(vari, able) == 12345);
}

static void test_join_macro_arguments(void) {
  #define A 1
  #define B 3
    assert(JOIN(A, B) == 13);
  #undef B
  #undef A

  #define PART1 vari
  #define PART2 able
    int variable = 12345;
    assert(JOIN(PART1, PART2) == 12345);
  #undef PART2
  #undef PART1
}

static void test_args_num(void) {
    assert(ARGS_NUM(0) == 1);
    assert(ARGS_NUM(0, 0) == 2);
    assert(ARGS_NUM(0, 0, 0) == 3);
    assert(ARGS_NUM(0, 0, 0, 0) == 4);
    assert(ARGS_NUM(0, 0, 0, 0, 0) == 5);
    assert(ARGS_NUM(0, 0, 0, 0, 0, 0) == 6);
    assert(ARGS_NUM(0, 0, 0, 0, 0, 0, 0) == 7);
    assert(ARGS_NUM(0, 0, 0, 0, 0, 0, 0, 0) == 8);
}

static void test_arg(void) {
    int a = 11, b = 22, c = 33;

    assert(ARG(1, a) == a);
    assert(ARG(1, a, b) == a);
    assert(ARG(1, a, b, c) == a);

    assert(ARG(2, a, b) == b);
    assert(ARG(2, a, b, c) == b);

    assert(ARG(3, a, b, c) == c);
}

int main(void) {
    test_join_direct_tokens();
    test_join_macro_arguments();
    test_args_num();
    test_arg();

    return 0;
}
