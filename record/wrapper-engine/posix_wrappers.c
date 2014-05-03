/*
 * Copyright (c) 2002-May 2, 2014: Bronis de Supinski (bronis@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */
/* mpi_wrappers.c -- code to record MPI wrappers when needed */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>

#include "y.tab.h"
#include "wrapper.h"




int PrintPosixFormalArg (char *buf, arg_t * arg) {
    int i;
    char *cp = buf;
    if (arg->type.t_extern) {
      cp += sprintf (cp, "extern ");
    }

    if (arg->type.t_const) {
      cp += sprintf (cp, "const ");
    }

    switch (arg->type.type) {
    case ELLIPSIS:
      cp += sprintf (cp, "...");
      break;
    case T_INT:
      cp += sprintf (cp, "int ");
      break;
    case T_DOUBLE:
      cp += sprintf (cp, "double ");
      break;
    case T_CHAR:
      cp += sprintf (cp, "char ");
      break;
    case T_STRUCT:
      cp += sprintf (cp, "struct %s ", arg->type.name);
      break;
    case T_USER:
      cp += sprintf (cp, "%s ", arg->type.name);
      break;
    case T_VOID:
      cp += sprintf (cp, "void ");
      break;
    default:
      printf ("TYPE BUILDING: [%s]\n", buf);
      Abort ("Type unknown");
      break;
    }

    for (i = 0; i < arg->type.t_pointers; i++) {
      cp += sprintf (cp, "*");
    }

    if (arg->name) {
      cp += sprintf (cp, "%s ", arg->name);
    }

    for (i = 0; i < 5; i++) {
      if (arg->type.array[i] > 0) {
        cp += sprintf (cp, "[%d]", arg->type.array[i]);
      }
      else if (arg->type.array[i] < 0) {
        cp += sprintf (cp, "[]");
      }
    }

    // return num chars printed out
    return (cp - buf);
}

/* Since everything is passed by reference in Fortran, we need to use pointers, where they do
 * not exist in the C definition that we invoke 
 *
 * This Returns the number of characters that were written out to the buffer, including the trailing '\0'.
 */
int PrintFormalPosixFortranArg (char *buf, arg_t * arg) {
    int i;
    char *cp = buf;

    /* Everything we care about in fortran bindings is some kind of pointer */
    const char *arg_tname = NameForType(&arg->type);
    if (strstr(arg_tname, "_function") != NULL || strcmp(NameForType(&arg->type), "MPI_Aint") == 0) {
      /* Just declare handler function pointers and address pointers as their actual type to 
         avoid compiler warnings on assignment. */
      cp += sprintf(cp, "%s ", arg_tname);
    } else {
      /* Most things are just pointers to fortran ints. */
      cp += sprintf(cp, "MPI_Fint ");
    }

    if ((arg->type.t_pointers == 0) && (arg->type.array[0] == 0)) {
      cp += sprintf (cp, "*");	/* all fortran parameters are pointers... */
    } else {
      for (i = 0; i < arg->type.t_pointers; i++) {
        cp += sprintf (cp, "*");
      }
    }
    
    cp += sprintf (cp, "%s ", arg->name);

    for (i = 0; i < 5; i++) {
      if (arg->type.array[i] > 0) {
        cp += sprintf (cp, "[%d]", arg->type.array[i]);
      } else if (arg->type.array[i] < 0) {
        cp += sprintf (cp, "[]");
      }
    }

    return (cp - buf);
}


/**
 * Prints out parameters for a delegation call from a C routine TO a C wrapper routine.
 * This passes arguments straight through, with no special processing as for Fortran.
 */
void
PrintAllPosixCParams (void)
{
    int i;
    fprintf (posixWrapFile, "pc");
    for (i = 1; i <= argCounter && args[i].type.type != ELLIPSIS; i++) {
      fprintf (posixWrapFile, ",\n\t\t\t%s", args[i].name);
    }
    return;
}


void
RecordPosixCWrapper (char *name, int is_definition)
{
    int i;
    char buf[4096];
    PrintPosixFormalArg (buf, &(args[0]));
    if(is_definition)
      fprintf (posixWrapFile, "\n%s__real_%s( ", buf, name);
    else
      fprintf (posixWrapFile, "\n%s__wrap_%s( ", buf, name);
    for (i = 1; i <= argCounter; i++) {
        PrintPosixFormalArg (buf, &(args[i]));
        if (i == 1) {
            fprintf (posixWrapFile, " %s", buf);
        }
        else {
            fprintf (posixWrapFile, ",\n\t\t%s", buf);
        }
    }
    if(is_definition)
      fprintf (posixWrapFile, ");\n");
    else {
    fprintf (posixWrapFile, ")\n");
    fprintf (posixWrapFile, "{\n");
    fprintf (posixWrapFile, "\tint rc = 0;\n");
    fprintf (posixWrapFile, "\tvoid *pc = 0x0;\n");
    fprintf (posixWrapFile, "\tGET_FP;\n");
    fprintf (posixWrapFile, "\t//DEFINE_PC;\n");
    fprintf (posixWrapFile, "\t//GATHER_PC;\n");

    fprintf (posixWrapFile, "\trc = gwrap_%s(", name);
    PrintAllPosixCParams();
    fprintf (posixWrapFile, " );\n");

    /* finish */
    fprintf (posixWrapFile, "\treturn rc;\n");
    fprintf (posixWrapFile, "}\n");
    }
    return;
}


void
PrintAllPosixGenericArgs (void)
{
    int i;
    char buf[4096];

    //Frank: 0 argument case cause "," -- should be omitted
    if (argCounter == 0)
        fprintf (posixWrapFile, "\nvoid * pc");
    else
        fprintf (posixWrapFile, "\nvoid * pc ,");
    for (i = 1; i <= argCounter; i++) {
        PrintPosixFormalArg (buf, &(args[i]));
        if (i == 1) {
            fprintf (posixWrapFile, " %s", buf);
        }
        else {
            fprintf (posixWrapFile, ",\n%s", buf);
        }
    }
    fprintf (posixWrapFile, ")\n");

    return;
}


int
RecordGenericPosixOpStart (char *name, int warn_omit)
{
    int i;

    PrintAllPosixGenericArgs();
    fprintf (posixWrapFile, "\n{\nint rc = 0;\numpi_op_t *uop = ");
    fprintf (posixWrapFile, "(umpi_op_t *)calloc(sizeof(umpi_op_t), 1);\n\n");
    fprintf (posixWrapFile, "assert(uop);\n");
    fprintf (posixWrapFile, "uop->op = umpi_%s;\n", name);
    fprintf (posixWrapFile, "uop->rank = umpi_rank;\n");
    fputs ("uop->seq_num = umpi_task_seq_num++;\n", posixWrapFile);
    fputs ("uop->pc = pc;\n", posixWrapFile);

    /* for fortran, dereference the fortran required pointer */
    /* if it was more than just a pointer, hope this works... */
    for (i = 1; i <= argCounter; i++) {
        const char *arg_tname = NameForType(&args[i].type);

        if ((!(args[i].eNoSave)) && (strlen (args[i].name) > 0)) {
            /* handle "special" cases first */
            if (strcmp(args[i].name, "op_ptr") == 0) {
                fprintf (posixWrapFile, "uop->data.%s.op = *(op_ptr);\n", nameStr);
            }
            else if (strcmp(args[i].name, "status") == 0) {
                /* this can break with Fortran but will often work */
                fprintf (posixWrapFile, 
                         "uop->data.%s.%s_src = %s->MPI_SOURCE;\n", 
                         nameStr, args[i].name, args[i].name);
            }
            else if (strcmp(args[i].name, "array_of_statuses") == 0) {
                /* this can break with Fortran but will often work */
                fprintf (posixWrapFile, 
                         "uop->data.%s.statuses_srcs = "
                         "(int *) malloc (uop->data.%s.count * sizeof(int));\n"
                         "assert(uop->data.%s.statuses_srcs);\n{\n"
                         "int i;\n"
                         "for(i=0;i<uop->data.%s.count;i++) {\n"
                         "uop->data.%s.statuses_srcs[i] = "
                         "array_of_statuses[i].MPI_SOURCE;\n"
                         "}\n}\n", 
                         nameStr, nameStr, nameStr, nameStr, nameStr);
            }
            else if (strstr(args[i].name, "function") != NULL) {
                /* save the address... */
                fprintf (posixWrapFile, "uop->data.%s.%s = &(%s);\n",
                         nameStr, args[i].name, args[i].name);
            }
            else if ((args[i].type.t_pointers != 0) &&
                     (args[i].type.type == T_VOID)) {
                /* save the address of void pointers; usually what you want... */
                fprintf (posixWrapFile, "uop->data.%s.%s = (void*)%s;\n",
                         nameStr, args[i].name, args[i].name);

            } else if (strstr(args[i].name, "array") != NULL) {
                /* save the array of values... */
                fprintf (posixWrapFile, 
                         "uop->data.%s.%s = "
                         "(%s *) malloc (uop->data.%s.count * sizeof (%s));\n"
                         "assert (uop->data.%s.%s);\n"
                         "bcopy (%s, uop->data.%s.%s, "
                         "uop->data.%s.count * sizeof (%s));\n",
                         nameStr, args[i].name, arg_tname, nameStr, arg_tname,
                         nameStr, args[i].name, args[i].name, 
                         nameStr, args[i].name, nameStr, arg_tname);
            }
            else if ((strstr(args[i].name, "counts") != NULL) ||
                     (strstr(args[i].name, "displs") != NULL)) {
                /* save the array of values if parameter is significant */
                if (((strcmp (name, "MPI_Gatherv") == 0) &&
                     ((strcmp(args[i].name, "recvcounts") == 0) ||
                      (strcmp(args[i].name, "displs") == 0))) ||
                    ((strcmp (name, "MPI_Scatterv") == 0) &&
                     ((strcmp(args[i].name, "counts") == 0) ||
                      (strcmp(args[i].name, "displs") == 0)))) {
                    /* these parameters are only significant at the root... */
                    fprintf (posixWrapFile, 
                             "{\n"
                             "int temp_umpi_comm_size, temp_umpi_lrank;\n"
                             "/* %s only significant at root... */\n"

                             //mike noeth fix
                             //"PMPI_Comm_rank (uop->data.%s.comm, "
                             "PMPI_Comm_rank (comm, "

                             "&temp_umpi_lrank);\n"
                             "if (temp_umpi_lrank == root) {\n"

                             //mike noeth fix
                             "PMPI_Comm_size (comm, "
                             //"PMPI_Comm_size (uop->data.%s.comm, "
                             "&temp_umpi_comm_size);\n"

                             "uop->data.%s.%s = "
                             "(%s *) malloc (temp_umpi_comm_size * sizeof (%s));\n"
                             "assert (uop->data.%s.%s);\n"
                             "bcopy (%s, uop->data.%s.%s, "
                             "temp_umpi_comm_size * sizeof (%s));\n"
                             "}\n"
                             "else {\n"
                             "uop->data.%s.%s = NULL;\n"
                             "}\n}\n",
		   
                             //mike noeth fix
                             //args[i].name, nameStr, nameStr, nameStr, 
                             args[i].name, 
                             nameStr, args[i].name, 
                             arg_tname, arg_tname,  // need to cast to this.

                             nameStr, args[i].name, args[i].name, 
                             nameStr, args[i].name, arg_tname, nameStr, args[i].name);
                }
                else {
                    /* save the array of values... */
                    fprintf (posixWrapFile, 
                             "{\n"
                             "int temp_umpi_comm_size;\n"

                             //mike noeth fix
                             //"PMPI_Comm_size (uop->data.%s.comm, "
                             "PMPI_Comm_size (comm, "

                             "&temp_umpi_comm_size);\n"
                             "uop->data.%s.%s = "
                             "(%s *) malloc (temp_umpi_comm_size * sizeof (%s));\n"
                             "assert (uop->data.%s.%s);\n"
                             "bcopy (%s, uop->data.%s.%s, "
                             "temp_umpi_comm_size * sizeof (int));\n"
                             "}\n",

                             //mike noeth fix
                             //nameStr, nameStr, args[i].name, 
                             nameStr, args[i].name, 
                             arg_tname, arg_tname,

                             nameStr, args[i].name, args[i].name, 
                             nameStr, args[i].name);
                }
            }
            else if (strstr(args[i].name, "level") != NULL) {
              fprintf(posixWrapFile, "uop->profile_level = level;\n");

            } else if ((strstr(args[i].name, "retcoords") != NULL) ||
                     (strstr(args[i].name, "dimsp") != NULL)) {
                /* save the array of values... */
                fprintf (posixWrapFile, 
                         "{\n"
                         "PMPI_Cartdim_get (uop->data.%s.comm, &uop->data.%s.size);\n"
                         "uop->data.%s.%s = "
                         "(int *) malloc (uop->data.%s.size * sizeof (int));\n"
                         "assert (uop->data.%s.%s);\n"
                         "bcopy (%s, uop->data.%s.%s, "
                         "uop->data.%s.size * sizeof (int));\n"
                         "}\n",
                         nameStr, nameStr, nameStr, args[i].name,
                         nameStr, nameStr, args[i].name, args[i].name, 
                         nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "retedges") != NULL) {
                /* save the array of values... */
                fprintf (posixWrapFile, 
                         "uop->data.%s.%s = "
                         "(int *) malloc (uop->data.%s.edgecount * sizeof (int));\n"
                         "assert (uop->data.%s.%s);\n"
                         "bcopy (%s, uop->data.%s.%s, "
                         "uop->data.%s.edgecount * sizeof (int));\n",
                         nameStr, args[i].name, nameStr,  
                         nameStr, args[i].name, args[i].name, 
                         nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "edges") != NULL) {
                /* save the array of values... */
                fprintf (posixWrapFile, 
                         "{\n"
                         "int temp_umpi_nedges = "
                         "uop->data.%s.array_of_degrees[uop->data.%s.count-1];\n"
                         "uop->data.%s.%s = "
                         "(int *) malloc (temp_umpi_nedges * sizeof (int));\n"
                         "assert (uop->data.%s.%s);\n"
                         "bcopy (%s, uop->data.%s.%s, "
                         "temp_umpi_nedges * sizeof (int));\n"
                         "}\n",
                         nameStr, nameStr, nameStr, args[i].name,
                         nameStr, args[i].name, args[i].name, 
                         nameStr, args[i].name);
            }
            else if (strstr(args[i].name, "the_addresses") != NULL) {
                /* save the array of values... */
                fprintf (posixWrapFile, 
                         "uop->data.%s.%s = "
                         "(MPI_Aint *) malloc (uop->data.%s.addrcount * sizeof (MPI_Aint));\n"
                         "assert (uop->data.%s.%s);\n"
                         "bcopy (%s, uop->data.%s.%s, "
                         "uop->data.%s.addrcount * sizeof (int));\n",
                         nameStr, args[i].name, nameStr,  
                         nameStr, args[i].name, args[i].name, 
                         nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "the_datatypes") != NULL) {
                /* save the array of values... */
                fprintf (posixWrapFile, 
                         "uop->data.%s.%s = "
                         "(MPI_Datatype *) malloc (uop->data.%s.dtypecount * sizeof (MPI_Datatype));\n"
                         "assert (uop->data.%s.%s);\n"
                         "bcopy (%s, uop->data.%s.%s, "
                         "uop->data.%s.dtypecount * sizeof (int));\n",
                         nameStr, args[i].name, nameStr,  
                         nameStr, args[i].name, args[i].name, 
                         nameStr, args[i].name, nameStr);
            }
            else if (strstr(args[i].name, "ranges") != NULL) {
                /* save the array of values... */
                fprintf (posixWrapFile, 
                         "uop->data.%s.%s = "
                         "(int *) malloc (3 * uop->data.%s.count * sizeof (int));\n"
                         "assert (uop->data.%s.%s);\n"
                         "bcopy (%s, uop->data.%s.%s, "
                         "3 * uop->data.%s.count * sizeof (int));\n",
                         nameStr, args[i].name, nameStr,  
                         nameStr, args[i].name, args[i].name, 
                         nameStr, args[i].name, nameStr);
            }/*Added by Karthik - Start*/
            else if(strstr(args[i].name, "filename") != NULL) {
                fprintf(posixWrapFile,
                        "uop->data.%s.%s = "
                        "(char *) malloc((strlen(%s)+1) * sizeof(char));\n"
                        "assert (uop->data.%s.%s);\n"
                        "bcopy (%s, uop->data.%s.%s, "
                        "(strlen(%s)+1) * sizeof(char));\n",
                        nameStr, args[i].name,
                        args[i].name,
                        nameStr, args[i].name,
                        args[i].name, nameStr, args[i].name,
                        args[i].name);
            }
            else if(strstr(args[i].name, "datarep") != NULL) {
                fprintf(posixWrapFile,
                        "uop->data.%s.%s = "
                        "(char *) malloc((strlen(%s)+1) * sizeof(char));\n"
                        "assert (uop->data.%s.%s);\n"
                        "bcopy (%s, uop->data.%s.%s, " 
                        "(strlen(%s)+1) * sizeof(char));\n",
                        nameStr, args[i].name,
                        args[i].name,
                        nameStr, args[i].name,
                        args[i].name, nameStr, args[i].name,
                        args[i].name);
            }/*Added by Karthik - End*/
            else if (args[i].type.t_pointers != 0)  {
                /* save the "value"... */
                fprintf (posixWrapFile, 
                         "uop->data.%s.%s = *(%s);\n", 
                         nameStr, args[i].name, args[i].name);
            }
            else if (args[i].type.array[0] == 0) {
                fprintf (posixWrapFile, "\t\tuop->data.%s.%s = %s;\n",
                         nameStr, args[i].name, args[i].name);
            }
            else if (warn_omit) {
                /* print warning */
                fprintf (stderr, 
                         "\n\t\tWARNING: Omitting arg %s from wrapper", 
                         args[i].name);
            }
        }
        else if ((warn_omit) && (strlen (args[i].name) > 0)) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: Omitting arg %s from wrapper (NoSave)", 
                     args[i].name);
        }
    }

    /* reserve uop for wrapper processing... */
    fprintf (posixWrapFile, "umpi_inc_ref_count (uop);\n");
    fprintf (posixWrapFile, "#ifdef UMPI_DEBUG_LEVEL_2\n");
    fprintf (posixWrapFile, "q_append (umpi_task_op_q, uop);\n");
    fprintf (posixWrapFile, "#endif\n\n");

    return 0;
}


int
RecordImmediatePosixLocalPreStart (char *name)
{
    /* record beginning of synchronous preop processing definition */
    fprintf (posixWrapFile, 
             "\nstatic int"
             "\nposix_%s_immediate_local_pre(umpi_op_t *op)\n", 
             name);
    fprintf (posixWrapFile, "{\n");
  
    return 0;
}


int
RecordImmediatePosixLocalPre (char *name)
{
    int i, needed = 0;

    if (wrapper_pre[0]) {
        fprintf (stderr, 
                 "\n\t\tWARNING: set extrafields to pass mgr wrapper-pre data");
    
        needed = 1;
        RecordImmediatePosixLocalPreStart (name);
    
        fprintf (posixWrapFile, "\n{\n");
        for (i = 0; wrapper_pre[i]; i++) {
            fprintf (posixWrapFile, "%s\n", wrapper_pre[i]);
        }
        fprintf (posixWrapFile, "}\n");
    }

    /* do stuff related to chksum processing... */
    if ((AL_Test("nonblocking")) || (AL_Test("persistent_init"))) {
        /* for now, tracking requests is done just for chksum processing... */
        /* need to save for completion/request handle... */
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }
    
        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_inc_ref_count (op);\n");
    
        if (AL_Test("persistent_init")) {
            /* need to save for start... */
            fprintf (posixWrapFile, 
                     "q_append (umpi_task_persistent_init_op_q, op);\n");
        }
        else {
            /* need to save for completion... */
            fprintf (posixWrapFile, "q_append (umpi_task_req_start_op_q, op);\n");
        }
    
        if (!AL_Test("persistent_start")) {
            if (!AL_Test("persistent_init")) {
                /* need storage to find completion op... */
                assert (!AL_Test("multiple_requests"));
                fprintf (posixWrapFile, 
                         "op->completions = \n"
                         "(umpi_completion_t *) "
                         "calloc (1, sizeof(umpi_completion_t));\n");
            }

            /* also need to save for getting request handles... */
            fprintf (posixWrapFile, "umpi_l_save_for_postop_processing(op);\n");

            /* also need to save pointer to type map... */
            fprintf (posixWrapFile, "umpi_l_save_typemap_op(op);\n");
        }

        fprintf (posixWrapFile, "#endif\n\n"); 
    }
    else if (AL_Test("send")) {
        /* need to save pointer to type map... */
        /* sendrecv's are weird so we do them within checksum routines... */
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }
    
        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_l_save_typemap_op(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }

    /* get send buffer chksum BEFORE giving to MPI implementation... */
    if ((AL_Test("send")) && (!AL_Test("persistent_init"))) {
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }

        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_get_chksum_send_pre(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }
    else if (AL_Test("collective")) {
        /* skip collectives for now */
        /* eventually want to check asynchronous collectives to make */
        /* sure root postop buffer agrees with the preop buffers */
        /* can check synchronous collectives to make sure postop */
        /* buffers are consistent (and agree with preop buffers) */
        /* WHAT "AGREEMENT" AMOUNTS TO IS TRICKY AND HIGHLY */
        /* DEPENDENT ON THE ACTUAL COLLECTIVE - */
        /* FOR EXAMPLE, MANY DON'T HAVE BUFFERS... */
    }
    else if (AL_Test("persistent_start")) {
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }

        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_get_chksum_persistent_start_pre(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }
    else if (AL_Test("request_free")) {
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }

        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_get_chksum_request_free_pre(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }
    else if (AL_Test("completion")) {
        /* completed request handles get munged during operation so we */
        /* have to figure out which requests are completed in the pre... */
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }

        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_get_chksum_completion_pre(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }

    if (strcmp (name, "MPI_Type_free") == 0) {
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }

        // mike noeth - i striped this out because it was causing a linking error
        // most likely i just have to restore all the umpi_mpi_type* stuff
        // but didn't have time and removing this line worked out...
        //fprintf (posixWrapFile, "umpi_l_free_type (op);\n");
    } 
    else if (strcmp (name, "MPI_Type_commit") == 0) {
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPreStart (name);
        }

        // Frank: see Mike's comment above
        //fprintf (posixWrapFile, 
        //	     "umpi_mpi_l_save_typemap (op, op->data.mpi.newtype);\n");
    } 

    if (needed)
        fprintf (posixWrapFile, "return 0;\n}\n\n");
    else
        AL_Set ("nolocalpre");

    return needed;
}


int
RecordDelayedPosixLocalorGlobalPre (char *name)
{
    if (AL_Test("MPI2_function"))
        /* for now, we just ignore these... */
        return 0;

    if ((strcmp (name, "MPI_Wtick") == 0) ||
        (strcmp (name, "MPI_Wtime") == 0))
        /* catching these would probably be a bad idea */
        /* especially since the standard says that the */
        /* implementation can omit them from the profiling interface... */
        return 0;

    /* these are handled in umpi_mpi_extra... */
    /* MPI_Errhandler_set is only handled there w/UMPI_MPI_PANIC... */
    if ((strcmp (name, "MPI_Abort") == 0) ||
        (strcmp (name, "MPI_Errhandler_set") == 0) ||
        (strcmp (name, "MPI_Finalize") == 0) ||
        (strcmp (name, "MPI_Init") == 0))
        return 0;

    /* initially, do it unless overridden by wrapper spec... */
    if ((AL_Test ("nopre")) || (AL_Test ("noasynchpre")))
        return 0;

    return 1;
}


int
RecordImmediatePosixLocalPostStart (char *name)
{
    /* record beginning of synchronous postop processing definition */
    fprintf (posixWrapFile, 
             "\nstatic int"
             "\nposix_%s_immediate_local_post(umpi_op_t *op)\n", 
              name);
    fprintf (posixWrapFile, "{\n");

    return 0;
}


int
RecordImmediatePosixLocalPost (char *name)
{
    int i, needed = 0;

    /* user/spec supplied immediate processing... */
    if (wrapper_post[0]) {
        fprintf (stderr, 
                 "\n\t\tWARNING: set extrafields to pass mgr wrapper-post data");
    
        needed = 1;
        RecordImmediatePosixLocalPostStart (name);

        fprintf (posixWrapFile, "\n{\n");
        for (i = 0; wrapper_post[i]; i++) {
            fprintf (posixWrapFile, "%s\n", wrapper_post[i]);
        }
        fprintf (posixWrapFile, "}\n");
    }

    if ((!AL_Test("persistent_start")) && 
        ((AL_Test("nonblocking")) || (AL_Test("persistent_init")))) {
        /* store handles in preop for chksum processing... */
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPostStart (name);
        }
    
        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_l_save_req_handle_from_postop (op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }

    /* get recv buffer chksum AFTER giving op to MPI implementation... */
    if (AL_Test("sendrecv")) {
        /* no reason to get the chksum now if it was nonblocking... */
        assert (!AL_Test("nonblocking"));

        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPostStart (name);
        }

        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_get_chksum_sendrecv_post(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }
    else if ((AL_Test("recv")) && (!AL_Test("nonblocking"))) {
        /* completed the receive so get chksum... */
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPostStart (name);
        }

        /* need to save pointer to type map... */
        /* sendrecv's are weird so we do them within checksum routines... */
        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_l_save_typemap_op(op);\n");

        fprintf (posixWrapFile, "umpi_get_chksum_recv_post(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }
    else if ((AL_Test("completion")) && (!AL_Test("request_free"))) {
        /* compute the chksums and see if locally consistent... */
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPostStart (name);
        }

        fprintf (posixWrapFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (posixWrapFile, "umpi_get_chksum_completion_post(op);\n");
        fprintf (posixWrapFile, "#endif\n\n"); 
    }

    /* get the lrank_to_grank translation when needed... */
    if ((AL_Test("comm_constructor")) &&
        (strcmp (name, "MPI_Comm_dup") != 0) &&
        (strcmp (name, "MPI_Intercomm_create") != 0)) {
        if (!needed) {
            needed = 1;
            RecordImmediatePosixLocalPostStart (name);
        }

        fprintf (posixWrapFile, 
                 "umpi_package_lranks_to_granks (op, op->data.%s.comm_out);\n",
                 nameStr);
    }

    if (needed)
        fprintf (posixWrapFile, "return 0;\n}\n\n");
    else
        AL_Set ("nolocalpost");

    return needed;
}


int
RecordDelayedPosixLocalorGlobalPost (char *name)
{
    if (AL_Test("MPI2_function"))
        /* for now, we just ignore these... */
        return 0;

    if ((strcmp (name, "MPI_Wtick") == 0) ||
        (strcmp (name, "MPI_Wtime") == 0))
        /* catching these would probably be a bad idea */
        /* especially since the standard says that the */
        /* implementation can omit them from the profiling interface... */
        return 0;

    /* these are handled in umpi_mpi_extra... */
    /* MPI_Errhandler_set is only handled there w/UMPI_MPI_PANIC... */
    if ((strcmp (name, "MPI_Abort") == 0) ||
        (strcmp (name, "MPI_Errhandler_set") == 0) ||
        (strcmp (name, "MPI_Finalize") == 0) ||
        (strcmp (name, "MPI_Init") == 0))
        return 0;

    /* initially, do it unless overridden by wrapper spec... */
    if ((AL_Test ("nopost")) || (AL_Test ("noasynchpost")))
        return 0;

    return 1;
}


int
RecordPosixPre (char *name)
{
    int need_immediate_local = RecordImmediatePosixLocalPre (name);
    int need_delayed_local_or_global = RecordDelayedPosixLocalorGlobalPre (name);

    /* no preop processing if no immediate or asynchronous processing... */
    if (!(need_immediate_local || need_delayed_local_or_global)) {
        AL_Set ("nopre");

        return 0;
    }

    if ((AL_Test ("nolocalpre")) && (AL_Test ("noasynchpre")))
        /* enables full override w/o explicitly setting... */
        AL_Set ("nopre");

    /* handle full override (explicit and implicit)... */
    if (AL_Test ("nopre")) {
        /* print warning */
        fprintf (stderr, 
                 "\n\t\tWARNING: Omitting preop processing for %s: ", name);
        if (need_immediate_local)
            fprintf (stderr, "\n\t\t\tSynchronous (local) processing needed;");
        if (need_delayed_local_or_global)
            fprintf (stderr, "\n\t\t\tAsynchronous processing needed;");
   
        /* record warning comment... */
        fprintf (posixWrapFile, 
                 "\n/*------------ Needed preop processing omitted */\n");

        return 0;
    }

    /* record beginning of preop processing definition */
    fprintf (posixWrapFile, 
             "\nstatic int\nposix_%s_pre( ", name);
    RecordGenericPosixOpStart (name, 1);
    fprintf (posixWrapFile, "uop->order = umpi_preop;\n");

    /* ##### if necessary, do any processing before make actual MPI call */
    if (need_immediate_local) {
        if (AL_Test ("nolocalpre")) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: "
                     "Omitting needed synchronous preop processing for %s!", name);

            /* record warning comment... */
            fprintf (posixWrapFile, 
                     "\n/*------ Needed synchronous preop processing omitted */\n");
        }
        else {
            fprintf (posixWrapFile, 
                     "\nrc = posix_%s_immediate_local_pre(uop);\n", 
                     name);
        }
    }

    /* ##### if necessary, set up for any asynchronous preop processing */
    if (need_delayed_local_or_global) {
        if (AL_Test ("noasynchpre")) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: "
                     "Omitting needed asynchronous preop processing for %s!", name);

            /* record warning comment... */
            fprintf (posixWrapFile, 
                     "\n/*----- Needed asynchronous preop processing omitted */\n");
        }
        else {
            fprintf (posixWrapFile, "rc |= umpi_Asynch_op(uop);\n");
        }
    }

    /* give up uop reservation for wrapper processing... */
    fprintf (posixWrapFile, 
             "umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);\n");

    fprintf (posixWrapFile, "return 0;\n}\n");

    return 1;
}


int
RecordPosixPost (char *name, int need_pre)
{
    int need_immediate_local = RecordImmediatePosixLocalPost (name);
    int need_delayed_local_or_global = RecordDelayedPosixLocalorGlobalPost (name);

    /* no postop processing if no immediate or asynchronous processing... */
    if (!(need_immediate_local || need_delayed_local_or_global)) {
        AL_Set ("nopost");

        return 0;
    }

    if ((AL_Test ("nolocalpost")) && (AL_Test ("noasynchpost")))
        /* enables full override w/o explicitly setting... */
        AL_Set ("nopost");

    /* handle full override (explicit and implicit)... */
    if (AL_Test ("nopost")) {
        /* print warning */
        fprintf (stderr, 
                 "\n\t\tWARNING: Omitting postop processing for %s: ", name);
        if (need_immediate_local)
            fprintf (stderr, "\n\t\t\tSynchronous (local) processing needed;");
        if (need_delayed_local_or_global)
            fprintf (stderr, "\n\t\t\tAsynchronous processing needed;");
   
        /* record warning comment... */
        fprintf (posixWrapFile, 
                 "\n/*------------ Needed postop processing omitted */\n");

        return 0;
    }

    /* record beginning of postop processing definition */
    fprintf (posixWrapFile, 
             "\nstatic int\nposix_%s_post( \nint MPI_rc, ", name);
    RecordGenericPosixOpStart (name, !need_pre);
    fprintf (posixWrapFile, "uop->order = umpi_postop;\n");
    fputs ("uop->res = MPI_rc;\n", posixWrapFile);

    /* ##### if necessary, do any processing before make actual MPI call */
    if (need_immediate_local) {
        if (AL_Test ("nolocalpost")) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: "
                     "Omitting needed synchronous postop processing for %s!", name);

            /* record warning comment... */
            fprintf (posixWrapFile, 
                     "\n/*----- Needed synchronous postop processing omitted */\n");
        }
        else {
            fprintf (posixWrapFile, 
                     "\nrc = posix_%s_immediate_local_post(uop);\n", 
                      name);
        }
    }

    /* ##### if necessary, set up for any asynchronous postop processing */
    if (need_delayed_local_or_global) {
        if (AL_Test ("noasynchpost")) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: "
                     "Omitting needed asynchronous postop processing for %s!", 
                     name);

            /* record warning comment... */
            fprintf (posixWrapFile, 
                     "\n/*---- Needed asynchronous postop processing omitted */\n");
        }
        else {
            fprintf (posixWrapFile, "rc |= umpi_Asynch_op(uop);\n");
        }
    }

    /* give up uop reservation for wrapper processing... */
    fprintf (posixWrapFile, 
             "umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);\n");

    fprintf (posixWrapFile, "return rc;\n}\n");

    return 1;
}

void MakePosixCall(char *name) {
  int i;

  /* make actual MPI call through profiling interface... */
  fprintf (posixWrapFile, "\trc = __real_%s( ", name);
  for (i = 1; i <= argCounter && args[i].type.type != ELLIPSIS; i++) {
    char aname[256];
    if ((args[i].type.t_pointers == 0) && (args[i].type.array[0] == 0)) {
      sprintf(aname, "(%s)", args[i].name);
    }
    else {
      sprintf(aname, "(%s)", args[i].name);
    }
    if (i == 1) {
      fprintf (posixWrapFile, " %s", aname);
    }
    else {
      fprintf (posixWrapFile, ",\n%s", aname);
    }
  }
  fprintf (posixWrapFile, ");\n");
}


void
RecordPosixOpenWrapperGuard(char *name) {
  /* This guards against reentering wrappers from inside mpi calls. */
  fprintf (posixWrapFile, "\nif (is_mpi_inited)");

}

void
RecordPosixCloseWrapperGuard() {
  /* Closing brace for reentry guard. */
  fprintf (posixWrapFile, "\nif (is_mpi_inited)");
}


void
RecordPosixWrapper (char *name)
{
    int i;
    int need_pre = 0;
    int need_post = 0;

    /* record delimiting comment... */
    fprintf (posixWrapFile, 
             "\n/*--------------------------------------------- %s */\n", name);


    /* Check if the wrapper's disabled.  If it is, just print as much and return. */
    if (AL_Test ("nowrapper")) {
        /* print warning */
        fprintf (stderr, "\n\t\tWARNING: Omitting wrapper for %s: ", name);
        if (need_pre)
            fprintf (stderr, "preop needed;");
        if (need_post)
            fprintf (stderr, "postop needed;");
   
        /* record warning comment... */
        fprintf (posixWrapFile, 
                 "\n/*-------------------- Wrapper for %s omitted */\n", name);

        return;
    }

    /* Wrapper not disabled for this function... generate it. */
    
    RecordPosixCWrapper(name, 1); //Record the definition of __real_fn
    /* check if we need pre- and post-routines */
    need_pre = RecordPosixPre (name);
    need_post = RecordPosixPost (name, need_pre);
    
    /* no wrapper needed if we don't need either a preop or a postop... */
    if (!(need_pre || need_post)) {
      AL_Set ("nowrapper");
      
      /* record warning comment... */
      fprintf (posixWrapFile, 
               "\n/*-------------------- Wrapper for %s omitted */\n", name);
      
      return;
    }

    /* record beginning of wrapper definition (name, args, etc...) */
    fprintf (posixWrapFile, "\nstatic int\ngwrap_%s( ", name);
    PrintAllPosixGenericArgs ();
    fprintf (posixWrapFile, "{\nint rc;\n");
    

    RecordPosixOpenWrapperGuard(name);

    /* ##### if necessary, package PRE op and connect to manager */
    if (need_pre) {
        assert (!AL_Test ("nopre"));

        /* Make the actual pre-wrapper call. */
        fprintf (posixWrapFile, "\nposix_%s_pre(", name);
        PrintAllPosixCParams();
        fprintf (posixWrapFile, ");\n");
    }

    MakePosixCall(name);
    
    RecordPosixCloseWrapperGuard();

    /* ##### if necessary, package POST op and connect to manager */
    if (need_post) {
        assert (!AL_Test ("nopost"));

        /* Make the actual post wrapper call. */
        fprintf (posixWrapFile, "\nposix_%s_post(rc, ", name);
        PrintAllPosixCParams();
        fprintf (posixWrapFile, ");\n");
    }


    /* finish */
    fprintf (posixWrapFile, "\treturn rc;\n");
    fprintf (posixWrapFile, "}\n");
    RecordPosixCWrapper(name, 0);
}


/* eof */
