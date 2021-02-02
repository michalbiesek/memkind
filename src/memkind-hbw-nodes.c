// SPDX-License-Identifier: BSD-2-Clause
/* Copyright (C) 2016 - 2021 Intel Corporation. */

#include <memkind/internal/memkind_hbw.h>

#include <stdio.h>

#define MAX_ARG_LEN 8

const char *help_message =
    "\n"
    "NAME\n"
    "    memkind-hbw-nodes - Prints comma-separated list of high bandwidth nodes.\n"
    "\n"
    "SYNOPSIS\n"
    "    memkind-hbw-nodes -h | --help\n"
    "        Prints this help message.\n"
    "\n"
    "DESCRIPTION\n"
    "    Prints a comma-separated list of high bandwidth NUMA nodes\n"
    "    that can be used with the numactl --membind option.\n"
    "\n"
    "EXIT STATUS\n"
    "    Return code is :\n"
    "        0 on success\n"
    "        1 on failure\n"
    "        2 on invalid argument\n"
    "\n"
    "COPYRIGHT\n"
    "    Copyright 2016 - 2021 Intel Corporation All Rights Reserved.\n"
    "\n"
    "AUTHORS\n"
    "    Krzysztof Kulakowski\n"
    "\n"
    "SEE ALSO\n"
    "    hbwmalloc(3), memkind(3)\n"
    "\n";

int print_hbw_nodes()
{
    int i, j = 0;
    int *nodes;
    int nodes_no;

    //WARNING: code below is usage of memkind experimental API which may be changed in future
    if(memkind_hbw_get_all_nodes_nodemask(nodes, &nodes_no) != 0) {
        return 1;
    }

    for(i=0; i<nodes_no; i++) {
        printf("%d%s", nodes[i], (i==nodes_no) ? "" : ",");
    }
    free(nodes);

    printf("\n");
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc == 1) {
        return print_hbw_nodes();
    } else if ((argc == 2) && (strncmp(argv[1], "-h", MAX_ARG_LEN) == 0 ||
                               strncmp(argv[1], "--help", MAX_ARG_LEN) == 0)) {
        printf("%s", help_message);
        return 2;
    }

    printf("ERROR: Unknown option %s. More info with \"%s --help\".\n", argv[1],
           argv[0]);
    return 2;
}
