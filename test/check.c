#include "common.h"
#include "str.h"
#include "file.h"
#include "compile.h"
#include "arch.h"
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

static void mkopt(struct options *opt, int optimize)
{
    opt->assemble=0;
    opt->link=0;
    opt->memsize=30000;
    opt->symbol="f";
    opt->outfile=NULL;
    opt->infile=NULL;
    opt->optimize=optimize;
    opt->arch=NULL;
    opt->set_fmts=NULL;
    opt->native=0;
}

START_TEST (test_file_fname_ext)
{
    struct options opt;
    mkopt(&opt, 0);
    opt.infile = "file";
    opt.assemble = 0;
    char *fname = gen_fname(&opt);

    opt.infile = "file.bf";
    char *fname_bf = gen_fname(&opt);

    opt.infile = "/path/to/file";
    char *fname_path = gen_fname(&opt);

    opt.infile = "/path/to/file.bf";
    char *fname_path_bf = gen_fname(&opt);

    opt.infile = "file.b";
    char *fname_b = gen_fname(&opt);

    opt.infile = "file.";
    char *fname_dot = gen_fname(&opt);

    opt.infile = ".bf";
    char *fname_dotfile = gen_fname(&opt);

    opt.infile = "file.a";
    char *fname_other = gen_fname(&opt);

    opt.assemble = 1;
    opt.link = 1;
    char *aout = gen_fname(&opt);

    ck_assert_str_eq(fname, "file.s");
    ck_assert_str_eq(fname_bf, "file.s");
    ck_assert_str_eq(fname_path, "file.s");
    ck_assert_str_eq(fname_path_bf, "file.s");
    ck_assert_str_eq(fname_b, "file.s");
    ck_assert_str_eq(fname_dot, "file..s");
    ck_assert_str_eq(fname_dotfile, ".bf.s");
    ck_assert_str_eq(fname_other, "file.a.s");
    ck_assert_str_eq(aout, "a.out");

    free(fname);
    free(fname_bf);
    free(fname_path);
    free(fname_path_bf);
    free(fname_b);
    free(fname_dot);
    free(fname_dotfile);
    free(fname_other);
    free(aout);
}
END_TEST

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
    set_arch(&opt);
    char *out = compile("a+a-a,a.a[a]a<a>a", &opt);
    char *out_trimmed = compile("+-,.[]<>", &opt);

    ck_assert_ptr_ne(out, NULL);
    ck_assert_str_eq(out, out_trimmed);

    free(out);
    free(out_trimmed);
    free(opt.arch);
}
END_TEST

START_TEST (test_compile_valid_optimize)
{
    struct options opt;
    mkopt(&opt, 1);
    set_arch(&opt);
    char *out = compile(">>>,<<<+>->+<-,-", &opt);

    ck_assert_ptr_ne(out, NULL);

    free(out);
    free(opt.arch);
}
END_TEST

START_TEST (test_compile_unmatching_loops)
{
    struct options opt;
    mkopt(&opt, 0);
    set_arch(&opt);
    char *out = compile("][", &opt);

    ck_assert_ptr_eq(out, NULL);

    free(opt.arch);
}
END_TEST

START_TEST (test_compile_empty)
{
    struct options opt;
    mkopt(&opt, 0);
    set_arch(&opt);
    char *out_empty = compile("", &opt);
    char *out_onlycomments = compile("foo", &opt);

    ck_assert_ptr_ne(out_empty, NULL);
    ck_assert_ptr_ne(out_onlycomments, NULL);
    ck_assert_str_eq(out_empty, out_onlycomments);

    free(out_empty);
    free(out_onlycomments);
    free(opt.arch);
}
END_TEST

START_TEST (test_arch_native)
{
    struct options opt;
    mkopt(&opt, 0);
    opt.arch = NULL;

    int ret = set_arch(&opt);

    ck_assert_int_eq(ret, 0);
    ck_assert_int_eq(opt.native, 1);
    ck_assert_ptr_ne(opt.arch, NULL);

    free(opt.arch);
}
END_TEST

START_TEST (test_arch_unsupported)
{
    struct options opt;
    mkopt(&opt, 0);
    opt.arch = "doesntexist";

    int ret = set_arch(&opt);

    ck_assert_int_eq(ret, -1);
}
END_TEST

Suite *bfc_suite(void)
{
    TCase *tc_str = tcase_create("str");
    tcase_add_test(tc_str, test_asprintfa);
    tcase_add_test(tc_str, test_asprintfa_op_num_fmt);
    tcase_add_test(tc_str, test_asprintfa_null);

    TCase *tc_file = tcase_create("file");
    tcase_add_test(tc_file, test_file_fname_ext);

    TCase *tc_compile = tcase_create("compile");
    tcase_add_test(tc_compile, test_compile_null);
    tcase_add_test(tc_compile, test_compile_valid);
    tcase_add_test(tc_compile, test_compile_valid_optimize);
    tcase_add_test(tc_compile, test_compile_unmatching_loops);
    tcase_add_test(tc_compile, test_compile_empty);

    TCase *tc_arch = tcase_create("arch");
    tcase_add_test(tc_file, test_arch_native);
    tcase_add_test(tc_file, test_arch_unsupported);

    Suite *s = suite_create("bfc");
    suite_add_tcase(s, tc_str);
    suite_add_tcase(s, tc_file);
    suite_add_tcase(s, tc_compile);
    suite_add_tcase(s, tc_arch);

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
