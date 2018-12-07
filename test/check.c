#include "common.h"
#include "str.h"
#include "file.h"
#include "compile.h"
#include <stdlib.h>
#include <string.h>
#include <check.h>

START_TEST (test_asprintfa)
{
    char *str = strdup("foo");
    int n = asprintfa(&str, "%dbar", 123);

    ck_assert_int_eq(n, 6);
    ck_assert_str_eq(str, "foo123bar");

    free(str);
}
END_TEST

START_TEST (test_asprintfa_op_num_fmt)
{
    char *str = strdup("foo");
    int n = asprintfa(&str, "%2$s%1$s", "baz", "bar");

    ck_assert_int_eq(n, 6);
    ck_assert_str_eq(str, "foobarbaz");

    free(str);
}
END_TEST

START_TEST (test_asprintfa_null)
{
    char *str = NULL;
    int n = asprintfa(&str, "%s", "foo");

    ck_assert_int_eq(n, 3);
    ck_assert_str_eq(str, "foo");

    free(str);
}
END_TEST

START_TEST (test_file_fname_null)
{
    char *fname = gen_fname("whatever", NULL);
    ck_assert_str_eq(fname, "a.out");

    free(fname);
}
END_TEST

START_TEST (test_file_fname_ext)
{
    char *fname = gen_fname("file", ".ext");
    char *fname_bf = gen_fname("file.bf", ".ext");
    char *fname_path = gen_fname("/path/to/file", ".ext");
    char *fname_path_bf = gen_fname("/path/to/file.bf", ".ext");

    ck_assert_str_eq(fname, "file.ext");
    ck_assert_str_eq(fname_bf, "file.ext");
    ck_assert_str_eq(fname_path, "file.ext");
    ck_assert_str_eq(fname_path_bf, "file.ext");

    free(fname);
    free(fname_bf);
    free(fname_path);
    free(fname_path_bf);
}
END_TEST

static void mkopt(struct options *opt, int optimize)
{
    opt->assemble=0;
    opt->link=0;
    opt->memsize=30000;
    opt->symbol="f";
    opt->outfile=NULL;
    opt->infile=NULL;
    opt->optimize=optimize;
}

START_TEST (test_compile_null)
{
    struct options opt;
    mkopt(&opt, 0);
    char *out_srcnull = compile(NULL, &opt);
    char *out_optnull = compile("", NULL);

    ck_assert_ptr_eq(out_srcnull, NULL);
    ck_assert_ptr_eq(out_optnull, NULL);
}
END_TEST

START_TEST (test_compile_valid)
{
    struct options opt;
    mkopt(&opt, 0);
    char *out = compile("a+a-a,a.a[a]a<a>a", &opt);
    char *out_trimmed = compile("+-,.[]<>", &opt);

    ck_assert_ptr_ne(out, NULL);
    ck_assert_str_eq(out, out_trimmed);

    free(out);
    free(out_trimmed);
}
END_TEST

START_TEST (test_compile_valid_optimize)
{
    struct options opt;
    mkopt(&opt, 1);
    char *out = compile(">>>,<<<+>->+<-,-", &opt);

    ck_assert_ptr_ne(out, NULL);

    free(out);
}
END_TEST

START_TEST (test_compile_unmatching_loops)
{
    struct options opt;
    mkopt(&opt, 0);
    char *out = compile("][", &opt);

    ck_assert_ptr_eq(out, NULL);
}
END_TEST

START_TEST (test_compile_empty)
{
    struct options opt;
    mkopt(&opt, 0);
    char *out_empty = compile("", &opt);
    char *out_onlycomments = compile("foo", &opt);

    ck_assert_ptr_ne(out_empty, NULL);
    ck_assert_ptr_ne(out_onlycomments, NULL);
    ck_assert_str_eq(out_empty, out_onlycomments);

    free(out_empty);
    free(out_onlycomments);
}
END_TEST


Suite *bfc_suite(void)
{
    TCase *tc_str = tcase_create("str");
    tcase_add_test(tc_str, test_asprintfa);
    tcase_add_test(tc_str, test_asprintfa_op_num_fmt);
    tcase_add_test(tc_str, test_asprintfa_null);

    TCase *tc_file = tcase_create("file");
    tcase_add_test(tc_file, test_file_fname_null);
    tcase_add_test(tc_file, test_file_fname_ext);

    TCase *tc_compile = tcase_create("compile");
    tcase_add_test(tc_compile, test_compile_null);
    tcase_add_test(tc_compile, test_compile_valid);
    tcase_add_test(tc_compile, test_compile_valid_optimize);
    tcase_add_test(tc_compile, test_compile_unmatching_loops);
    tcase_add_test(tc_compile, test_compile_empty);

    Suite *s = suite_create("bfc");
    suite_add_tcase(s, tc_str);
    suite_add_tcase(s, tc_file);
    suite_add_tcase(s, tc_compile);

    return s;
}

int main(void)
{
    SRunner *sr = srunner_create(bfc_suite());

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
