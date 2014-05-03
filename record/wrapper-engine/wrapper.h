
#include <assert.h>
#include <stdio.h>
#include <search.h>

typedef struct _type_t
{
  int type;
  char name[64];
  int t_const;
  int t_extern;
  int t_volatile;
  int t_pointers;
  int array[5];
}
type_t;

typedef struct _arg_t
{
  type_t type;
  char name[64];
  int eNoSave;
}
arg_t;

#define MAX_ARGS 16

typedef struct _extraFields_t
{
  char name[128];
  char structtype[128];
}
extraFields_t;

typedef struct paramNode
{
  char *name;
  char *type;
}
paramNode_t;

extern char *nameStr;


extern FILE *wrapperFile;    /* Fortran, C and generic wrappers */
extern FILE *opsFile;        /* enums for mpi operations */
extern FILE *structFile;     /* umpi_mpi_lookup_t definition */
extern FILE *lookupFile;     /* umpi_mpi_lookup declaration */
extern FILE *paramFile;      /* umpi_mpi_param_t definition */
extern FILE *mgrverifyFile;  /* mgrverify_op function definitions */
extern FILE *mgrverifyHFile; /* mgrverify_op function declarations */
extern FILE *outverifyFile;  /* outverify_op function definitions */
extern FILE *outverifyHFile; /* outverify_op function declarations */
extern FILE *printFile;      /* print_op function definitions */
extern FILE *printHFile;     /* print_op function declarations */
extern FILE *packFile;       /* pack_op function definitions */
extern FILE *packHFile;      /* pack_op function declarations */
extern FILE *unpackFile;     /* unpack_op function definitions */
extern FILE *unpackHFile;    /* unpack_op function declarations */
extern FILE *freeFile;       /* free_op function definitions */
extern FILE *freeHFile;      /* free_op function declarations */
extern FILE *copyFile;       /* copy_op function definitions */
extern FILE *copyHFile;      /* copy_op function declarations */
extern FILE *replayHFile;      /* replay_op function declarations */
extern FILE *replayFile;      /* replay_op function declarations */
extern FILE *opTableHFile;   /* Declaration of table listing valid params for each op. */
extern FILE *opTableCFile;   /* Table listing valid params for each op. */
extern FILE *posixWrapFile;  /* Posix wrappers */

/* Returns the name of the type as it should appear in C code. */
extern const char *NameForType(const type_t *type);

extern void RecordPosixWrapper (char *name);
extern void RecordWrapper (char *name);
extern void RecordCWrapper (char *name);
extern void RecordFortranWrapper (char *name);
extern void RecordLookup (char *name);
extern void RecordVerificationCode (char *name);
extern void RecordParams (char *name);
extern void RecordStruct (char *name);
extern void RecordPrintOp(char *name);
extern void RecordPackOps(char *name);
extern void RecordFreeAndCopyOps(char *name, int *needed);
extern void RecordTypeMatchOp (char *name, char *fname_buf);
extern void RecordLocalTypeMatchOp (char *name, char *fname_buf);
extern void RecordTypeResOp(char *name, char *fname_buf);
extern void CheckConfig (char *name);
extern void *paramKeywords;
extern int param_cmp_name(const void *a, const void *b);

/* True if an arg with this name has multiple values. */
extern int isMultivalue(const char *argName);

extern arg_t args[MAX_ARGS];	/* arg[0] is the function return type */
extern int argCounter;
extern int extraFieldCounter;
extern void Reset (void);
extern void Abort (char *);
extern void attr_action_print_bit (const void *i_node, 
				   VISIT order, int level);
extern int AL_Test(char *name);
extern int AL_Set(char *name);
extern int AL_AddAttribute(char *name);
extern int AL_Clr(char *name);
extern int AL_ClrAll();

extern int tag;

extern extraFields_t extraFields[MAX_ARGS];

extern int lineno;

typedef struct attrNode
{
  char *name;
  int type;
  int bit;			/* 0 or 1 */
}
attrNode_t;

extern void *AL;

extern char *vcode_g_pre[];
extern char *vcode_g_post[];
extern char *vcode_l_pre[];
extern char *vcode_l_post[];
extern char *wrapper_pre[];
extern char *wrapper_post[];
extern int vcode_g_pre_cnt;
extern int vcode_g_post_cnt;
extern int vcode_l_pre_cnt;
extern int vcode_l_post_cnt;
extern int wrapper_pre_cnt;
extern int wrapper_post_cnt;

/* this isn't a real token but I need it */
#define T_USER 1000

