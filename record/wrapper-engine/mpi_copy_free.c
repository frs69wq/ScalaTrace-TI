/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Mon July 1 2002 */
/* mpi_pack.c -- print code to free/copy Umpire ops... */


#include <stdio.h>
#include <string.h>
#include <search.h>

#include "y.tab.h"
#include "wrapper.h"



void
RecordFreeAndCopyOpsStart (char *name, int *needed)
{
    assert (!(*needed));

    /* need to start this function definition and declaration... */
    *needed = 1;

    fprintf (freeHFile, 
             "\nextern void umpi_free_op_%s(umpi_op_t*op);",name);

    /* start free_op definition.. */
    fprintf (freeFile, 
             "void\n"
             "umpi_free_op_%s(umpi_op_t*op)\n{\n", name);

    fprintf (copyHFile, 
             "\nextern void umpi_copy_op_%s(umpi_op_t*op, umpi_op_t*newop);",
             name);

    /* start copy_op definition.. */
    fprintf (copyFile, 
             "void\n"
             "umpi_copy_op_%s(umpi_op_t*op, umpi_op_t*newop)\n{\n", 
             name);
  
    return;
}


void
RecordFreeAndCopyOps(char *name, int *needed)
{
    int i;

    /* haven't needed it yet... */
    *needed = 0;

    for (i = 1; i <= argCounter; i++) {
        const char *arg_tname = NameForType(&args[i].type);

        if ((!(args[i].eNoSave)) && (strlen (args[i].name) > 0)) {
            /* only need this for special cases that */
            /* use heap storage in wrapper... */
            if (strcmp(args[i].name, "array_of_statuses") == 0) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.statuses_srcs != NULL) {\n"
                         "free (op->data.%s.statuses_srcs);\n" 
                         "op->data.%s.statuses_srcs = NULL;\n}\n\n", 
                         nameStr, nameStr, nameStr);

                fprintf (copyFile, 
                         "if (op->data.%s.statuses_srcs != NULL) {\n"
                         "newop->data.%s.statuses_srcs =\n"
                         "(int *) malloc (sizeof(int) * op->data.%s.count);\n"
                         "assert (newop->data.%s.statuses_srcs);\n"
                         "bcopy (op->data.%s.statuses_srcs,\n"
                         "newop->data.%s.statuses_srcs,\n"
                         "sizeof(int) * op->data.%s.count);\n}\n\n", 
                         nameStr, nameStr, nameStr, 
                         nameStr, nameStr, nameStr, nameStr);
            }
            else if (strstr(args[i].name, "array") != NULL) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "free (op->data.%s.%s);\n" 
                         "op->data.%s.%s = NULL;\n}\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (copyFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "newop->data.%s.%s =\n"
                         "(%s *) malloc (sizeof(%s) * op->data.%s.count);\n"
                         "assert (newop->data.%s.%s);\n"
                         "bcopy (op->data.%s.%s,\n"
                         "newop->data.%s.%s,\n"
                         "sizeof(int) * op->data.%s.count);\n}\n\n", 
                         nameStr, args[i].name, nameStr, args[i].name, 
                         arg_tname, arg_tname,
                         nameStr, nameStr, args[i].name, nameStr, 
                         args[i].name, nameStr, args[i].name, nameStr);
            }
            else if ((strstr(args[i].name, "counts") != NULL) ||
                     (strstr(args[i].name, "displs") != NULL) ||
                     (strstr(args[i].name, "retcoords") != NULL) ||
                     (strstr(args[i].name, "dimsp") != NULL)) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "free (op->data.%s.%s);\n" 
                         "op->data.%s.%s = NULL;\n}\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (copyFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "newop->data.%s.%s =\n"
                         "(int *) malloc (sizeof(int) * op->data.%s.size);\n"
                         "assert (newop->data.%s.%s);\n"
                         "bcopy (op->data.%s.%s,\n"
                         "newop->data.%s.%s,\n"
                         "sizeof(int) * op->data.%s.size);\n}\n\n", 
                         nameStr, args[i].name, nameStr, args[i].name, 
                         nameStr, nameStr, args[i].name, nameStr, 
                         args[i].name, nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "retedges") != NULL) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "free (op->data.%s.%s);\n" 
                         "op->data.%s.%s = NULL;\n}\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (copyFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "newop->data.%s.%s =\n"
                         "(int *) malloc (sizeof(int) * op->data.%s.edgecount);\n"
                         "assert (newop->data.%s.%s);\n"
                         "bcopy (op->data.%s.%s,\n"
                         "newop->data.%s.%s,\n"
                         "sizeof(int) * op->data.%s.edgecount);\n}\n\n", 
                         nameStr, args[i].name, nameStr, args[i].name, 
                         nameStr, nameStr, args[i].name, nameStr, 
                         args[i].name, nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "edges") != NULL) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "free (op->data.%s.%s);\n" 
                         "op->data.%s.%s = NULL;\n}\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (copyFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "newop->data.%s.%s =\n"
                         "(int *) malloc (sizeof(int) * "
                         "op->data.%s.array_of_degrees[op->data.%s.count-1]);\n"
                         "assert (newop->data.%s.%s);\n"
                         "bcopy (op->data.%s.%s,\n"
                         "newop->data.%s.%s,\n"
                         "sizeof(int) * op->data.%s.array_of_degrees["
                         "op->data.%s.count-1]);\n}\n\n", 
                         nameStr, args[i].name, nameStr, args[i].name, 
                         nameStr, nameStr, nameStr, args[i].name, nameStr, 
                         args[i].name, nameStr, args[i].name, nameStr, nameStr);
            }
            else if (strstr(args[i].name, "the_addresses") != NULL) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "free (op->data.%s.%s);\n" 
                         "op->data.%s.%s = NULL;\n}\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (copyFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "newop->data.%s.%s =\n"
                         "(MPI_Aint *) malloc (sizeof(MPI_Aint) * op->data.%s.addrcount);\n"
                         "assert (newop->data.%s.%s);\n"
                         "bcopy (op->data.%s.%s,\n"
                         "newop->data.%s.%s,\n"
                         "sizeof(int) * op->data.%s.addrcount);\n}\n\n", 
                         nameStr, args[i].name, nameStr, args[i].name, 
                         nameStr, nameStr, args[i].name, nameStr, 
                         args[i].name, nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "the_datatypes") != NULL) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "free (op->data.%s.%s);\n" 
                         "op->data.%s.%s = NULL;\n}\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (copyFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "newop->data.%s.%s =\n"
                         "(%s *) malloc (sizeof(%s) * op->data.%s.dtypecount);\n"
                         "assert (newop->data.%s.%s);\n"
                         "bcopy (op->data.%s.%s,\n"
                         "newop->data.%s.%s,\n"
                         "sizeof(int) * op->data.%s.dtypecount);\n}\n\n", 
                         nameStr, args[i].name, nameStr, args[i].name, 
                         arg_tname, arg_tname,
                         nameStr, nameStr, args[i].name, nameStr, 
                         args[i].name, nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "ranges") != NULL) {
                if (!(*needed)) {
                    /* need to start this function definition and declaration... */
                    RecordFreeAndCopyOpsStart (name, needed);
                }

                fprintf (freeFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "free (op->data.%s.%s);\n" 
                         "op->data.%s.%s = NULL;\n}\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (copyFile, 
                         "if (op->data.%s.%s != NULL) {\n"
                         "newop->data.%s.%s =\n"
                         "(int *) malloc (sizeof(int) * 3 * op->data.%s.count);\n"
                         "assert (newop->data.%s.%s);\n"
                         "bcopy (op->data.%s.%s,\n"
                         "newop->data.%s.%s,\n"
                         "sizeof(int) * 3 * op->data.%s.count);\n}\n\n", 
                         nameStr, args[i].name, nameStr, args[i].name, 
                         nameStr, nameStr, args[i].name, nameStr, 
                         args[i].name, nameStr, args[i].name, nameStr);
            }
            else {
                /* don't print warning... */
            }
        }
        else if (strlen (args[i].name) > 0) {
            /* don't print warning... */
        }
    }

    if ((AL_Test("comm_constructor")) &&
        (strcmp (name, "MPI_Comm_dup") != 0)) {
        /* copy comm info from HACK locations... */
        if (!(*needed)) {
            /* need to start this function definition and declaration... */
            RecordFreeAndCopyOpsStart (name, needed);
        }

        fprintf (freeFile, 
                 "if (op->data.%s.array_of_indices != NULL) {\n"
                 "free (op->data.%s.array_of_indices);\n" 
                 "op->data.%s.array_of_indices = NULL;\n}\n\n", 
                 nameStr, nameStr, nameStr);

        fprintf (copyFile, 
                 "if (op->data.%s.array_of_indices != NULL) {\n"
                 "newop->data.%s.array_of_indices =\n"
                 "(int *) malloc (sizeof(int) * op->data.%s.size);\n"
                 "assert (newop->data.%s.array_of_indices);\n"
                 "bcopy (op->data.%s.array_of_indices,\n"
                 "newop->data.%s.array_of_indices,\n"
                 "sizeof(int) * op->data.%s.size);\n}\n\n", 
                 nameStr, nameStr, nameStr, nameStr, nameStr, nameStr, nameStr);
    }

    if (*needed) {
        /* finish free_op definition... */
        fprintf (freeFile, 
                 "return;\n"
                 "} /* umpi_free_op_%s */\n\n\n", name);

        /* finish copy_op definition... */
        fprintf (copyFile, 
                 "return;\n"
                 "} /* umpi_copy_op_%s */\n\n\n", name);
    }

    return;
}

  
/* eof */
