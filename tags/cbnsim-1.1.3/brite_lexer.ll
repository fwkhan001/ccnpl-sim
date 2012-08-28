/* -*- C++ -*- */
%option noyywrap
%{
#include <cstring>
#include "brite_parser.h"

int lineno = 1;
int yyerror( char * s );
%}

%%



#.* { /* eat comment line */; }

	/* specific tokens */
^Topology:.* { return TOPOLOGY_LINE; }
^Model.* { return MODEL_LINE; }
^Nodes:.* { return NODES_LINE; }
^Edges:.* { return EDGES_LINE; }
AS_NODE { return TOK_AS_NODE; }
RT_NODE { return TOK_RT_NODE; }
E_AS { return TOK_E_AS; }
E_RT { return TOK_E_RT; }
U { return TOK_U; }

	/* common tokens */
-?[0-9]+ { yylval.ival = atoi( yytext ); return INT; }
[0-9]*\.[0-9]* { yylval.dval = atof( yytext ); return DOUBLE; }
[A-Za-z] { yylval.cval = *yytext; return CHAR; }
\n { lineno++; }

[ \t\r] ;

. { yyerror( "invalid character" ); }

%%

int yyerror( char * s )
{
  fprintf( stderr, "%d: %s at '%s'\n", lineno, s, yytext );
  return 0;
}
