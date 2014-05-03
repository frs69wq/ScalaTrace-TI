/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Mon July 1 2002 */
/* currently, mostly relocated Umpire prototype code by Jeffrey Vetter */
/* also has minor additions for other associated functions... */
/* mpi_lookup.c -- code to record lookup table entries */


#include <stdio.h>
#include <strings.h>
#include <search.h>

#include "y.tab.h"
#include "wrapper.h"


extern int tag;

int lookupCount = 0;


void
RecordLookup (char *name)
{
  int needed;
  char cbuf[4096];

  sprintf (cbuf, "\"%s\"", name);
  if (lookupCount > 0)
    {
      fprintf (lookupFile, ",\n{%-27s, %4d", cbuf, tag);
    }
  else
    {
      fprintf (lookupFile, "\n{%-27s, %4d", cbuf, tag);
    }

  twalk (AL, attr_action_print_bit);

  sprintf (cbuf, "umpi_vcode_%s_g_pre", name);
  fprintf (lookupFile, ", %s", 
	   (!(AL_Test("noasynchpre") ||
	      AL_Test("noasynchglobalpre"))) ?  cbuf: "NULL");
  sprintf (cbuf, "umpi_vcode_%s_g_post", name);
  fprintf (lookupFile, ", %s", 
	   (!(AL_Test("noasynchpost") ||
	      AL_Test("noasynchglobalpost"))) ? cbuf : "NULL");

  sprintf (cbuf, "umpi_vcode_%s_l_pre", name);
  fprintf (lookupFile, ", %s", 
	   (!(AL_Test("noasynchpre") ||
	      AL_Test("noasynchlocalpre"))) ? cbuf : "NULL");
  sprintf (cbuf, "umpi_vcode_%s_l_post", name);
  fprintf (lookupFile, ", %s", 
	   (!(AL_Test("noasynchpost") ||
	      AL_Test("noasynchlocalpost"))) ? cbuf : "NULL") ;

  sprintf (cbuf, "umpi_print_op_%s", name);
  fprintf (lookupFile, ", %s", cbuf);

  sprintf (cbuf, "umpi_pack_op_%s", name);
  fprintf (lookupFile, ", %s", cbuf);

  sprintf (cbuf, "umpi_pack_op_size_%s", name);
  fprintf (lookupFile, ", %s", cbuf);

  sprintf (cbuf, "umpi_unpack_op_%s", name);
  fprintf (lookupFile, ", %s", cbuf);

  RecordFreeAndCopyOps (name, &needed);
  sprintf (cbuf, "umpi_free_op_%s", name);
  fprintf (lookupFile, ", %s", (needed) ? cbuf : "NULL" );
  sprintf (cbuf, "umpi_copy_op_%s", name);
  fprintf (lookupFile, ", %s", (needed) ? cbuf : "NULL" );

  RecordTypeMatchOp (name, cbuf);
  fprintf (lookupFile, ", %s", cbuf);

  RecordLocalTypeMatchOp (name, cbuf);
  fprintf (lookupFile, ", %s", cbuf);

  RecordTypeResOp (name, cbuf);
  fprintf (lookupFile, ", %s", cbuf);

/*Adding call back function to be used by replay instead of big switch-case function*/
  sprintf (cbuf, "umpi_replay_op_%s", name);
  fprintf (lookupFile, ", %s", cbuf);

#if 0
  for (i = 1; i <= argCounter; i++)
    {
      if ((!(args[i].eNoSave)) && (strlen (args[i].name) > 0))
	{
	  if ((args[i].type.t_pointers == 0) && (args[i].type.array[0] == 0))
	    {
	      /*              printf("%s [%s]\n",name,args[i].name); */
	      fprintf (lookupFile, "\tint %s;\n", args[i].name);
	      counter++;
	    }
	}
    }
  for (i = 0; i < extraFieldCounter; i++)
    {
      fprintf (lookupFile, "\t%s %s;\n",
	       extraFields[i].structtype, extraFields[i].name);
    }
#endif

  fprintf (lookupFile, "}");
  lookupCount++;
}


void
RecordStruct (char *name)
{
  fprintf (opsFile, "\numpi_%s = %d,", name, tag);
  return;
}

  
/* eof */
