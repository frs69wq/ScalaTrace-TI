/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Mon July 1 2002 */
/* mpi_print.c -- prints code to print specific Umpire ops */

#include <stdio.h>
#include <string.h>
#include <search.h>

#include "y.tab.h"
#include "wrapper.h"


void
RecordPrintOp(char *name)
{
  int i;
  fprintf (printHFile, "\nextern void umpi_print_op_%s(umpi_op_t*op,char *buf);",name);
  fprintf (printFile, "void umpi_print_op_%s(umpi_op_t*op,char *buf)\n{\n",name);
  fprintf (printFile, 
           "assert(op);\n"
           "assert(buf);\n"
           "sprintf(buf, \"%%d/%%d/%%\" PRIxPTR \": %%d %%s %%s \\n\", "
           "op->rank, op->seq_num, (intptr_t)op->pc,"
           "op->op, "
           "(get_struct_by_opcode(op->op))->name, "
           "((op->order==umpi_preop)?\"pre\":\"pos\"));\n");
  fprintf (printFile, "sprintf(buf+strlen(buf),"
           "\"\\tumpi_op_ref_count = %%i\\n\", op->umpi_op_ref_count);\n");

  for (i = 1; i <= argCounter; i++) {
    if ((!(args[i].eNoSave)) && (strlen (args[i].name) > 0)) {
      /* this can break with Fortran but will often work */
      if (strcmp(args[i].name, "op_ptr") == 0) {
        fprintf (printFile, "sprintf(buf+strlen(buf),"
                 "\"\\top = %%\" PRImpiPTR \"\\n\", (mpi_hval_t)op->data.%s.op);\n", 
                 nameStr);
      }
      if (strcmp(args[i].name, "status") == 0) {
        fprintf (printFile, "sprintf(buf+strlen(buf),"
                 "\"\\tstatus->MPI_SOURCE = %%i\\n\","
                 " op->data.%s.%s_src);\n", nameStr, args[i].name);
      }
      else if (strcmp(args[i].name, "array_of_statuses") == 0) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\tarray_of_statuses[].MPI_SOURCE = [ \");");
        fprintf (printFile, 
                 "{\n"
                 "int i;\n"
                 "for(i=0;i<op->data.%s.count;i++) {\n"
                 "sprintf(buf+strlen(buf), "
                 "\"%%d \", op->data.%s.statuses_srcs[i]);\n"
                 "}\n}\n"
                 "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                 nameStr, nameStr);
      }
      else if (strstr(args[i].name, "array") != NULL) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\t%s = [ \");", args[i].name);

        if ((strstr(args[i].name, "displacements") != NULL)) {
          fprintf (printFile, 
                   "{\n"
                   "int i;\n"
                   "for(i=0;i<op->data.%s.count;i++) {\n"
                   "sprintf(buf+strlen(buf), "
                   "\"%%\" PRIuPTR \" \", (MPI_Aint)op->data.%s.%s[i]);\n"
                   "}\n}\n"
                   "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                   nameStr, nameStr, args[i].name);

        } else if ((strstr(args[i].name, "requests") != NULL) ||
                   (strstr(args[i].name, "types") != NULL)) {
          fprintf (printFile, 
                   "{\n"
                   "int i;\n"
                   "for(i=0;i<op->data.%s.count;i++) {\n"
                   "sprintf(buf+strlen(buf), "
                   "\"%%\" PRImpiPTR \" \", (mpi_hval_t)op->data.%s.%s[i]);\n"
                   "}\n}\n"
                   "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                   nameStr, nameStr, args[i].name);
        } else {
          fprintf (printFile, 
                   "{\n"
                   "int i;\n"
                   "for(i=0;i<op->data.%s.count;i++) {\n"
                   "sprintf(buf+strlen(buf), "
                   "\"%%i \", op->data.%s.%s[i]);\n"
                   "}\n}\n"
                   "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                   nameStr, nameStr, args[i].name);
        }
      }
      else if ((strstr(args[i].name, "counts") != NULL) ||
               (strstr(args[i].name, "displs") != NULL) ||
               (strstr(args[i].name, "retcoords") != NULL) ||
               (strstr(args[i].name, "dimsp") != NULL)) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\t%s = [ \");", args[i].name);
        fprintf (printFile, 
                 "{\n"
                 "int i;\n"
                 "for(i=0;i<op->data.%s.size;i++) {\n"
                 "sprintf(buf+strlen(buf), "
                 "\"%%i \", op->data.%s.%s[i]);\n"
                 "}\n}\n"
                 "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                 nameStr, nameStr, args[i].name);
      }
      else if (strstr(args[i].name, "retedges") != NULL) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\t%s = [ \");", args[i].name);
        fprintf (printFile, 
                 "{\n"
                 "int i;\n"
                 "for(i=0;i<op->data.%s.edgecount;i++) {\n"
                 "sprintf(buf+strlen(buf), "
                 "\"%%i \", op->data.%s.%s[i]);\n"
                 "}\n}\n"
                 "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                 nameStr, nameStr, args[i].name);
      }
      else if (strstr(args[i].name, "edges") != NULL) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\t%s = [ \");", args[i].name);
        fprintf (printFile, 
                 "{\n"
                 "int i;\n"
                 "for(i=0;"
                 "i<op->data.%s.array_of_degrees[op->data.%s.count-1];"
                 "i++) {\n"
                 "sprintf(buf+strlen(buf), "
                 "\"%%i \", op->data.%s.%s[i]);\n"
                 "}\n}\n"
                 "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                 nameStr, nameStr, nameStr, args[i].name);
      }
      else if (strstr(args[i].name, "the_addresses") != NULL) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\t%s = [ \");", args[i].name);
        fprintf (printFile, 
                 "{\n"
                 "int i;\n"
                 "for(i=0;i<op->data.%s.addrcount;i++) {\n"
                 "sprintf(buf+strlen(buf), "
                 "\"%%\" PRIxPTR \" \", (MPI_Aint)op->data.%s.%s[i]);\n"
                 "}\n}\n"
                 "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                 nameStr, nameStr, args[i].name);
      }
      else if (strstr(args[i].name, "the_datatypes") != NULL) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\t%s = [ \");", args[i].name);
        fprintf (printFile, 
                 "{\n"
                 "int i;\n"
                 "for(i=0;i<op->data.%s.dtypecount;i++) {\n"
                 "sprintf(buf+strlen(buf), "
                 "\"%%\" PRImpiPTR \" \", (mpi_hval_t)op->data.%s.%s[i]);\n"
                 "}\n}\n"
                 "sprintf(buf+strlen(buf), \"]\\n\");\n\n", 
                 nameStr, nameStr, args[i].name);
      }
      else if (strstr(args[i].name, "ranges") != NULL) {
        fprintf (printFile, "\nsprintf(buf+strlen(buf),"
                 "\"\\t%s = \\n\");", args[i].name);
        fprintf (printFile, 
                 "{\n"
                 "int i, j;\n"
                 "for(i=0;i<op->data.%s.count;i++) {\n"
                 "sprintf(buf+strlen(buf), \"\\t\\t| \");\n"
                 "for(j=0;j<3;j++) {\n"
                 "sprintf(buf+strlen(buf), "
                 "\"%%i \", op->data.%s.%s[3*i+j]);\n"
                 "}\n"
                 "sprintf(buf+strlen(buf), \"|\\n\");\n\n" 
                 "}\n}\n",
                 nameStr, nameStr, args[i].name);
      }
      else if ((strstr(args[i].name, "buf") != NULL) ||
               (strcmp(args[i].name, "function") == 0)) {
        fprintf (printFile, "sprintf(buf+strlen(buf),"
                 "\"\\t%s = %%\" PRIuPTR \"\\n\", (intptr_t)op->data.%s.%s);\n", 
                 args[i].name, nameStr, args[i].name);
      }
      else if ((strcmp(NameForType(&args[i].type), "MPI_Status") != 0) &&
               ((args[i].type.type != T_INT) || (strstr(args[i].name, "stride") != NULL))
      ) {
        fprintf (printFile, "sprintf(buf+strlen(buf),"
                 "\"\\t%s = %%\" PRImpiPTR \"\\n\", (mpi_hval_t)op->data.%s.%s);\n", 
                 args[i].name, nameStr, args[i].name);
      }
      else if (strcmp(args[i].name, "c_status") == 0) {
        fprintf (printFile, "sprintf(buf+strlen(buf),"
                 "\"\\t%s = %%i\\n\", (int)op->data.%s.c_status.MPI_SOURCE);\n", 
                 args[i].name, nameStr);
      }
      else if ((args[i].type.t_pointers != 0) || (args[i].type.array[0] == 0)) {
        fprintf (printFile, "sprintf(buf+strlen(buf),"
                 "\"\\t%s = %%i\\n\", op->data.%s.%s);\n", 
                 args[i].name, nameStr, args[i].name);
      }
      else {
        /* print warning */
        fprintf (stderr, 
                 "\n\t\tWARNING: Omitting arg %s from print_op", 
                 args[i].name);
      }
    }
    else if (strlen (args[i].name) > 0) {
      /* print warning */
      fprintf (stderr, 
               "\n\t\tWARNING: Omitting arg %s from print_op (NoSave)", 
               args[i].name);
    }
  }

  for (i = 0; i < extraFieldCounter; i++) {
    fprintf (printFile, "sprintf(buf+strlen(buf),"
             "\"\\t%s = %%\" PRIuPTR \"\\n\", (intptr_t)op->data.%s.%s);\n", 
             (extraFields[i].name), nameStr, (extraFields[i].name));
  }

  fprintf (printFile, "return;\n}\n");

  return;
}

  
/* eof */
