#include "common.h"
#include "compile.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

#define PROGRAM_NAME "bfc"

static struct options opt = {
    .assemble = 1,
    .link = 1,
    .memsize = 30000,
    .symbol = "main",
    .outfile = NULL,
    .infile = NULL,
};

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

char *gen_fname(const char* path, const char* ext)
{
    if (ext == NULL)
        return strdup("a.out");

    const char *bf_ext = ".bf";
    size_t bf_ext_len = strlen(bf_ext);
    size_t ext_len = strlen(ext);

    const char *base = strrchr(path, '/');
    base = (base != NULL ? base+1 : path);
    size_t base_len = strlen(base);

    /* reserve buffer big enough for added extension */
    char *fname = (char *) malloc(base_len + ext_len);
    strcpy(fname, base);

    /* remove .bf extension if there is one */
    if (base_len > bf_ext_len &&
            strcmp(fname + base_len - bf_ext_len, bf_ext) == 0)
        fname[base_len - bf_ext_len] = 0;

    return strcat(fname, ext);
}

int main(int argc, char *argv[])
{
    FILE *in_stream, *out_stream;

    int c;
    while ((c = getopt_long(argc, argv, "cs:So:", long_opts, NULL)) != -1)
    {
        switch (c)
        {
        case 'c':
            opt.link = 0;
            break;

        case 'S':
            opt.assemble = 0;
            break;

        case 'o':
            opt.outfile = strdup(optarg);
            break;

        case MEM_SIZE:
            opt.memsize = atoi(optarg);
            break;

        case 's':
            if (strcmp(optarg, "_start") == 0)
            {
                fprintf(stderr, "%s: %s\n", PROGRAM_NAME,
                        "symbol '_start' is reserved, use 'main' instead");
                return EXIT_FAILURE;
            }
            opt.symbol = strdup(optarg);
            break;

        case '?':
            return EXIT_FAILURE;

        default:
            abort();
        }
    }

    switch (argc-optind)
    {
    case 1:
        opt.infile = strdup(argv[optind]);
        break;
    case 0:
        fprintf(stderr, "%s: %s\n", PROGRAM_NAME, "no input file");
        return EXIT_FAILURE;
    default:
        fprintf(stderr, "%s: %s\n", PROGRAM_NAME, "too many arguments");
        return EXIT_FAILURE;
    }

    struct stat stbuf;
    if (stat(opt.infile, &stbuf) == -1)
    {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, opt.infile,
                strerror(errno));
        return EXIT_FAILURE;
    }
    if (S_ISDIR(stbuf.st_mode))
    {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, opt.infile,
                strerror(EISDIR));
        return EXIT_FAILURE;
    }

    in_stream = fopen(opt.infile, "r");
    if (in_stream == NULL)
    {
        fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, opt.infile,
                strerror(errno));
        return EXIT_FAILURE;
    }

    if (opt.outfile == NULL)
    {
        if (!opt.assemble)
            opt.outfile = gen_fname(opt.infile, ".s");
        else if (!opt.link)
            opt.outfile = gen_fname(opt.infile, ".o");
        else
            opt.outfile = gen_fname(opt.infile, NULL);
    }

    if (!opt.assemble)
    {
        out_stream = fopen(opt.outfile, "w");
        if (out_stream == NULL)
        {
            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, opt.outfile,
                    strerror(errno));
            return EXIT_FAILURE;
        }
    }
    else
    {
        int fd[2];
        if (pipe(fd) == -1)
        {
            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "pipe",
                    strerror(errno));
            return EXIT_FAILURE;
        }

        out_stream = fdopen(fd[1], "w");
        if (out_stream == NULL)
        {
            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "pipe",
                    strerror(errno));
            return EXIT_FAILURE;
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "fork",
                    strerror(errno));
            return EXIT_FAILURE;
        }
        if (pid == 0)
        {
            if (dup2(fd[0], STDIN_FILENO) == -1)
            {
                fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "dup2",
                        strerror(errno));
                return EXIT_FAILURE;
            }

            close(fd[0]);
            close(fd[1]);

            char *gcc_argv[8] = {
                "gcc", "-x", "assembler", "-", "-o", opt.outfile, NULL, NULL
            };
            if (!opt.link)
                gcc_argv[6] = "-c";

            execvp("gcc", gcc_argv);

            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, "gcc",
                    strerror(errno));
            return EXIT_FAILURE;
        }

        close(fd[0]);
    }

    if (!compile(in_stream, out_stream, &opt))
    {
        fprintf(stderr, "%s: cannot parse input\n", PROGRAM_NAME);
        return EXIT_FAILURE;
    }

    fclose(in_stream);
    fclose(out_stream);

    if (opt.assemble)
    {
        int status;
        wait(&status);
        status = WEXITSTATUS(status);

        if (status != 0)
        {
            fprintf(stderr, "%s: gcc returned %d exit status\n",
                    PROGRAM_NAME, status);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
