/* -*- Mode: C; -*- */

%{
/* $Id: lexer.lex,v 1.1 2012/04/02 05:12:10 xwu3 Exp $
 * Created: Jeff Vetter (vetter@lanl.gov) on Mon Jul 15 1996.
 */


/*#include "gtypes.h"*/
#include "y.tab.h"

#include <stdlib.h>		/* for the conver functs */
#include <string.h>

int lineno = 1;

%}

%option nounput

%s VERIFICATION_CODE

ws	[ \t]+
qstring	\"[^\"\n]*[\"\n]
nl	\n
real	-?(([0-9]*(\.)[0-9]+)([eE][-+]?[0-9]+)?)
integer	-?([0-9]+)
id	[a-zA-Z0-9_]+
comment \#.*

%%

{ws}		{;}
{nl}    	{lineno++;}
{comment}	{;}

^"verify-global-pre"                   	{BEGIN VERIFICATION_CODE; /*fprintf(stderr,"LEXER: [%s]\n",yytext);*/ return VCODE_G_PRE_B;}
<VERIFICATION_CODE>^"verify-global-pre"{nl}	{BEGIN INITIAL; return VCODE_G_PRE_E;}
<VERIFICATION_CODE>.* 			{yylval.s = (char*)strdup( yytext );/*fprintf(stderr,"LEXER: [%s] yytext[%s]\n",yylval.s,yytext);*/ return VCODE;}

^"verify-global-post"                     	{BEGIN VERIFICATION_CODE; return VCODE_G_POST_B;}
<VERIFICATION_CODE>^"verify-global-post"{nl}	{BEGIN INITIAL; return VCODE_G_POST_E;}

^"verify-local-pre"                     	{BEGIN VERIFICATION_CODE; return VCODE_L_PRE_B;}
<VERIFICATION_CODE>^"verify-local-pre"{nl}	{BEGIN INITIAL; return VCODE_L_PRE_E;}

^"verify-local-post"                     	{BEGIN VERIFICATION_CODE; return VCODE_L_POST_B;}
<VERIFICATION_CODE>^"verify-local-post"{nl}	{BEGIN INITIAL; return VCODE_L_POST_E;}

^"wrapper-pre"                     	{BEGIN VERIFICATION_CODE; return WRAPPER_PRE_B;}
<VERIFICATION_CODE>^"wrapper-pre"{nl}	{BEGIN INITIAL; return WRAPPER_PRE_E;}

^"wrapper-post"                     	{BEGIN VERIFICATION_CODE; return WRAPPER_POST_B;}
<VERIFICATION_CODE>^"wrapper-post"{nl}	{BEGIN INITIAL; return WRAPPER_POST_E;}



\.		{return PERIOD;}
\.\.\.		{return ELLIPSIS;}
\;		{return SEMI;}
\:		{return COLON;}

\%		{return MOD;}
\*		{return ASTERICK;}
\+		{return PLUS;}
\,		{return COMMA;}
\-		{return MINUS;}
\/		{return DIV;}

\=		{return ASNOP;}
\/\=		{return ASNDIV;}
\-\=		{return ASNMINUS;}
\*\=		{return ASNMULT;}
\+\=		{return ASNPLUS;}

\<		{return LT;}
\<\=		{return LE;}
\=\=		{return EQ;}
\>		{return GT;}
\>\=    	{return GE;}

\(		{return LPAREN;}
\)		{return RPAREN;}

\{		{return LBRACE;}
\}		{return RBRACE;}

\[		{return LBRACKET;}
\]		{return RBRACKET;}

void		{return T_VOID;}
double		{return T_DOUBLE;}
int		{return T_INT;}
char		{return T_CHAR;}
extern		{return T_EXTERN;}
const		{return T_CONST;}
volatile	{return T_VOLATILE;}
struct		{return T_STRUCT;}

save		{return SAVE;}
nosave		{return NOSAVE;}
pre 	  	{return PRE;}
post   		{return POST;}
extrafield	{return EXTRAFIELD;}
boot		{return BOOT;}
attr		{return ATTR;}
set		{return SET;}
clr		{return CLR;}

{qstring}	{
		  yylval.s = (char*)strdup( yytext + 1);
		  if(yylval.s[yyleng-2] != '"')
		    {
		      printf("Unterminated character string");
		    }
		  else
		    {
#if 0
		      yylval.s[yyleng-2] = ' '; /*remove close quote */
#endif
		      yylval.s[yyleng-2] = '\0'; /*remove close quote */
		    }
		  return QSTRING;
		}

{real}		{yylval.r = atof(yytext);return DOUBLE;}
{integer}	{yylval.i = atoi(yytext);return INTEGER;}
{id}		{yylval.s = (char*)strdup(yytext);return ID;}

.      		{ECHO;}

%%


int 
yywrap()
{
  return 1;
}

void
yyerror(char *s)
{
  printf("\nERROR:%d:%s at \"%s\"\n\n", lineno, s, yytext);
  exit(-1);
}

				 /* EOF */
