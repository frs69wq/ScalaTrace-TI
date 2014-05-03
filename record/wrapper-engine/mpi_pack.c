/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Mon July 1 2002 */
/* mpi_pack.c -- print code to pack/unpack Umpire ops for messaging... */


#include <stdio.h>
#include <string.h>
#include <search.h>

#include "y.tab.h"
#include "wrapper.h"


static void printUnpackMalloc(int i, const char *typeName) {
    fprintf (unpackFile,  
             "{\n"
             "int temp;\n"
             "bcopy (&buf[index], &(temp), sizeof(int));\n"
             "index += sizeof(int);\n\n" 
             "if (temp) {\n"
             "op->data.%s.%s =\n(%s *) "
             "malloc (sizeof(op->data.%s.%s[0])*temp);\n"
             "assert (op->data.%s.%s);\n"
             "bcopy (&buf[index], op->data.%s.%s, "
             "sizeof(op->data.%s.%s[0])*temp);\n"
             "index += (sizeof(op->data.%s.%s[0])*temp);\n\n"
             "} else {\n"
             "op->data.%s.%s = NULL;\n"
             "}\n}\n\n",
             nameStr, args[i].name, typeName,
             nameStr, args[i].name, 
             nameStr, args[i].name, 
             nameStr, args[i].name, 
             nameStr, args[i].name, 
             nameStr, args[i].name, 
             nameStr, args[i].name);
}


void
RecordPackOps(char *name)
{
    int i;
    char size_buf[4096];

    fprintf (packHFile, "\nextern int umpi_pack_op_size_%s(umpi_op_t*op);",name);
    fprintf (packHFile, "\nextern int umpi_pack_op_%s(umpi_op_t*op,char *buf,int maxlen);",name);

    fprintf (unpackHFile, "\nextern int umpi_unpack_op_%s(umpi_op_t*op,char *buf);",name);

    /* account for header, packed_size and trailer in pack_op_size... */
    sprintf(size_buf,"3*sizeof(int)");

    /* start pack_op definition.. */
    fprintf (packFile, 
             "int umpi_pack_op_%s(umpi_op_t*op,char *buf,int maxlen)\n{\n",
             name);
    fprintf (packFile, 
             "unsigned index;\n"
             "int temp = UMPI_START_PACKED_OP;\n"
             "int packed_size = umpi_pack_op_size_%s(op);\n\n"
             "assert(op);\n"
             "assert(buf);\n"
             "if (maxlen < packed_size)\n"
             "return UMPIERR_INSUFFICIENT_BUF;\n\n", name);

    /* pack header */
    fprintf (packFile, 
             "bcopy (&temp, buf, sizeof(int));\n"
             "index = sizeof(int);\n\n");
    fprintf (packFile, 
             "bcopy (&packed_size, &buf[index], sizeof(int));\n"
             "index += sizeof(int);\n\n");

    /* start unpack_op definition.. */
    fprintf (unpackFile, 
             "int umpi_unpack_op_%s(umpi_op_t*op,char *buf)\n{\n",name);
    fprintf (unpackFile, 
             "int index = 0;\n\n"
             "assert(op);\n"
             "assert(buf);\n");

    /* pack general op info: op, order, rank, pc and seq_num */
    fprintf (packFile, 
             "bcopy (&(op->op), &buf[index], sizeof(op->op));\n"
             "index += sizeof(op->op);\n\n");
    fprintf (packFile, 
             "bcopy (&(op->order), &buf[index], sizeof(op->order));\n"
             "index += sizeof(op->order);\n\n");
    fprintf (packFile, 
             "bcopy (&(op->rank), &buf[index], sizeof(op->rank));\n"
             "index += sizeof(op->rank);\n\n");
    fprintf (packFile, 
             "bcopy (&(op->pc), &buf[index], sizeof(op->pc));\n"
             "index += sizeof(op->pc);\n\n");
    fprintf (packFile, 
             "bcopy (&(op->seq_num), &buf[index], sizeof(op->seq_num));\n"
             "index += sizeof(op->seq_num);\n\n");

    /* account for general op info in pack_op_size... */
    sprintf(size_buf+strlen(size_buf),
            "+sizeof(op->op)+sizeof(op->order)"
            "+sizeof(op->rank)+sizeof(op->pc)+sizeof(op->seq_num)");

    /* pack/unpack params */
    for (i = 1; i <= argCounter; i++) {
        const char *arg_tname = NameForType(&args[i].type);
      
        if ((!(args[i].eNoSave)) && (strlen (args[i].name) > 0)) {
            /* handle "special" cases first */
            if (strcmp(args[i].name, "op_ptr") == 0) {
                fprintf (packFile, 
                         "bcopy (&(op->data.%s.op), &buf[index], "
                         "sizeof(op->data.%s.op));\n"
                         "index += sizeof(op->data.%s.op);\n\n", 
                         nameStr, nameStr, nameStr);

                fprintf (unpackFile, 
                         "bcopy (&buf[index], &(op->data.%s.op), "
                         "sizeof(op->data.%s.op));\n"
                         "index += sizeof(op->data.%s.op);\n\n", 
                         nameStr, nameStr, nameStr);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.op)", nameStr);
            }
            else if (strcmp(args[i].name, "status") == 0) {
                fprintf (packFile, 
                         "bcopy (&(op->data.%s.%s_src), &buf[index], "
                         "sizeof(op->data.%s.%s_src));\n"
                         "index += sizeof(op->data.%s.%s_src);\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (unpackFile, 
                         "bcopy (&buf[index], &(op->data.%s.%s_src), "
                         "sizeof(op->data.%s.%s_src));\n"
                         "index += sizeof(op->data.%s.%s_src);\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf),
                        "+sizeof(op->data.%s.%s_src)", 
                        nameStr, args[i].name);
            }
            else if (strcmp(args[i].name, "array_of_statuses") == 0) {
                fprintf (packFile,
                         "temp = op->data.%s.count;\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.statuses_srcs, &buf[index], "
                         "sizeof(op->data.%s.statuses_srcs[0])*temp);\n"
                         "index += "
                         "(sizeof(op->data.%s.statuses_srcs[0])*temp);\n" 
                         "}\n\n",
                         nameStr, nameStr, nameStr, nameStr);

                fprintf (unpackFile, 
                         "{\n"
                         "int temp;\n"
                         "bcopy (&buf[index], &(temp), sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "op->data.%s.statuses_srcs =\n(int *) "
                         "malloc (sizeof(op->data.%s.statuses_srcs[0])*temp);\n"
                         "assert (op->data.%s.statuses_srcs);\n"
                         "bcopy (&buf[index], op->data.%s.statuses_srcs, "
                         "sizeof(op->data.%s.statuses_srcs[0])*temp);\n"
                         "index += "
                         "(sizeof(op->data.%s.statuses_srcs[0])*temp);\n" 
                         "} else {\n"
                         "op->data.%s.statuses_srcs = NULL;\n"
                         "}\n}\n\n",
                         nameStr, nameStr, nameStr, 
                         nameStr, nameStr, nameStr, nameStr);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.count)"
                        "+(op->data.%s.count*"
                        "sizeof(op->data.%s.statuses_srcs[0]))", 
                        nameStr, nameStr, nameStr);
            }
            else if (strstr(args[i].name, "array") != NULL) {
                fprintf (packFile,
                         "temp = op->data.%s.count;\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.%s, &buf[index], "
                         "sizeof(op->data.%s.%s[0])*temp);\n"
                         "index += (sizeof(op->data.%s.%s[0])*temp);\n" 
                         "}\n\n",
                         nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, nameStr, args[i].name);

                printUnpackMalloc(i, arg_tname);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.count)"
                        "+(op->data.%s.count*sizeof(op->data.%s.%s[0]))", 
                        nameStr, nameStr, nameStr, args[i].name);
            }
            else if ((strstr(args[i].name, "counts") != NULL) ||
                     (strstr(args[i].name, "displs") != NULL) ||
                     (strstr(args[i].name, "retcoords") != NULL) ||
                     (strstr(args[i].name, "dimsp") != NULL)) {
                fprintf (packFile,
                         "temp = op->data.%s.size;\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.%s, &buf[index], "
                         "sizeof(op->data.%s.%s[0])*temp);\n"
                         "index += (sizeof(op->data.%s.%s[0])*temp);\n" 
                         "}\n\n",
                         nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, nameStr, args[i].name);

                printUnpackMalloc(i, arg_tname);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.size)"
                        "+(op->data.%s.size*sizeof(op->data.%s.%s[0]))", 
                        nameStr, nameStr, nameStr, args[i].name);
            }
            else if (strstr(args[i].name, "retedges") != NULL) {
                fprintf (packFile,
                         "temp = op->data.%s.edgecount;\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.%s, &buf[index], "
                         "sizeof(op->data.%s.%s[0])*temp);\n"
                         "index += (sizeof(op->data.%s.%s[0])*temp);\n"
                         "}\n\n",
                         nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, nameStr, args[i].name);


                printUnpackMalloc(i, arg_tname);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.edgecount)"
                        "+(op->data.%s.edgecount*sizeof(op->data.%s.%s[0]))", 
                        nameStr, nameStr, nameStr, args[i].name);
            }
            else if (strstr(args[i].name, "edges") != NULL) {
                fprintf (packFile,
                         "temp = op->data.%s.array_of_degrees["
                         "op->data.%s.count-1];\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.%s, &buf[index], "
                         "sizeof(op->data.%s.%s[0])*temp);\n"
                         "index += (sizeof(op->data.%s.%s[0])*temp);\n" 
                         "}\n\n",
                         nameStr, nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, nameStr, args[i].name);

                printUnpackMalloc(i, arg_tname);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.array_of_degrees[0])"
                        "+(op->data.%s.array_of_degrees[op->data.%s.count-1]"
                        "*sizeof(op->data.%s.%s[0]))", 
                        nameStr, nameStr, nameStr, nameStr, args[i].name);
            }
            else if (strstr(args[i].name, "the_addresses") != NULL) {
                fprintf (packFile,
                         "temp = op->data.%s.addrcount;\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.%s, &buf[index], "
                         "sizeof(op->data.%s.%s[0])*temp);\n"
                         "index += (sizeof(op->data.%s.%s[0])*temp);\n"
                         "}\n\n",
                         nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, nameStr, args[i].name);

                printUnpackMalloc(i, arg_tname);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.addrcount)"
                        "+(op->data.%s.addrcount*sizeof(op->data.%s.%s[0]))", 
                        nameStr, nameStr, nameStr, args[i].name);
            }
            else if (strstr(args[i].name, "the_datatypes") != NULL) {
                fprintf (packFile,
                         "temp = op->data.%s.dtypecount;\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.%s, &buf[index], "
                         "sizeof(op->data.%s.%s[0])*temp);\n"
                         "index += (sizeof(op->data.%s.%s[0])*temp);\n" 
                         "}\n\n",
                         nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, nameStr, args[i].name);

                printUnpackMalloc(i, arg_tname);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.dtypecount)"
                        "+(op->data.%s.dtypecount*sizeof(op->data.%s.%s[0]))", 
                        nameStr, nameStr, nameStr, args[i].name);
            }
            else if (strstr(args[i].name, "ranges") != NULL) {
                fprintf (packFile,
                         "temp = 3 * op->data.%s.count;\n" 
                         "bcopy (&(temp), &buf[index], sizeof(int));\n"
                         "index += sizeof(int);\n\n" 
                         "if (temp) {\n"
                         "bcopy (op->data.%s.%s, &buf[index], "
                         "sizeof(op->data.%s.%s[0])*temp);\n"
                         "index += (sizeof(op->data.%s.%s[0])*temp);\n" 
                         "}\n\n",
                         nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, nameStr, args[i].name);

                printUnpackMalloc(i, arg_tname);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf), 
                        "+sizeof(op->data.%s.count)"
                        "+(3*op->data.%s.count*sizeof(op->data.%s.%s[0]))", 
                        nameStr, nameStr, nameStr, args[i].name);
            }
            else if ((strstr(args[i].name, "buf") != NULL) ||
                     (strcmp(args[i].name, "function") == 0) ||
                     (args[i].type.t_pointers != 0) || 
                     (args[i].type.array[0] == 0)) {
                fprintf (packFile, 
                         "bcopy (&(op->data.%s.%s), &buf[index], "
                         "sizeof(op->data.%s.%s));\n"
                         "index += sizeof(op->data.%s.%s);\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                fprintf (unpackFile, 
                         "bcopy (&buf[index], &(op->data.%s.%s), "
                         "sizeof(op->data.%s.%s));\n"
                         "index += sizeof(op->data.%s.%s);\n\n", 
                         nameStr, args[i].name, 
                         nameStr, args[i].name, 
                         nameStr, args[i].name);

                /* account for in pack_op_size... */
                sprintf(size_buf+strlen(size_buf),
                        "+sizeof(op->data.%s.%s)", 
                        nameStr, args[i].name);
            }
            else {
                /* print warning */
                fprintf (stderr, 
                         "\n\t\tWARNING: Omitting arg %s from packed op", 
                         args[i].name);
            }
        }
        else if (strlen (args[i].name) > 0) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: Omitting arg %s from packed op (NoSave)", 
                     args[i].name);
        }
    }

    /* pack/unpack extrafield scalars */
    for (i = 0; i < extraFieldCounter; i++) {
        fprintf (packFile, 
                 "bcopy (&(op->data.%s.%s), &buf[index], "
                 "sizeof(op->data.%s.%s));\n"
                 "index += sizeof(op->data.%s.%s);\n\n", 
                 nameStr, extraFields[i].name, 
                 nameStr, extraFields[i].name, 
                 nameStr, extraFields[i].name);

        fprintf (unpackFile, 
                 "bcopy (&buf[index], &(op->data.%s.%s), "
                 "sizeof(op->data.%s.%s));\n"
                 "index += sizeof(op->data.%s.%s);\n\n", 
                 nameStr, extraFields[i].name, 
                 nameStr, extraFields[i].name, 
                 nameStr, extraFields[i].name);

        /* account for in pack_op_size... */
        sprintf(size_buf+strlen(size_buf),
                "+sizeof(op->data.%s.%s)", 
                nameStr, extraFields[i].name);
    }

    /* still need to pack wrapper computed data (e.g., typemap) */
#if 0
    /* eventually want to do this so we can send chksums */
    /* however, sometimes the chksums aren't associated with */
    /* the local_op->valchksum when this code expects them to be... */
    /* PERHAPS BECAUSE THE COPY OP DOESN'T HANDLE THEM? */
    if ((AL_Test("persistent_start") || 
         (AL_Test("completion") && !AL_Test("request_free"))) && 
        (AL_Test("multiple_requests"))) {
        /* record chksums... */
        fprintf (packFile,
                 "temp = op->data.%s.count;\n"
                 "if ((temp) && (op->valchksum != NULL)) {\n"
                 "bcopy (&(temp), &buf[index], sizeof(int));\n"
                 "index += sizeof(int);\n\n" 
                 "bcopy (op->valchksum, &buf[index], "
                 "sizeof(op->valchksum[0])*temp);\n"
                 "index += (sizeof(op->valchksum[0])*temp);\n}\n\n", 
                 nameStr);

        fprintf (unpackFile, 
                 "{\n"
                 "int temp;\n"
                 "bcopy (&buf[index], &(temp), sizeof(int));\n"
                 "index += sizeof(int);\n\n" 
                 "if (temp) {\n"
                 "op->valchksum = (int *) malloc (sizeof(int) * temp);\n"
                 "assert (op->valchksum);\n"
                 "bcopy (&buf[index], op->valchksum, "
                 "sizeof(op->valchksum[0])*temp);\n"
                 "index += (sizeof(op->valchksum[0])*temp);\n"
                 "}\n}\n\n",
                 nameStr);

        /* account for chksums in pack_op_size... */
        sprintf(size_buf+strlen(size_buf), 
                "+sizeof(op->data.%s.count)"
                "+(op->data.%s.count*sizeof(op->valchksum[0]))", 
                nameStr, nameStr);
    }
    else if (AL_Test("persistent_start") || 
             (AL_Test("completion") && !AL_Test("request_free")) || 
             AL_Test("sendrecv")) {
        /* only use one of the chksums for starts/completions */
        /* but this is easier than figuring out which... */
        fprintf (packFile,
                 "bcopy (&(op->schksum), &buf[index], sizeof(op->schksum));\n"
                 "index += sizeof(op->schksum);\n\n");
        fprintf (packFile,
                 "bcopy (&(op->rchksum), &buf[index], sizeof(op->rchksum));\n"
                 "index += sizeof(op->rchksum);\n\n");

        fprintf (unpackFile, 
                 "bcopy (&buf[index], &(op->schksum), sizeof(op->schksum));\n"
                 "index += sizeof(op->schksum);\n\n");
        fprintf (unpackFile, 
                 "bcopy (&buf[index], &(op->rchksum), sizeof(op->rchksum));\n"
                 "index += sizeof(op->rchksum);\n\n");

        /* account for chksums in pack_op_size... */
        sprintf(size_buf+strlen(size_buf),
                "+sizeof(op->schksum)+sizeof(op->rchksum)");
    }
    else if (AL_Test("send")) { 
        fprintf (packFile,
                 "bcopy (&(op->schksum), &buf[index], sizeof(op->schksum));\n"
                 "index += sizeof(op->schksum);\n\n");

        fprintf (unpackFile, 
                 "bcopy (&buf[index], &(op->schksum), sizeof(op->schksum));\n"
                 "index += sizeof(op->schksum);\n\n");

        /* account for schksum in pack_op_size... */
        sprintf(size_buf+strlen(size_buf),
                "+sizeof(op->schksum)");
    }
    else if (AL_Test("recv")) { 
        fprintf (packFile,
                 "bcopy (&(op->rchksum), &buf[index], sizeof(op->rchksum));\n"
                 "index += sizeof(op->rchksum);\n\n");

        fprintf (unpackFile, 
                 "bcopy (&buf[index], &(op->rchksum), sizeof(op->rchksum));\n"
                 "index += sizeof(op->rchksum);\n\n");

        /* account for rchksum in pack_op_size... */
        sprintf(size_buf+strlen(size_buf),
                "+sizeof(op->rchksum)");
    }
#endif

    if ((AL_Test("comm_constructor")) &&
        (strcmp (name, "MPI_Comm_dup") != 0)) {
        /* pack comm info from HACK locations... */
        fprintf (packFile,
                 "temp = op->data.%s.size;\n" 
                 "bcopy (&(op->data.%s.size), &buf[index], sizeof(int));\n"
                 "index += sizeof(int);\n\n" 
                 "if (temp) {\n"
                 "bcopy (op->data.%s.array_of_indices, &buf[index], "
                 "sizeof(op->data.%s.array_of_indices[0])*temp);\n"
                 "index += (sizeof(op->data.%s.array_of_indices[0])*temp);\n"
                 "}\n\n", 
                 nameStr, nameStr, nameStr, nameStr, nameStr);

        fprintf (unpackFile, 
                 "{\n"
                 "int temp;\n"
                 "bcopy (&buf[index], &(temp), sizeof(int));\n"
                 "op->data.%s.size = temp;\n"
                 "index += sizeof(int);\n\n" 
                 "if (temp) {\n"
                 "op->data.%s.array_of_indices = "
                 "(int *) malloc (sizeof(int) * temp);\n"
                 "assert(op->data.%s.array_of_indices);\n"
                 "bcopy (&buf[index], op->data.%s.array_of_indices, "
                 "sizeof(op->data.%s.array_of_indices[0])*temp);\n"
                 "index += (sizeof(op->data.%s.array_of_indices[0])*temp);\n"
                 "} else {\n"
                 "op->data.%s.array_of_indices = NULL;\n"
                 "}\n}\n\n", 
                 nameStr, nameStr, nameStr, nameStr, nameStr, nameStr, nameStr);

        /* account for comm info in pack_op_size... */
        sprintf(size_buf+strlen(size_buf), 
                "+sizeof(op->data.%s.size)"
                "+(op->data.%s.size*sizeof(op->data.%s.array_of_indices[0]))", 
                nameStr, nameStr, nameStr);
    }
  
    if (strcmp (name, "MPI_Type_commit") == 0) {
        fprintf (packFile,
                 "//umpi_pack_typemap (buf, op->themap, &index);\n\n");

        fprintf (unpackFile, 
                 "//umpi_unpack_typemap (buf, &(op->themap), &index);\n\n");

        /* account for typemap in pack_op_size... */
        sprintf(size_buf+strlen(size_buf),
                //	    "+umpi_mpi_typemap_packed_size(op->themap)");
                "+0");
    }

    /* pack trailer */
    fprintf (packFile, 
             "assert (index == (packed_size - sizeof(int)));\n"
             "temp = UMPI_END_PACKED_OP;\n"
             "bcopy (&temp, &buf[index], sizeof(int));\n\n");

    /* finish pack_op definition... */
    fprintf (packFile, 
             "return UMPIERR_OK;\n"
             "} /* umpi_pack_op_%s */\n\n\n", name);

    /* finish unpack_op definition... */
    fprintf (unpackFile, 
             "return index;\n"
             "} /* umpi_unpack_op_%s */\n\n\n", name);

    /* print pack_size definition... */
    fprintf (packFile, "int umpi_pack_op_size_%s(umpi_op_t*op)\n{\n",name);
    fprintf (packFile, 
             "return %s;\n"
             "} /* umpi_pack_op_size_%s */\n\n\n",size_buf,name);

    return;
}


/* eof */
