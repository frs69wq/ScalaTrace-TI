/* -*- Mode: C -*- */
/* jeffrey vetter */
/* parser.y --  parser */

/* C-declarations  */
%{

#define YYDEBUG 1
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "wrapper.h"

/* Some declarations for yacc */
 extern int yyerror(char *err);
 extern int yylex();

arg_t args[MAX_ARGS];		/* arg[0] is the function return type */
int argCounter = 0;
char * pre = NULL;
char * post = NULL;
int extraFieldCounter = 0;
extraFields_t extraFields[MAX_ARGS];
char * vcode_g_pre[1024];
char * vcode_g_post[1024];
char * vcode_l_pre[1024];
char * vcode_l_post[1024];
char * wrapper_pre[1024];
char * wrapper_post[1024];
int vcode_g_pre_cnt = 0;
int vcode_g_post_cnt = 0;
int vcode_l_pre_cnt = 0;
int vcode_l_post_cnt = 0;
int wrapper_pre_cnt = 0;
int wrapper_post_cnt = 0;
char ** vcode_ptr = NULL;
int * vcode_cnt_ptr = NULL;

void Reset(void);
void Abort(char*);
%}

/* Bison Declarations */
%union
{
  char * s;
  long i;
  double r;
  /*  array_ele_t * a;*/
}

%token<i> ASNDIV
%token<i> ASNMINUS
%token<i> ASNMULT
%token<i> ASNOP
%token<i> ASNPLUS
%token<i> ASTERICK
%token<i> ATTR
%token<i> BOOT
%token<i> CLR
%token<i> COLON
%token<i> COMMA
%token<i> COMMENT
%token<i> DIV
%token<i> ELLIPSIS
%token<i> EQ
%token<i> EXTRAFIELD
%token<i> GE
%token<i> GT
%token<i> INCR
%token<i> INTEGER
%token<i> LBRACE
%token<i> LBRACKET
%token<i> LE
%token<i> LPAREN
%token<i> LT
%token<i> MINUS
%token<i> MOD
%token<i> NL
%token<i> NOSAVE
%token<i> PRE
%token<i> POST
%token<i> PERIOD
%token<i> PLUS
%token<i> POUND
%token<i> RBRACE
%token<i> RBRACKET 
%token<i> RPAREN
%token<i> SAVE
%token<i> SEMI
%token<i> SET
%token<i> T_CHAR
%token<i> T_CONST
%token<i> T_DOUBLE
%token<i> T_EXTERN
%token<i> T_INT
%token<i> T_STRING
%token<i> T_STRUCT
%token<i> T_VOID
%token<i> T_VOLATILE
%token<i> VERIFY
%token<i> VCODE
%token<i> VCODE_G_PRE_B
%token<i> VCODE_G_PRE_E
%token<i> VCODE_G_POST_B
%token<i> VCODE_G_POST_E
%token<i> VCODE_L_PRE_B
%token<i> VCODE_L_PRE_E
%token<i> VCODE_L_POST_B
%token<i> VCODE_L_POST_E
%token<i> WRAPPER_PRE_B
%token<i> WRAPPER_PRE_E
%token<i> WRAPPER_POST_B
%token<i> WRAPPER_POST_E
%token<r> DOUBLE
%token<s> ID
%token<s> QSTRING

%start  stmt_list

%%
/* 
 * Grammer Rules 
 */
stmt_list: 
	| stmt_list {Reset();} stmt
	;

stmt: type ID LPAREN full_arg_list RPAREN SEMI
{
  fprintf(stderr, "\n\t%s", $<s>2);
  CheckConfig($<s>2);
  RecordVerificationCode($<s>2);
  RecordWrapper($<s>2);
  RecordStruct($<s>2);
  RecordLookup($<s>2);
  RecordParams($<s>2);
  RecordPrintOp($<s>2);
  RecordReplayOp($<s>2);
  RecordPackOps($<s>2);
  tag++;
}
| type ID LPAREN full_arg_list RPAREN LBRACE directives RBRACE vcode_directives SEMI
{
  fprintf(stderr, "\n\t%s", $<s>2);
  CheckConfig($<s>2);
  RecordVerificationCode($<s>2);
  RecordWrapper($<s>2);
  RecordStruct($<s>2);
  RecordLookup($<s>2);
  RecordParams($<s>2);
  RecordPrintOp($<s>2);
  RecordReplayOp($<s>2);
  RecordPackOps($<s>2);
  tag++;
}
| BOOT LBRACE bdirectives RBRACE SEMI
{
  ;
}
;

vcode_directives: 
	| vcode_directives vcode_directive {;}
	;

vcode_directive: 
	  VCODE_G_PRE_B {vcode_ptr = vcode_g_pre;vcode_cnt_ptr = &vcode_g_pre_cnt;} vcodes VCODE_G_PRE_E 
	{
	  /*printf("Completed VCODE_G_PRE_B vcodes VCODE_G_PRE_E\n");*/
	}
	| VCODE_G_POST_B {vcode_ptr = vcode_g_post;vcode_cnt_ptr = &vcode_g_post_cnt;} vcodes VCODE_G_POST_E
	{
	  /*printf("Completed VCODE_G_POST_B vcodes VCODE_G_POST_E\n");*/
	}
	| VCODE_L_PRE_B {vcode_ptr = vcode_l_pre;vcode_cnt_ptr = &vcode_l_pre_cnt;} vcodes VCODE_L_PRE_E
	{
	  /*printf("Completed VCODE_L_PRE_B vcodes VCODE_L_PRE_E\n");*/
	}
	| VCODE_L_POST_B {vcode_ptr = vcode_l_post;vcode_cnt_ptr = &vcode_l_post_cnt;} vcodes VCODE_L_POST_E
	{
	  /*printf("Completed VCODE_L_POST_B vcodes VCODE_L_POST_E\n");*/
	}
	| WRAPPER_PRE_B {vcode_ptr = wrapper_pre;vcode_cnt_ptr = &wrapper_pre_cnt;} vcodes WRAPPER_PRE_E
	{
	  /*printf("Completed WRAPPER_PRE_B vcodes WRAPPER_PRE_E\n");*/
	}
	| WRAPPER_POST_B {vcode_ptr = wrapper_post;vcode_cnt_ptr = &wrapper_post_cnt;} vcodes WRAPPER_POST_E
	{
	/*printf("Completed WRAPPER_POST_B vcodes WRAPPER_POST_E\n");*/
	}
	;

vcodes: 
	| vcodes VCODE 
	{
	  /*fprintf(stderr,"\t\t[%s]\n", $<s>2);*/
	  vcode_ptr[*vcode_cnt_ptr] = strdup($<s>2);
	  (*vcode_cnt_ptr)++;
	}
	;

typemod:
	| T_EXTERN {args[argCounter].type.t_extern++;}
	| T_CONST {args[argCounter].type.t_const++;}
	| T_VOLATILE {args[argCounter].type.t_volatile++;}
	;

pointer:
	| ASTERICK {args[argCounter].type.t_pointers=1;}
	| ASTERICK ASTERICK {args[argCounter].type.t_pointers=2;}
	| ASTERICK ASTERICK ASTERICK {args[argCounter].type.t_pointers=3;}
	;

type:  typemod T_INT pointer 
	{
	  args[argCounter].type.type = T_INT;
	}
	| typemod T_DOUBLE  pointer 
	{
	  args[argCounter].type.type = T_DOUBLE;
	}
	| typemod T_VOID  pointer 
	{
	  args[argCounter].type.type = T_VOID;
	}
	| typemod T_CHAR pointer 
	{
	  args[argCounter].type.type = T_CHAR;
	}
	| typemod T_STRUCT ID pointer 
	{
	  args[argCounter].type.type = T_STRUCT;
	  strcpy(args[argCounter].type.name, $<s>3);
	}
	| typemod ID pointer 
	{
	  args[argCounter].type.type = T_USER;
	  strcpy(args[argCounter].type.name, $<s>2);
	  /*printf("User type [%s]\n",args[argCounter].type.name);fflush(stdout);*/
	}
	;

full_arg_list
  : arg_list
  | arg_list COMMA {argCounter++;} ELLIPSIS
    {
      args[argCounter].type.type = ELLIPSIS;
      args[argCounter].name[0] = '\0';
    } 
  ;

arg_list:
	| arg_list COMMA {argCounter++;} arg
	| {argCounter++;} arg
	;

arg: 	 type ID 
	{
	  strcpy(args[argCounter].name,$<s>2);
	}
	| type ID LBRACKET RBRACKET 
	{
	  args[argCounter].type.array[0] = -1;
	  strcpy(args[argCounter].name,$<s>2);
	}
	| type ID LBRACKET RBRACKET LBRACKET INTEGER RBRACKET 
	{
	  args[argCounter].type.array[0] = -1;
	  args[argCounter].type.array[2] = $<i>6;
	  strcpy(args[argCounter].name,$<s>2);
	}
	| type ID LBRACKET RBRACKET LBRACKET ID RBRACKET 
	{
	  strcpy(args[argCounter].name,$<s>2);
	}
	| type 
	{
	  ;
	}
	;

bdirectives: 
	| bdirectives bdirective
	;

bdirective: ATTR ID SEMI
	{
	  AL_AddAttribute($<s>2);
	}
	;

directives: 
	| directives directive
	;

directive: SAVE ID SEMI
	{
	  int i, done = 0;
	  for(i = 1; (i <= argCounter) || !done; i++)
	    {
	      if(!strcmp(args[i].name, $<s>2))
		{
		  args[i].eNoSave = 0;
		  done = 1;
		}
	    }
	  if(done == 0)
	    {
	      char buf[128];
	      sprintf(buf,"Couldn't find [%s] for directive", $<s>2);
	      Abort(buf);
	    }
	}
	| NOSAVE ID SEMI
	{
	  int i, done = 0;
	  for(i = 1; (i <= argCounter) || !done; i++)
	    {
	      if(!strcmp(args[i].name, $<s>2))
		{
		  args[i].eNoSave = 1;
		  done = 1;
		}
	    }
	  if(done == 0)
	    {
	      char buf[128];
	      sprintf(buf,"Couldn't find [%s] for directive", $<s>2);
	      Abort(buf);
	    }
	}
	| PRE QSTRING SEMI
	{
	  pre = strdup($<s>2);
	}
	| POST QSTRING SEMI
	{
	  post = strdup($<s>2);
	}
	| EXTRAFIELD QSTRING QSTRING SEMI
	{
	  strcpy(extraFields[extraFieldCounter].name, $<s>2);
	  strcpy(extraFields[extraFieldCounter].structtype, $<s>3);
	  extraFieldCounter++;
	}
	| SET ID SEMI
	{
	  AL_Set($<s>2);
	}
	| CLR ID SEMI
	{
	  AL_Clr($<s>2);
	}
	;
%%

/* see parser_util.c */

				 /* EOF */

