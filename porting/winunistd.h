#ifndef __WINUNISTD__
#define __WINUNISTD__ 1

#include <stdio.h>
#include <process.h>

struct option {
    const char *name;
    int         has_arg;
    int        *flag;
    int         val;
};

int getopt(int argc, char * const argv[],
           const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;


int getopt_long(int argc, char * const argv[],
           const char *optstring,
           const struct option *longopts, int *longindex);

int getopt_long_only(int argc, char * const argv[],
           const char *optstring,
           const struct option *longopts, int *longindex);

#define getpid _getpid


#endif
