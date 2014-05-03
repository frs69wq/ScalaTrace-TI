/*  * Additional C code  for parser */

#include <stdio.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"
#include "wrapper.h"


extern int tag;

static int firstTrip = 1;

void
Reset (void)
{
  int i;
  bzero ((void *) &args, sizeof (arg_t) * MAX_ARGS);
  bzero ((void *) &extraFields, sizeof (extraFields_t) * MAX_ARGS);
  argCounter = 0;
  extraFieldCounter = 0;
  AL_ClrAll ();
  for (i = 0; i < 1024; i++)
    {
      if (vcode_g_pre[i] && !firstTrip)
	free (vcode_g_pre[i]);
      if (vcode_g_post[i] && !firstTrip)
	free (vcode_g_post[i]);
      if (vcode_l_pre[i] && !firstTrip)
	free (vcode_l_pre[i]);
      if (vcode_l_post[i] && !firstTrip)
	free (vcode_l_post[i]);
      if (wrapper_pre[i] && !firstTrip)
	free (wrapper_pre[i]);
      if (wrapper_post[i] && !firstTrip)
	free (wrapper_post[i]);
      vcode_g_pre[i] = NULL;
      vcode_g_post[i] = NULL;
      vcode_l_pre[i] = NULL;
      vcode_l_post[i] = NULL;
      wrapper_pre[i] = NULL;
      wrapper_post[i] = NULL;
    }
  vcode_g_pre_cnt = 0;
  vcode_g_post_cnt = 0;
  vcode_l_pre_cnt = 0;
  vcode_l_post_cnt = 0;
  wrapper_pre_cnt = 0;
  wrapper_post_cnt = 0;
  if (firstTrip)
    firstTrip = 0;
  return;
}


void
attr_action_print_bit (const void *i_node, VISIT order, int level)
{
  attrNode_t *rNode = *(attrNode_t **) i_node;
  switch (order)
    {
    case postorder:
    case leaf:
      {
	fprintf (lookupFile, ", %d", rNode->bit);
      }
      break;

    default:
      break;
    }

  return;
}


void
attr_action_print_name_for_struct (const void *i_node, VISIT order, int level)
{
  attrNode_t *rNode = *(attrNode_t **) i_node;
  switch (order)
    {
    case postorder:
    case leaf:
      {
	fprintf (structFile, "\tint %s;\n", rNode->name);
      }
      break;

    default:
      break;
    }

  return;
}

void
Abort (char *str)
{
  fprintf (stderr, "ERROR:%d:%s\nExiting!\n", lineno, str);
  exit (1);
}


/* ################################################## CheckConfig */
/* Check all the settings for the current prototype. Does it make
   sense? Crosscheck and update if necessary */

void CheckConfig(char *name)
{
  return ;
}


/* ################################################## Attribute mgt */

int
al_cmp_name (const void *n1, const void *n2)
{
  char *s1 = ((attrNode_t *) n1)->name;
  char *s2 = ((attrNode_t *) n2)->name;
  return (strcmp (s1, s2));
}

void *AL = NULL;

int
AL_Init ()
{
  AL = NULL;
  return 0;
}

int
AL_AddAttribute (char *name)
{
  attrNode_t searchNode;
  searchNode.name = name;
  fprintf(stdout, "Attribute being added is %s\n", name);
  if (NULL == tfind (&searchNode, &AL, al_cmp_name))
    {
      /* n/a, so insert */
      attrNode_t *aln = (attrNode_t *) malloc (sizeof (attrNode_t));
      bzero (aln, sizeof (attrNode_t));
      aln->name = strdup (name);
      aln->type = 0;
      aln->bit = 0;
      tsearch (aln, &AL, al_cmp_name);
    }
  else
    {
      Abort ("redefining attr is not allowed");
    }

  return 0;
}

int
AL_Set (char *name)
{
  attrNode_t searchNode;
  attrNode_t **resNode = NULL;
  searchNode.name = name;
  if (NULL != (resNode = (attrNode_t**)tfind (&searchNode, &AL, al_cmp_name)))
    {
      (*resNode)->bit = 1;
    }
  else
    {
      char buf[256];
      sprintf (buf, "attr [%s] not defined", name);
      Abort (buf);
    }

  return 0;
}

int
AL_Clr (char *name)
{
  attrNode_t searchNode;
  attrNode_t **resNode = NULL;
  searchNode.name = name;
  if (NULL != (resNode = (attrNode_t**)tfind (&searchNode, &AL, al_cmp_name)))
    {
      (*resNode)->bit = 0;
    }
  else
    {
      char buf[256];
      sprintf (buf, "attr [%s] not defined", name);
      Abort (buf);
    }

  return 0;
}

static void
al_action_clr (const void *i_node, VISIT order, int level)
{
  attrNode_t *rNode = *(attrNode_t **) i_node;
  switch (order)
    {
    case postorder:
    case leaf:
      {
	rNode->bit = 0;
      }
      break;

    default:
      break;
    }

  return;
}

int
AL_ClrAll ()
{
  twalk (AL, al_action_clr);

  return 0;
}

int 
AL_Test (char *name) 
{
  attrNode_t searchNode;
  attrNode_t **resNode = NULL;
  searchNode.name = name;
  if (NULL != (resNode = (attrNode_t**)tfind (&searchNode, &AL, al_cmp_name)))
    {
      return (*resNode)->bit != 0;
    }
  else
    {
      char buf[256];
      sprintf (buf, "attr [%s] not defined", name);
      Abort (buf);
    }

  return 0;
}

  
/* eof */
