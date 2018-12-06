#define _GNU_SOURCE

#include "common.h"
#include "file.h"
#include "compile.h"
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
    MEM_SIZE = CHAR_MAX + 1
};

static struct option const long_opts[] = {
    {"mem-size", required_argument, NULL, MEM_SIZE},
    {"symbol", required_argument, NULL, 's'},
    {NULL, 0, NULL, 0}
};

int handle_args(int argc, char *argv[], struct options *opt)
{
    int c;
    while ((c = getopt_long(argc, argv, "cs:So:", long_opts, NULL)) != -1)
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

        case 's':
            if (strcmp(optarg, "_start") == 0)
                error(EXIT_FAILURE, 0,
                        "symbol '_start' is reserved, use 'main' instead");
            opt->symbol = strdup(optarg);
            break;

        case '?':
            return 0;

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
        __builtin_unreachable(); /* stop gcc complaining about fallthrough */
    default:
        error(EXIT_FAILURE, 0, "too many arguments");
    }

    return 1;
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
    };

    handle_args(argc, argv, &opt);

    struct stat stbuf;
    if (stat(opt.infile, &stbuf) == -1)
        error(EXIT_FAILURE, errno, "%s", opt.infile);

    if (S_ISDIR(stbuf.st_mode))
        error(EXIT_FAILURE, EISDIR, "%s", opt.infile);

    char *src;
    if ((src = read_file(opt.infile)) == NULL)
        error(EXIT_FAILURE, errno, "%s", opt.infile);

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

    char *out = compile(src, &opt);
    if (out == NULL)
        error(EXIT_FAILURE, 0, "cannot parse input");

    fputs(out, out_stream);
    fclose(out_stream);

    free(src);
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
