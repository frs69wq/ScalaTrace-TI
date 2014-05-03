/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Mon July 1 2002 */
/* currently, just relocated prototype code by Jeffrey Vetter... */
/* mpi_params.c -- code to record MPI params in Umpire struct... */


#include <stdio.h>
#include <string.h>
#include <search.h>
#include <stdlib.h>

#include "y.tab.h"
#include "wrapper.h"


const char *const MULTIVALUE_PARAM_TYPES[] = {
  "array", "counts", "displs", "retcoords", "dimsp", "edges", "the_addresses", "the_datatypes", "ranges", "filename", "datarep",
  NULL   /* NULL TERMINATED!  Keep this last. */
};


int isMultivalue(const char *argName) {
  const char *const *type;
  /* For names that imply multiple vales, make them pointers. */
  for (type = MULTIVALUE_PARAM_TYPES; *type != NULL; type++) {
    if (strstr(argName, *type) != 0) return 1;
  }
  return 0;
}


int
param_cmp_name (const void *n1, const void *n2)
{
  char *s1 = ((paramNode_t *) n1)->name;
  char *s2 = ((paramNode_t *) n2)->name;
  return (strcmp (s1, s2));
}

void
RecordParams (char *name)
{
  int i;
  paramNode_t searchNode;
  paramNode_t *newNode = NULL;

  /* Formal MPI params */
  for (i = 1; i <= argCounter; i++)
  {
    if ((!(args[i].eNoSave)) && (strlen (args[i].name) > 0))
    {
      searchNode.name = args[i].name;
      if (NULL == tfind (&searchNode, &paramKeywords, param_cmp_name))
      {
        /* n/a, so insert */
        newNode = (paramNode_t *) malloc (sizeof (paramNode_t));
        assert(newNode);
        newNode->name = strdup (args[i].name);

        /* usually want the value or values but want the */
        /* address for void pointer (mostly buffers) */
        /* and "function" (handler, keyval stuff) args... */
        if (((args[i].type.t_pointers != 0) 
             && (args[i].type.type == T_VOID)) 
            || (strstr(args[i].name, "function") != 0)) 
        {
          newNode->type = "void*";

        } else {
          char type_name[4096];
          type_name[0] = '\0';

          strcat(type_name, NameForType(&args[i].type));

          if (isMultivalue(newNode->name)) {
              strcat(type_name, "*");  /* make it a pointer type */
          } 
          
          /* argv needs to be char ** */
          if (strcmp(newNode->name, "argv") == 0) {
            strcat(type_name, "**");
          }

          /* TODO: where does this need to be freed, if at all? */
          newNode->type = strdup(type_name);
        }
        tsearch (newNode, &paramKeywords, param_cmp_name);

      } else {
        /* TODO: check other info to make sure a match */
            
      }
    }
  }


  /* extra fields */
  for (i = 0; i < extraFieldCounter; i++)
  {
    searchNode.name = extraFields[i].name;
    if (NULL == tfind (&searchNode, &paramKeywords, param_cmp_name))
    {
      /* n/a, so insert */
      newNode = (paramNode_t *) malloc (sizeof (paramNode_t));
      assert (newNode);
      newNode->name = strdup (extraFields[i].name);
      newNode->type = strdup (extraFields[i].structtype);
      tsearch (newNode, &paramKeywords, param_cmp_name);
    }
    else
    {
      /* check other info to make sure a match */
	  
    }
  }

  return;
}

  
/* eof */
