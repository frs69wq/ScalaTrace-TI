/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Mon July 1 2002 */
/* based on prototype code by Jeffrey Vetter... */
/* mpi_wrappers.c -- code to record MPI wrappers when needed */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>

#include "y.tab.h"
#include "wrapper.h"


/* Descriptor for MPI handles. */
typedef struct _mpi_handle_type_t {
  const char *const type_name;  /* Name of type */
  const char *const cpre;      /* Name of f2c/c2f conversion function prefix */
} mpi_htype_t;

/* Todd: We use MPI_-prefixed conversion routines b/c MPICH doesn't declare all the
 * PMPI_blah_f2c prototypes. If we want to profile the conversion routines (which
 * will likely never be very interesting), we might want to change these to be PMPI.
 */
static mpi_htype_t MPI_DEFINED_TYPES[] = {
  {"MPI_Comm",       "MPI_Comm"},
  {"MPI_Errhandler", "MPI_Errhandler"},
  {"MPI_File",       "MPI_File"},
  {"MPI_Group",      "MPI_Group"},
  {"MPI_Info",       "MPI_Info"},
  {"MPI_Op",         "MPI_Op"},
  {"MPI_Request",    "MPI_Request"},
  {"MPI_Status",     "MPI_Status"},
  {"MPI_Datatype",   "MPI_Type"},
  {"MPI_Win",        "MPI_Win"}
};
static const int NUM_MPI_TYPES = 10; /* Keep this synced w/array. */

/* Suffix for fortran wrapper implementation. */
#define F_WRAP_SUFFIX "f_wrap"


/**
 * Returns true if there is a parameter of a particular name in the arg list.
 */
int
HasParameterWithName(char *name) {
  for (int i = 1; i <= argCounter && args[i].type.type != ELLIPSIS; i++) {
    if (strcmp(args[i].name, name) == 0) {
      return 1;
    }
  }
  return 0;
}



int PrintFormalArg (char *buf, arg_t * arg) {
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
int PrintFormalFortranArg (char *buf, arg_t * arg) {
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
 * This prints paramters for a delegation call TO a C routine FROM the fortran wrapper.  
 * That is, for arguments that are NOT pointers in the C MPI declarations, it dereferences
 * them for the C call.  For arguments that are of MPI-defined types, this invokes
 * the relevant f2c routine, as well.
 */
void
DelegateFortranCall (char *name)
{
    int i, j;
    char local_buf[4096]       = "\0";
    char copy_buf[4096]        = "\0";
    char call_buf[4096]        = "\0";
    char mpich_call_buf[4096]  = "\0";
    char write_buf[4096]       = "\0";

    char *locals =     local_buf;
    char *copies =     copy_buf;
    char *call =       call_buf;
    char *mpich_call = mpich_call_buf;
    char *writebacks = write_buf;

    int haveLoopVar = 0;

    call += sprintf (call, "\t\trc = gwrap_%s(pc ", name);
    mpich_call += sprintf (mpich_call, "\t\trc = gwrap_%s(pc ", name);

    for (i = 1; i <= argCounter && args[i].type.type != ELLIPSIS; i++) {
      const char *const t_name = NameForType(&args[i].type);
      const char *const arg_name = args[i].name;
      mpi_htype_t *mpi_type = NULL;

      call += sprintf (call, ",\n\t\t\t");
      mpich_call += sprintf (mpich_call, ",\n\t\t\t");

      /* See if the arg is one of the special MPI types. */
      for (j=0; j < NUM_MPI_TYPES; j++) {
        if (strcmp(MPI_DEFINED_TYPES[j].type_name, t_name) == 0) {
          mpi_type = &MPI_DEFINED_TYPES[j];
          break;
        }
      }

      /* See if the arg is a pointer/array type or not. */
      if ((args[i].type.t_pointers == 0) && (args[i].type.array[0] == 0)) {
        if (mpi_type) {
          /* Need to dereference and convert non-pointer MPI types. */
          /* Note that we don't need a special case for MPI_Status here b/c statuses aren't ever passed by value. */
          call += sprintf(call, "%s_f2c(*(%s))", mpi_type->cpre, arg_name);
          mpich_call += sprintf(mpich_call, "(%s)(*(%s))", mpi_type->type_name, arg_name);

        } else {
          /* Need to dereference non-pointer types. */
          call += sprintf(call, "*(%s)", arg_name);
          mpich_call += sprintf(mpich_call, "*(%s)", arg_name);
        }

      } else {
        /* For MPI type arrays/pointers, need to make a local one and assign forward and back. */
        if (mpi_type) {
          /* Just cast for MPICH; we'll pass this straight through. */
          mpich_call += sprintf(mpich_call, "(%s *)%s", NameForType(&args[i].type), arg_name);

          /* Check if it's an array. */
          if (strstr(args[i].name, "array") != NULL ||
              strcmp(args[i].name, "the_datatypes") == 0
          ) {
            char copy[128];
            char writeback[128];

            /* Need to allocate, copy, and copy back a temporary array of converted values. 
               This assumes that for array calls, there's always a "count" parameter that tells us how 
               many things are in the array. */
            if (!haveLoopVar) {
              locals += sprintf(locals, "\t\tint temp_i;\n");
              haveLoopVar = 1;
            }

            if (strcmp(mpi_type->type_name, "MPI_Status") == 0) {
              /* Syntax for status conversion is special. */
              sprintf(copy, "\t\t\tPMPI_Status_f2c(&%s[temp_i], &temp_%s[temp_i]);", arg_name, arg_name);
              sprintf(writeback, "\t\t\tPMPI_Status_c2f(&temp_%s[temp_i], &%s[temp_i]);", arg_name, arg_name);
            } else {
              /* All others follow this formula. */
              sprintf(copy, "\t\t\ttemp_%s[temp_i] = %s_f2c(%s[temp_i]);", arg_name, mpi_type->cpre, arg_name);
              sprintf(writeback, "\t\t\t%s[temp_i] = %s_c2f(temp_%s[temp_i]);", arg_name, mpi_type->cpre, arg_name);
            }

            locals += sprintf(locals, 
                    "\t\t%s *temp_%s = (%s *)malloc(sizeof(%s) * *(count));\n",
                    t_name, arg_name, t_name, t_name
            );
            copies += sprintf(copies, "\t\tfor (temp_i = 0; temp_i < *(count); temp_i++) {\n%s\n\t\t}\n", copy);
            call += sprintf(call, "temp_%s", arg_name);
            writebacks += sprintf(writebacks, 
                    "\t\tfor (temp_i = 0; temp_i < *(count); temp_i++) {\n%s\n\t\t}\n"
                    "\t\tfree(temp_%s);\n",
                    writeback, arg_name
            );
            
          } else {
            /* Need to allocate, copy, and copy back a temporary value. */
            locals += sprintf(locals, "\t\t%s temp_%s;\n", t_name, arg_name);
            call += sprintf(call, "&temp_%s", arg_name);

            if (strcmp(mpi_type->type_name, "MPI_Status") == 0) {
              copies += sprintf(copies, "\t\tPMPI_Status_f2c(%s, &temp_%s);\n", arg_name, arg_name);
              writebacks += sprintf(writebacks, "\t\tPMPI_Status_c2f(&temp_%s, %s);\n", arg_name, arg_name);
            } else {
              copies += sprintf(copies, "\t\ttemp_%s = %s_f2c(*(%s));\n", arg_name, mpi_type->cpre, arg_name);
              writebacks += sprintf(writebacks, "\t\t*(%s) = %s_c2f(temp_%s);\n", arg_name, mpi_type->cpre, arg_name);
            }
          }
        } else {
          /* Non-mpi pointer/array types can be passed straight through. */
          call += sprintf(call, "%s", arg_name);
          mpich_call += sprintf(mpich_call, "%s", arg_name);
        }
      }
    }
    call += sprintf (call, ");\n");
    mpich_call += sprintf (mpich_call, ");\n");


    /* Now that we've generated code for the delegation, put it all together. */
    fprintf(wrapperFile, "\t{\n");
    if (strcmp(mpich_call_buf, call_buf) != 0) {
      /* If calls were different, then there's some special treatment necessary for MPI types if we're
         not using MPICH.  */
      fprintf(wrapperFile, "#if (defined(MPICH_NAME) && (MPICH_NAME == 1)) /* MPICH test */\n");
      fprintf(wrapperFile, "%s", mpich_call_buf);
      fprintf(wrapperFile, "#else /* other mpi's need conversions */\n");
      fprintf(wrapperFile, "%s%s%s%s", local_buf, copy_buf, call_buf, write_buf);
      fprintf(wrapperFile, "#endif /* MPICH test */\n");
    } else {
      /* If both calls are the same, locals, copies, and writebacks are empty, so we can avoid 
         ifdef nastiness. */
      fprintf(wrapperFile, "%s", call_buf);
    }
    fprintf(wrapperFile, "\t\t*ierr = rc;\n");
    fprintf(wrapperFile, "\t}\n");
}


/**
 * Prints out parameters for a delegation call from a C routine TO a C wrapper routine.
 * This passes arguments straight through, with no special processing as for Fortran.
 */
void
PrintAllCParams (void)
{
    int i;
    fprintf (wrapperFile, "pc");
    for (i = 1; i <= argCounter && args[i].type.type != ELLIPSIS; i++) {
      fprintf (wrapperFile, ",\n\t\t\t%s", args[i].name);
    }
    return;
}


void
RecordCWrapper (char *name)
{
    int i;
    char buf[4096];

    PrintFormalArg (buf, &(args[0]));
    fprintf (wrapperFile, "\n%s%s( ", buf, name);
    for (i = 1; i <= argCounter; i++) {
        PrintFormalArg (buf, &(args[i]));
        if (i == 1) {
            fprintf (wrapperFile, " %s", buf);
        }
        else {
            fprintf (wrapperFile, ",\n\t\t%s", buf);
        }
    }
    fprintf (wrapperFile, ")\n");
    fprintf (wrapperFile, "{\n");
    fprintf (wrapperFile, "\tint rc = 0;\n");
    fprintf (wrapperFile, "\tvoid *pc = 0x0;\n");
    fprintf (wrapperFile, "\tGET_FP;\n");
    fprintf (wrapperFile, "\t//DEFINE_PC;\n");
    fprintf (wrapperFile, "\t//GATHER_PC;\n");

    fprintf (wrapperFile, "\trc = gwrap_%s(", name);
    PrintAllCParams();
    fprintf (wrapperFile, " );\n");

    /* finish */
    fprintf (wrapperFile, "\treturn rc;\n");
    fprintf (wrapperFile, "}\n");

    return;
}


void
RecordFortranWrapper (char *name)
{
    int i;
    char lcname[128];		  /* lower case version of the name */
    char ucname[128];		  /* upper case version of the name */

    char formal_buf[4096] = "\0";    /* temp array for formal params. */
    char actual_buf[4096] = "\0";    /* temp array for actual params. */
    char *formals = formal_buf;      /* For printing. */
    char *actuals = actual_buf;      /* For printing. */

    for (i = 0; name[i]; i++) {
        lcname[i] = tolower(name[i]);
        ucname[i] = toupper(name[i]);
    }
    lcname[i] = ucname[i] = '\0';

    /* Skip params for MPI_Init(), since fortran version takes only ierr */
    if(strcmp(lcname, "mpi_init") != 0) {
      /* Create formal arg list. */
      for (i = 1; i <= argCounter && args[i].type.type != ELLIPSIS; i++) {
        formals += PrintFormalFortranArg(formals, &(args[i]));
        formals += sprintf(formals, ", ");
        actuals += sprintf(actuals, "%s, ", args[i].name);
      }
    }
    formals += sprintf(formals, "MPI_Fint * ierr");
    actuals += sprintf(actuals, "ierr");

    /* Print out the actual fortran wrapper function. */
    fprintf (wrapperFile, "\nstatic void %s_%s(%s) {\n", lcname, F_WRAP_SUFFIX, formal_buf);

#if defined(i386) || defined(__x86_64__)
    if(strcmp(lcname, "mpi_init") == 0) {
      fprintf (wrapperFile, "extern int xargc;\n");
      fprintf (wrapperFile, "extern char ** xargv;\n");
      fprintf (wrapperFile, "extern void parse_command_line();\n");
    }
#endif /* i36, x86_64 */
    fprintf(wrapperFile, "\tint rc;\n");
    fprintf(wrapperFile, "\tvoid *pc = 0x0;\n");

    /* mpi_init in fortran has 1 param - c has 3:
       this is a hack to fix it (mike noeth) */
    if(strstr(lcname, "mpi_init") != NULL) {
#if defined(i386) || defined(__x86_64__)
        fprintf (wrapperFile, "\tint *argc = &xargc;\n");
        fprintf (wrapperFile, "\tchar ***argv = &xargv;\n");
        fprintf (wrapperFile, "parse_command_line();\n");
#else
        fprintf (wrapperFile, "\tvoid *argc = (void *) ierr;\n");
        fprintf (wrapperFile, "\tvoid *argv = (void *) ierr;\n");
#endif
    }

    fprintf (wrapperFile, "\t// DEFINE_PC;\n");
    fprintf (wrapperFile, "\t// GATHER_PC;\n");
    DelegateFortranCall(name);
    fprintf (wrapperFile, "\treturn;\n");
    fprintf (wrapperFile, "}\n\n");

    /* Print delegating bindings for various types of fortran linkage. */
    fprintf (wrapperFile, "/* Fortran bindings delegate to wrapper above. */\n");
    fprintf (wrapperFile, "extern void %s(%s) { %s_%s(%s); }\n", 
             ucname, formal_buf, lcname, F_WRAP_SUFFIX, actual_buf);
    fprintf (wrapperFile, "extern void %s(%s) { %s_%s(%s); }\n", 
             lcname, formal_buf, lcname, F_WRAP_SUFFIX, actual_buf);
    fprintf (wrapperFile, "extern void %s_(%s) { %s_%s(%s); }\n", 
             lcname, formal_buf, lcname, F_WRAP_SUFFIX, actual_buf);
    fprintf (wrapperFile, "extern void %s__(%s) { %s_%s(%s); }\n", 
             lcname, formal_buf, lcname, F_WRAP_SUFFIX, actual_buf);

    return;
}


void
PrintAllGenericArgs (void)
{
    int i;
    char buf[4096];

    //Frank: 0 argument case cause "," -- should be omitted
    if (argCounter == 0)
        fprintf (wrapperFile, "\nvoid * pc");
    else
        fprintf (wrapperFile, "\nvoid * pc ,");
    for (i = 1; i <= argCounter; i++) {
        PrintFormalArg (buf, &(args[i]));
        if (i == 1) {
            fprintf (wrapperFile, " %s", buf);
        }
        else {
            fprintf (wrapperFile, ",\n%s", buf);
        }
    }
    fprintf (wrapperFile, ")\n");

    return;
}


int
RecordGenericOpStart (char *name, int warn_omit)
{
    int i;

    PrintAllGenericArgs();
    fprintf (wrapperFile, "\n{\nint rc = 0;\numpi_op_t *uop = ");
    fprintf (wrapperFile, "(umpi_op_t *)calloc(sizeof(umpi_op_t), 1);\n\n");
    fprintf (wrapperFile, "assert(uop);\n");
    fprintf (wrapperFile, "uop->op = umpi_%s;\n", name);
    fprintf (wrapperFile, "uop->rank = umpi_rank;\n");
    fputs ("uop->seq_num = umpi_task_seq_num++;\n", wrapperFile);
    fputs ("uop->pc = pc;\n", wrapperFile);

    /* for fortran, dereference the fortran required pointer */
    /* if it was more than just a pointer, hope this works... */
    for (i = 1; i <= argCounter; i++) {
        const char *arg_tname = NameForType(&args[i].type);

        if ((!(args[i].eNoSave)) && (strlen (args[i].name) > 0)) {
            /* handle "special" cases first */
            if (strcmp(args[i].name, "op_ptr") == 0) {
                fprintf (wrapperFile, "uop->data.%s.op = *(op_ptr);\n", nameStr);
            }
            else if (strcmp(args[i].name, "status") == 0) {
                /* this can break with Fortran but will often work */
                fprintf (wrapperFile, 
                         "uop->data.%s.%s_src = %s->MPI_SOURCE;\n", 
                         nameStr, args[i].name, args[i].name);
            }
            else if (strcmp(args[i].name, "array_of_statuses") == 0) {
                /* this can break with Fortran but will often work */
                fprintf (wrapperFile, 
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
                fprintf (wrapperFile, "uop->data.%s.%s = &(%s);\n",
                         nameStr, args[i].name, args[i].name);
            }
            else if ((args[i].type.t_pointers != 0) &&
                     (args[i].type.type == T_VOID)) {
                /* save the address of void pointers; usually what you want... */
                fprintf (wrapperFile, "uop->data.%s.%s = %s;\n",
                         nameStr, args[i].name, args[i].name);

            } else if (strstr(args[i].name, "array") != NULL) {
                /* save the array of values... */
                fprintf (wrapperFile, 
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
                    fprintf (wrapperFile, 
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
                    fprintf (wrapperFile, 
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
              fprintf(wrapperFile, "uop->profile_level = level;\n");

            } else if ((strstr(args[i].name, "retcoords") != NULL) ||
                     (strstr(args[i].name, "dimsp") != NULL)) {
                /* save the array of values... */
                fprintf (wrapperFile, 
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
                fprintf (wrapperFile, 
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
                fprintf (wrapperFile, 
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
                fprintf (wrapperFile, 
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
                fprintf (wrapperFile, 
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
                fprintf (wrapperFile, 
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
                fprintf(wrapperFile,
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
                fprintf(wrapperFile,
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
                fprintf (wrapperFile, 
                         "uop->data.%s.%s = *(%s);\n", 
                         nameStr, args[i].name, args[i].name);
            }
            else if (args[i].type.array[0] == 0) {
                fprintf (wrapperFile, "\t\tuop->data.%s.%s = %s;\n",
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
    fprintf (wrapperFile, "umpi_inc_ref_count (uop);\n");
    fprintf (wrapperFile, "#ifdef UMPI_DEBUG_LEVEL_2\n");
    fprintf (wrapperFile, "q_append (umpi_task_op_q, uop);\n");
    fprintf (wrapperFile, "#endif\n\n");

    return 0;
}


int
RecordImmediateLocalPreStart (char *name)
{
    /* record beginning of synchronous preop processing definition */
    fprintf (wrapperFile, 
             "\nstatic int"
             "\numpi_%s_%s_immediate_local_pre(umpi_op_t *op)\n", 
             nameStr, name);
    fprintf (wrapperFile, "{\n");
  
    return 0;
}


int
RecordImmediateLocalPre (char *name)
{
    int i, needed = 0;

    if (wrapper_pre[0]) {
        fprintf (stderr, 
                 "\n\t\tWARNING: set extrafields to pass mgr wrapper-pre data");
    
        needed = 1;
        RecordImmediateLocalPreStart (name);
    
        fprintf (wrapperFile, "\n{\n");
        for (i = 0; wrapper_pre[i]; i++) {
            fprintf (wrapperFile, "%s\n", wrapper_pre[i]);
        }
        fprintf (wrapperFile, "}\n");
    }

    /* do stuff related to chksum processing... */
    if ((AL_Test("nonblocking")) || (AL_Test("persistent_init"))) {
        /* for now, tracking requests is done just for chksum processing... */
        /* need to save for completion/request handle... */
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPreStart (name);
        }
    
        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_inc_ref_count (op);\n");
    
        if (AL_Test("persistent_init")) {
            /* need to save for start... */
            fprintf (wrapperFile, 
                     "q_append (umpi_task_persistent_init_op_q, op);\n");
        }
        else {
            /* need to save for completion... */
            fprintf (wrapperFile, "q_append (umpi_task_req_start_op_q, op);\n");
        }
    
        if (!AL_Test("persistent_start")) {
            if (!AL_Test("persistent_init")) {
                /* need storage to find completion op... */
                assert (!AL_Test("multiple_requests"));
                fprintf (wrapperFile, 
                         "op->completions = \n"
                         "(umpi_completion_t *) "
                         "calloc (1, sizeof(umpi_completion_t));\n");
            }

            /* also need to save for getting request handles... */
            fprintf (wrapperFile, "umpi_l_save_for_postop_processing(op);\n");

            /* also need to save pointer to type map... */
            fprintf (wrapperFile, "umpi_l_save_typemap_op(op);\n");
        }

        fprintf (wrapperFile, "#endif\n\n"); 
    }
    else if (AL_Test("send")) {
        /* need to save pointer to type map... */
        /* sendrecv's are weird so we do them within checksum routines... */
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPreStart (name);
        }
    
        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_l_save_typemap_op(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }

    /* get send buffer chksum BEFORE giving to MPI implementation... */
    if ((AL_Test("send")) && (!AL_Test("persistent_init"))) {
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPreStart (name);
        }

        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_get_chksum_send_pre(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
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
            RecordImmediateLocalPreStart (name);
        }

        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_get_chksum_persistent_start_pre(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }
    else if (AL_Test("request_free")) {
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPreStart (name);
        }

        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_get_chksum_request_free_pre(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }
    else if (AL_Test("completion")) {
        /* completed request handles get munged during operation so we */
        /* have to figure out which requests are completed in the pre... */
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPreStart (name);
        }

        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_get_chksum_completion_pre(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }

    if (strcmp (name, "MPI_Type_free") == 0) {
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPreStart (name);
        }

        // mike noeth - i striped this out because it was causing a linking error
        // most likely i just have to restore all the umpi_mpi_type* stuff
        // but didn't have time and removing this line worked out...
        //fprintf (wrapperFile, "umpi_l_free_type (op);\n");
    } 
    else if (strcmp (name, "MPI_Type_commit") == 0) {
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPreStart (name);
        }

        // Frank: see Mike's comment above
        //fprintf (wrapperFile, 
        //	     "umpi_mpi_l_save_typemap (op, op->data.mpi.newtype);\n");
    } 

    if (needed)
        fprintf (wrapperFile, "return 0;\n}\n\n");
    else
        AL_Set ("nolocalpre");

    return needed;
}


int
RecordDelayedLocalorGlobalPre (char *name)
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
RecordImmediateLocalPostStart (char *name)
{
    /* record beginning of synchronous postop processing definition */
    fprintf (wrapperFile, 
             "\nstatic int"
             "\numpi_%s_%s_immediate_local_post(umpi_op_t *op)\n", 
             nameStr, name);
    fprintf (wrapperFile, "{\n");

    return 0;
}


int
RecordImmediateLocalPost (char *name)
{
    int i, needed = 0;

    /* user/spec supplied immediate processing... */
    if (wrapper_post[0]) {
        fprintf (stderr, 
                 "\n\t\tWARNING: set extrafields to pass mgr wrapper-post data");
    
        needed = 1;
        RecordImmediateLocalPostStart (name);

        fprintf (wrapperFile, "\n{\n");
        for (i = 0; wrapper_post[i]; i++) {
            fprintf (wrapperFile, "%s\n", wrapper_post[i]);
        }
        fprintf (wrapperFile, "}\n");
    }

    if ((!AL_Test("persistent_start")) && 
        ((AL_Test("nonblocking")) || (AL_Test("persistent_init")))) {
        /* store handles in preop for chksum processing... */
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPostStart (name);
        }
    
        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_l_save_req_handle_from_postop (op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }

    /* get recv buffer chksum AFTER giving op to MPI implementation... */
    if (AL_Test("sendrecv")) {
        /* no reason to get the chksum now if it was nonblocking... */
        assert (!AL_Test("nonblocking"));

        if (!needed) {
            needed = 1;
            RecordImmediateLocalPostStart (name);
        }

        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_get_chksum_sendrecv_post(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }
    else if ((AL_Test("recv")) && (!AL_Test("nonblocking"))) {
        /* completed the receive so get chksum... */
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPostStart (name);
        }

        /* need to save pointer to type map... */
        /* sendrecv's are weird so we do them within checksum routines... */
        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_l_save_typemap_op(op);\n");

        fprintf (wrapperFile, "umpi_get_chksum_recv_post(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }
    else if ((AL_Test("completion")) && (!AL_Test("request_free"))) {
        /* compute the chksums and see if locally consistent... */
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPostStart (name);
        }

        fprintf (wrapperFile, "#ifndef UMPI_NOCHKSUM\n"); 
        fprintf (wrapperFile, "umpi_get_chksum_completion_post(op);\n");
        fprintf (wrapperFile, "#endif\n\n"); 
    }

    /* get the lrank_to_grank translation when needed... */
    if ((AL_Test("comm_constructor")) &&
        (strcmp (name, "MPI_Comm_dup") != 0) &&
        (strcmp (name, "MPI_Intercomm_create") != 0)) {
        if (!needed) {
            needed = 1;
            RecordImmediateLocalPostStart (name);
        }

        fprintf (wrapperFile, 
                 "umpi_package_lranks_to_granks (op, op->data.%s.comm_out);\n",
                 nameStr);
    }

    if (needed)
        fprintf (wrapperFile, "return 0;\n}\n\n");
    else
        AL_Set ("nolocalpost");

    return needed;
}


int
RecordDelayedLocalorGlobalPost (char *name)
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
RecordGenericPre (char *name)
{
    int need_immediate_local = RecordImmediateLocalPre (name);
    int need_delayed_local_or_global = RecordDelayedLocalorGlobalPre (name);

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
        fprintf (wrapperFile, 
                 "\n/*------------ Needed preop processing omitted */\n");

        return 0;
    }

    /* record beginning of preop processing definition */
    fprintf (wrapperFile, 
             "\nstatic int\numpi_%s_%s_pre( ", nameStr, name);
    RecordGenericOpStart (name, 1);
    fprintf (wrapperFile, "uop->order = umpi_preop;\n");

    /* ##### if necessary, do any processing before make actual MPI call */
    if (need_immediate_local) {
        if (AL_Test ("nolocalpre")) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: "
                     "Omitting needed synchronous preop processing for %s!", name);

            /* record warning comment... */
            fprintf (wrapperFile, 
                     "\n/*------ Needed synchronous preop processing omitted */\n");
        }
        else {
            fprintf (wrapperFile, 
                     "\nrc = umpi_%s_%s_immediate_local_pre(uop);\n", 
                     nameStr, name);
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
            fprintf (wrapperFile, 
                     "\n/*----- Needed asynchronous preop processing omitted */\n");
        }
        else {
            fprintf (wrapperFile, "rc |= umpi_Asynch_op(uop);\n");
        }
    }

    /* give up uop reservation for wrapper processing... */
    fprintf (wrapperFile, 
             "umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);\n");

    fprintf (wrapperFile, "return 0;\n}\n");

    return 1;
}


int
RecordGenericPost (char *name, int need_pre)
{
    int need_immediate_local = RecordImmediateLocalPost (name);
    int need_delayed_local_or_global = RecordDelayedLocalorGlobalPost (name);

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
        fprintf (wrapperFile, 
                 "\n/*------------ Needed postop processing omitted */\n");

        return 0;
    }

    /* record beginning of postop processing definition */
    fprintf (wrapperFile, 
             "\nstatic int\numpi_%s_%s_post( \nint MPI_rc, ", nameStr, name);
    RecordGenericOpStart (name, !need_pre);
    fprintf (wrapperFile, "uop->order = umpi_postop;\n");
    fputs ("uop->res = MPI_rc;\n", wrapperFile);

    /* ##### if necessary, do any processing before make actual MPI call */
    if (need_immediate_local) {
        if (AL_Test ("nolocalpost")) {
            /* print warning */
            fprintf (stderr, 
                     "\n\t\tWARNING: "
                     "Omitting needed synchronous postop processing for %s!", name);

            /* record warning comment... */
            fprintf (wrapperFile, 
                     "\n/*----- Needed synchronous postop processing omitted */\n");
        }
        else {
            fprintf (wrapperFile, 
                     "\nrc = umpi_%s_%s_immediate_local_post(uop);\n", 
                     nameStr, name);
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
            fprintf (wrapperFile, 
                     "\n/*---- Needed asynchronous postop processing omitted */\n");
        }
        else {
            fprintf (wrapperFile, "rc |= umpi_Asynch_op(uop);\n");
        }
    }

    /* give up uop reservation for wrapper processing... */
    fprintf (wrapperFile, 
             "umpi_dec_ref_count (uop, UMPI_TASK_SYNCHRONOUS);\n");

    fprintf (wrapperFile, "return rc;\n}\n");

    return 1;
}

void MakePMPICall(char *name) {
  int i;

  /* make actual MPI call through profiling interface... */
  fprintf (wrapperFile, "\trc = P%s( ", name);
  for (i = 1; i <= argCounter && args[i].type.type != ELLIPSIS; i++) {
    char aname[256];
    if ((args[i].type.t_pointers == 0) && (args[i].type.array[0] == 0)) {
      sprintf(aname, "(%s)", args[i].name);
    }
    else {
      sprintf(aname, "(%s)", args[i].name);
    }
    if (i == 1) {
      fprintf (wrapperFile, " %s", aname);
    }
    else {
      fprintf (wrapperFile, ",\n%s", aname);
    }
  }
  fprintf (wrapperFile, ");\n");
}


void
RecordOpenWrapperGuard(char *name) {
  /* This guards against reentering wrappers from inside mpi calls. */
  fprintf (wrapperFile, "\n#ifndef NO_REENTRY_GUARD");
  fprintf (wrapperFile, "\nif (in_wrapper) {");

  MakePMPICall(name);
  fprintf (wrapperFile, "\n    return rc;");

  fprintf (wrapperFile, "\n}");
  fprintf (wrapperFile, "\nin_wrapper=1;");
  fprintf (wrapperFile, "\n#endif /* NO_REENTRY_GUARD */\n");
}

void
RecordCloseWrapperGuard() {
  /* Closing brace for reentry guard. */
  fprintf (wrapperFile, "\n#ifndef NO_REENTRY_GUARD");
  fprintf (wrapperFile, "\nin_wrapper=0;");
  fprintf (wrapperFile, "\n#endif /* NO_REENTRY_GUARD */\n");
}


void
RecordGenericWrapper (char *name)
{
    int i;
    int need_pre = 0;
    int need_post = 0;

    /* record delimiting comment... */
    fprintf (wrapperFile, 
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
        fprintf (wrapperFile, 
                 "\n/*-------------------- Wrapper for %s omitted */\n", name);

        return;
    }

    /* Wrapper not disabled for this function... generate it. */
    
    /* check if we need pre- and post-routines */
    need_pre = RecordGenericPre (name);
    need_post = RecordGenericPost (name, need_pre);
    
    /* no wrapper needed if we don't need either a preop or a postop... */
    if (!(need_pre || need_post)) {
      AL_Set ("nowrapper");
      
      /* record warning comment... */
      fprintf (wrapperFile, 
               "\n/*-------------------- Wrapper for %s omitted */\n", name);
      
      return;
    }

    /* record beginning of wrapper definition (name, args, etc...) */
    fprintf (wrapperFile, "\nstatic int\ngwrap_%s( ", name);
    PrintAllGenericArgs ();
    fprintf (wrapperFile, "{\nint rc;\n");
    
    /* For functions that have status as a parameter, need to check if it's set to 
       MPI_STATUS_IGNORE.  If it is, we need to use a temporary status instead, so that
       internal functions can still use the status param for their own purposes. */
    if (HasParameterWithName("status")) {
      fprintf (wrapperFile, "\tMPI_Status temp_status;\n");
      fprintf (wrapperFile, "\tif (status == MPI_STATUS_IGNORE) {\n");
      fprintf (wrapperFile, "\t\tstatus = &temp_status;\n");
      fprintf (wrapperFile, "\t}\n\n");
    }

    /* Similar check for functions with arrays of statuses -- need this in case we ignore. 
       Need to put a check at the end, too, since we will have to free the array. */
    int free_temp_statuses = 0;
    if (HasParameterWithName("array_of_statuses")) {
      fprintf (wrapperFile, "\tint free_temp_statuses = 0;\n");
      fprintf (wrapperFile, "\tif (array_of_statuses == MPI_STATUSES_IGNORE) {\n");
      // we always have "count" parameter if we have "array_of_statuses"
      fprintf (wrapperFile, "\t\tarray_of_statuses = (MPI_Status*)malloc(sizeof(MPI_Status) * count);\n");
      fprintf (wrapperFile, "\t\tfree_temp_statuses = 1;\n");
      fprintf (wrapperFile, "\t}\n\n");
      free_temp_statuses = 1;
    }

    RecordOpenWrapperGuard(name);

    /* ##### if necessary, package PRE op and connect to manager */
    if (need_pre) {
        assert (!AL_Test ("nopre"));

        /* Make the actual pre-wrapper call. */
        fprintf (wrapperFile, "\numpi_%s_%s_pre(", nameStr, name);
        PrintAllCParams();
        fprintf (wrapperFile, ");\n");
    }

    MakePMPICall(name);
    
    /* ##### if necessary, package POST op and connect to manager */
    if (need_post) {
        assert (!AL_Test ("nopost"));

        /* Make the actual post wrapper call. */
        fprintf (wrapperFile, "\numpi_%s_%s_post(rc, ", nameStr, name);
        PrintAllCParams();
        fprintf (wrapperFile, ");");
    }

    RecordCloseWrapperGuard();

    /* Free temporary status array if necessary. */
    if (free_temp_statuses) {
      fprintf (wrapperFile, "\tif (free_temp_statuses) {\n");
      fprintf (wrapperFile, "\t\tfree(array_of_statuses);\n");
      fprintf (wrapperFile, "\t}\n");
    }

    /* finish */
    fprintf (wrapperFile, "\treturn rc;\n");
    fprintf (wrapperFile, "}\n");
}


void
RecordWrapper (char *name) {

    if (AL_Test ("posix_function")) {
      fprintf(stdout, "POOOOOOOOOOOOOOOOOOOOOOOOSIXXXXXXXXXXXXXXX\n");
      RecordPosixWrapper(name);
      return;
    }


    RecordGenericWrapper(name);

    if (AL_Test ("nowrapper")) {
        return;
    }
    
    RecordCWrapper (name);
    RecordFortranWrapper (name);

}

  
/* eof */
