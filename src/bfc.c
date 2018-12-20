#define _GNU_SOURCE

#include "common.h"
#include "file.h"
#include "compile.h"
#include "arch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

/* long opts without short option */
enum
{
    MEM_SIZE = CHAR_MAX + 1,
    NO_OPTIMIZE
};

static struct option const long_opts[] = {
    {"mem-size", required_argument, NULL, MEM_SIZE},
    {"no-optimize", no_argument, NULL, NO_OPTIMIZE},
    {"symbol", required_argument, NULL, 's'},
    {"arch", required_argument, NULL, 'a'},
    {NULL, 0, NULL, 0}
};

void handle_args(int argc, char *argv[], struct options *opt)
{
    int c;
    while ((c = getopt_long(argc, argv, "a:co:s:S", long_opts, NULL)) != -1)
    {
        switch (c)
        {
        case 'c':
            opt->link = 0;
            break;

        case 'S':
            opt->assemble = 0;
            break;

        case 'o':
            opt->outfile = strdup(optarg);
            break;

        case MEM_SIZE:
            opt->memsize = atoi(optarg);
            break;

        case NO_OPTIMIZE:
            opt->optimize = 0;
            break;

        case 's':
            if (strcmp(optarg, "_start") == 0)
                error(EXIT_FAILURE, 0,
                        "symbol '_start' is reserved, use 'main' instead");

            opt->symbol = strdup(optarg);
            break;

        case 'a':
            opt->arch = strdup(optarg);
            break;

        case '?':
            error(EXIT_FAILURE, 0, "unrecognized argument");
            __builtin_unreachable();

        default:
            abort();
        }
    }

    switch (argc-optind)
    {
    case 1:
        opt->infile = strdup(argv[optind]);
        break;

    case 0:
        error(EXIT_FAILURE, 0, "no input file");
        __builtin_unreachable();

    default:
        error(EXIT_FAILURE, 0, "too many arguments");
    }
}

int main(int argc, char *argv[])
{
    /* use short name i.e. no directories for error messages */
    program_invocation_name = program_invocation_short_name;

    struct options opt = {
        .assemble = 1,
        .link = 1,
        .memsize = 30000,
        .symbol = "main",
        .outfile = NULL,
        .infile = NULL,
        .optimize = 1,
        .arch = NULL,
        .set_fmts = NULL,
        .native = 0,
    };

    handle_args(argc, argv, &opt);

    if (set_arch(&opt) == -1)
        error(EXIT_FAILURE, 0, "architecture not supported");
    if (opt.native == 0 && opt.assemble == 1)
        error(EXIT_FAILURE, 0, "assembling supported only on native builds");

    struct stat stbuf;
    if (stat(opt.infile, &stbuf) == -1)
        error(EXIT_FAILURE, errno, "%s", opt.infile);

    if (S_ISDIR(stbuf.st_mode))
        error(EXIT_FAILURE, EISDIR, "%s", opt.infile);

    char *src;
    if ((src = read_file(opt.infile)) == NULL)
        error(EXIT_FAILURE, errno, "%s", opt.infile);

    char *out = compile(src, &opt);
    if (out == NULL)
        error(EXIT_FAILURE, 0, "cannot parse input");
    free(src);

    if (opt.outfile == NULL)
    {
        if (!opt.assemble)
            opt.outfile = gen_fname(opt.infile, ".s");
        else if (!opt.link)
            opt.outfile = gen_fname(opt.infile, ".o");
        else
            opt.outfile = gen_fname(opt.infile, NULL);
    }

    FILE *out_stream;
    if (!opt.assemble)
    {
        out_stream = fopen(opt.outfile, "w");
        if (out_stream == NULL)
            error(EXIT_FAILURE, errno, "%s", opt.outfile);
    }
    else
    {
        int fd[2];
        if (pipe(fd) == -1)
            error(EXIT_FAILURE, errno, "pipe");

        out_stream = fdopen(fd[1], "w");
        if (out_stream == NULL)
            error(EXIT_FAILURE, errno, "fdopen");

        pid_t pid = fork();
        if (pid == -1)
            error(EXIT_FAILURE, errno, "fork");

        if (pid == 0)
        {
            if (dup2(fd[0], STDIN_FILENO) == -1)
                error(EXIT_FAILURE, errno, "dup2");

            close(fd[0]);
            close(fd[1]);

            char *gcc_argv[8] = {
                "gcc", "-x", "assembler", "-", "-o", opt.outfile, NULL, NULL
            };
            if (!opt.link)
                gcc_argv[6] = "-c";

            execvp("gcc", gcc_argv);

            error(EXIT_FAILURE, errno, "gcc");
        }

        close(fd[0]);
    }

    fputs(out, out_stream);
    fclose(out_stream);

    free(out);

    if (opt.assemble)
    {
        int status;
        wait(&status);
        status = WEXITSTATUS(status);

        if (status != 0)
            error(EXIT_FAILURE, 0, "gcc returned %d exit status", status);
    }

    return EXIT_SUCCESS;
}
