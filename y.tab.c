
# line 2 "blif.y"
/******************************************************************\ 
 *
 *
 * This is free software, licensed under the GNU Public License V2.
 * See the file COPYING for details.
 *
 * This code is provided as is, with no warranty implied or expressed.
 * Use at your own risk.  This software may contain bugs which could
 * cause it to malfunction in possibly dangerous ways.
 *  
 * Neither the author, nor any affiliates of the author are responsible for 
 * any damages caused by use of this software.
 *
 * Copyright:  Narayanan Krishnamurthy, Heling Yi, 1997
 *
\******************************************************************/


/******************************************************************\ 
 *
 * This code was developed as part of a project in the digital system 
 * simulation class (Spring 1997 ) taught by 
 * Dr. Nur Touba at The University of Texas at Austin.
 * The project is Path Delay Fault simulation. This file is a "yacc" 
 * file that describes the grammar of a BLIF file that has 
 * been mapped to a technology library containing only 2-input or less
 * combinational gates. The parser parses the BLIF files and calls
 * the Add_Gate() and Build_Ckt() routines in "parse_ckt.c"
\******************************************************************/

#include <stdio.h>
#include <string.h> 
#include "read_ckt.h"

/******************************************************************\
 * STAND_ALONE has to be defined if the parser has to be
 * run in a stand_alone mode with its own "main()" function
 * Otherwise, the function ParseAndBuildCkt() function
 * is called from the read_circuit() function in read_circuit.c
\******************************************************************/

#ifndef STAND_ALONE
int ParseAndBuildCkt(FILE *);
#endif

/******************************************************************\
 * Add_Gate() and Build_Ckt() are functions being called from
 * this yacc file. The functions are defined in "read_ckt.cc"
\******************************************************************/
/*extern  void Add_Gate(Gate_Info_t *);*/
/*extern void Build_Ckt(void);*/

/******************************************************************\
 * DEBUG_MESSAGES  has to be defined to generate debug messages
 * as the BLIF file is being parsed
\******************************************************************/
#ifdef DEBUG_MESSAGES
void PrintGateInfo(void);
#endif


extern char *nameBuf;
extern char *funcCover;

/******************************************************************\
 * NETLIST_DUMP has to be defined to generate the "Output_Netlist"
 * which is a parsed output of the input BLIF file
\******************************************************************/
#ifdef NETLIST_DUMP
FILE *GateFile;
void DumpGateToFile(FILE *, Gate_Info_t *);
#endif

Gate_Info_t		gateInfo; 

/******************************************************************\
 * The following variables and data_structures are defined for 
 * local use within the yacc file. Added an additional UNKNOWN
 * gate because while parsing the signal list of the gate the
 * the gate type is unknown until the cover of the function
 * is read.

 * Currently handles 2-ip AND, OR, NAND, NOR and 1-i/p INV,
 * 1-i/p BUF, constant 0 (GND) and constant 1 (VCC) nodes.
 * PI's and PO's are also handled as separate nodes.
\******************************************************************/
#define MAX_GATE_TYPES 11 

/******************************************************************\
 * Commented this out because Dr. Touba said that it was not necesary
 * consider a generic CONSTANT node but just a PO constant node.
struct Gate_Type_t {
	char *name;
	char *cover;
	int  total;
} gate_type[MAX_GATE_TYPES] = {
                        "AND",  "11 1", 0,
		      			"OR",   "1- 1", 0,
				       	"NAND", "0- 1", 0,
				        "NOR", 	"00 1", 0,
				      	"INV",  "0 1", 0,
				       	"BUF",  "1 1", 0,
				      	"CONSTANT_1", "1", 0,
				      	"CONSTANT_0", "", 0,
				      	"PI", 	"", 0,
				       	"PO", 	"", 0,
			                "UNKNOWN", "", 0
			    };
\******************************************************************/

struct Gate_Type_t {
	char *name;
	char *cover;
	int  total;
} gate_type[MAX_GATE_TYPES] = {
                        "AND",  "11 1", 0,
		      			"OR",   "1- 1", 0,
				       	"NAND", "0- 1", 0,
				        "NOR", 	"00 1", 0,
				      	"INV",  "0 1", 0,
				       	"BUF",  "1 1", 0,
				      	"PI", 	"", 0,
				       	"PO", 	"", 0,
				      	"PO_GND", "", 0,
				      	"PO_VCC", "1", 0,
			                "UNKNOWN", "", 0
			    };
Gate_Type_Enum		nodeKind = UNKNOWN;   
int			signalNum; 
char                    **signalArray;


# line 135 "blif.y"
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
 {
  int ival;
  char* sval;
} YYSTYPE;
# define BLIF_MODEL 257
# define BLIF_INPUTS 258
# define BLIF_OUTPUTS 259
# define BLIF_NAMES 260
# define BLIF_END 261
# define NAME 262
# define GATE_COVER 263
# define LINE_CONT 264
# define BLIF_SEQ 265
# define BLIF_FSM 266

#ifdef __STDC__
#include <stdlib.h>
#include <string.h>
#else
#include <malloc.h>
#include <memory.h>
#endif

#include <values.h>

#ifdef __cplusplus

#ifndef yyerror
	void yyerror(const char *);
#endif

#ifndef yylex
#ifdef __EXTERN_C__
	extern "C" { int yylex(void); }
#else
	int yylex(void);
#endif
#endif
	int yyparse(void);

#endif
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval;
YYSTYPE yyval;
typedef int yytabelem;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#if YYMAXDEPTH > 0
int yy_yys[YYMAXDEPTH], *yys = yy_yys;
YYSTYPE yy_yyv[YYMAXDEPTH], *yyv = yy_yyv;
#else	/* user does initial allocation */
int *yys;
YYSTYPE *yyv;
#endif
static int yymaxdepth = YYMAXDEPTH;
# define YYERRCODE 256

# line 517 "blif.y"

extern FILE *yyin;

#ifdef STAND_ALONE
main(int argc, char **argv)
{
	FILE	*file;
#else
int ParseAndBuildCkt(FILE *file)
{
#endif
#ifdef NETLIST_DUMP
	char    output_file[] = "Output_Netlist";
#endif

#ifdef STAND_ALONE
	if (argc == 2){
		file = fopen(argv[1], "r");
		if (!file) {
			fprintf(stderr, "could not open %s\n", argv[1]);
			exit(1);
		}
	}
	else{
	    printf("Usage::  parse <filename>\n");
	    exit(0);
        }
#endif
		yyin = file;
#ifdef NETLIST_DUMP
		GateFile = fopen(output_file,"wt");
		if (!GateFile) {
			fprintf(stderr, "could not open %s\n", output_file);
			exit(1);
		}
#endif
	while (!feof(yyin)) {
		yyparse();
	}
  	Build_Ckt();
	printf("Circuit Parsed and Built successfully\n");
#ifdef STAND_ALONE
        fclose(file);
#endif
#ifdef NETLIST_DUMP
	fclose(GateFile);
#endif
        return 1;
}

yyerror(char *s)
{
	fprintf(stderr, "%s\n", s);
}

#ifdef DEBUG_MESSAGES 

void PrintGateInfo(void)
{
 int i;

 printf("Gate Type %s\n", gate_type[gateInfo.GateType].name); 
 for (i=0; i<gateInfo.NumOfNode-1; i++)
   printf("i/p name %s\n", gateInfo.NameOfNode[i]);
 printf("o/p name %s\n", gateInfo.NameOfNode[i]);
}

#endif

#ifdef NETLIST_DUMP
void DumpGateToFile(FILE *file, Gate_Info_t *gate)
{
  int i;

  fprintf(file, "%d\n", gate->GateType);
  /*  fprintf(file, "%s\n", gate_type[gate->GateType].name); */
  fprintf(file, "%d\n", gate->NumOfNode);
  for (i=0; i<gate->NumOfNode; i++)
    fprintf(file, "%s\n", gate->NameOfNode[i]);
  
}
#endif

void read_circuit(FILE *ckt_file)
{
  ParseAndBuildCkt(ckt_file);
} 

static const yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 25
# define YYLAST 53
static const yytabelem yyact[]={

     4,    24,    33,    13,    30,    22,    24,    15,     5,     6,
    17,    22,    34,    15,     9,    17,    13,    10,    15,    15,
    15,    22,    31,    22,    27,    20,    11,     9,    10,    14,
     7,    19,     8,    21,     3,     2,    32,     1,    18,    16,
    12,     0,    18,    23,     0,    28,     0,    26,    25,     0,
     0,    28,    29 };
static const yytabelem yypact[]={

  -257,-10000000,  -231,-10000000,  -236,-10000000,-10000000,  -242,  -244,-10000000,
-10000000,-10000000,  -249,  -243,  -256,-10000000,  -255,  -243,  -256,  -237,
-10000000,-10000000,  -243,  -256,  -260,  -239,  -256,-10000000,-10000000,  -261,
-10000000,-10000000,  -251,-10000000,-10000000 };
static const yytabelem yypgo[]={

     0,    37,    29,    36,    35,    30,    32,    31,    34,    25,
    33 };
static const yytabelem yyr1[]={

     0,     1,     1,     1,     4,     5,     5,     5,     5,     6,
     6,     6,     6,     2,     2,     2,     7,     7,     9,    10,
     3,     3,     3,     8,     8 };
static const yytabelem yyr2[]={

     0,    11,    11,     3,     5,     7,     5,     5,     3,     7,
     5,     5,     3,     7,     5,     3,     4,     2,     2,     7,
     5,     3,     1,     3,     3 };
static const yytabelem yychk[]={

-10000000,    -1,    -4,    -8,   257,   265,   266,    -5,    -6,   258,
   259,   262,    -6,   258,    -2,   262,    -5,   259,    -2,    -7,
    -9,   -10,   260,    -2,   262,    -7,    -2,   261,    -9,    -2,
   264,   261,    -3,   263,   263 };
static const yytabelem yydef[]={

     0,    -2,     0,     3,     0,    23,    24,     0,     0,     8,
    12,     4,     0,     6,     7,    15,     0,    10,    11,     0,
    17,    18,     0,     5,    14,     0,     9,     1,    16,    22,
    13,     2,    19,    21,    20 };
typedef struct
#ifdef __cplusplus
	yytoktype
#endif
{ char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"BLIF_MODEL",	257,
	"BLIF_INPUTS",	258,
	"BLIF_OUTPUTS",	259,
	"BLIF_NAMES",	260,
	"BLIF_END",	261,
	"NAME",	262,
	"GATE_COVER",	263,
	"LINE_CONT",	264,
	"BLIF_SEQ",	265,
	"BLIF_FSM",	266,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"circuit_module : decl_model decl_ip_list decl_op_list commandlist BLIF_END",
	"circuit_module : decl_model decl_op_list decl_ip_list commandlist BLIF_END",
	"circuit_module : invalid_ckt",
	"decl_model : BLIF_MODEL NAME",
	"decl_ip_list : decl_ip_list BLIF_INPUTS signal_list",
	"decl_ip_list : decl_ip_list BLIF_INPUTS",
	"decl_ip_list : decl_ip_list signal_list",
	"decl_ip_list : BLIF_INPUTS",
	"decl_op_list : decl_op_list BLIF_OUTPUTS signal_list",
	"decl_op_list : decl_op_list BLIF_OUTPUTS",
	"decl_op_list : decl_op_list signal_list",
	"decl_op_list : BLIF_OUTPUTS",
	"signal_list : signal_list NAME LINE_CONT",
	"signal_list : signal_list NAME",
	"signal_list : NAME",
	"commandlist : commandlist command",
	"commandlist : command",
	"command : logic_gate",
	"logic_gate : BLIF_NAMES signal_list cover_list",
	"cover_list : cover_list GATE_COVER",
	"cover_list : GATE_COVER",
	"cover_list : /* empty */",
	"invalid_ckt : BLIF_SEQ",
	"invalid_ckt : BLIF_FSM",
};
#endif /* YYDEBUG */
# line	1 "/usr/ccs/bin/yaccpar"
/*
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#pragma ident	"@(#)yaccpar	6.14	97/01/16 SMI"

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#define YYNEW(type)	malloc(sizeof(type) * yynewmax)
#define YYCOPY(to, from, type) \
	(type *) memcpy(to, (char *) from, yymaxdepth * sizeof (type))
#define YYENLARGE( from, type) \
	(type *) realloc((char *) from, yynewmax * sizeof(type))
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-10000000)

/*
** global variables used by the parser
*/
YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



#ifdef YYNMBCHARS
#define YYLEX()		yycvtok(yylex())
/*
** yycvtok - return a token if i is a wchar_t value that exceeds 255.
**	If i<255, i itself is the token.  If i>255 but the neither 
**	of the 30th or 31st bit is on, i is already a token.
*/
#if defined(__STDC__) || defined(__cplusplus)
int yycvtok(int i)
#else
int yycvtok(i) int i;
#endif
{
	int first = 0;
	int last = YYNMBCHARS - 1;
	int mid;
	wchar_t j;

	if(i&0x60000000){/*Must convert to a token. */
		if( yymbchars[last].character < i ){
			return i;/*Giving up*/
		}
		while ((last>=first)&&(first>=0)) {/*Binary search loop*/
			mid = (first+last)/2;
			j = yymbchars[mid].character;
			if( j==i ){/*Found*/ 
				return yymbchars[mid].tvalue;
			}else if( j<i ){
				first = mid + 1;
			}else{
				last = mid -1;
			}
		}
		/*No entry in the table.*/
		return i;/* Giving up.*/
	}else{/* i is already a token. */
		return i;
	}
}
#else/*!YYNMBCHARS*/
#define YYLEX()		yylex()
#endif/*!YYNMBCHARS*/

/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
#if defined(__STDC__) || defined(__cplusplus)
int yyparse(void)
#else
int yyparse()
#endif
{
	register YYSTYPE *yypvt = 0;	/* top of value stack for $vars */

#if defined(__cplusplus) || defined(lint)
/*
	hacks to please C++ and lint - goto's inside
	switch should never be executed
*/
	static int __yaccpar_lint_hack__ = 0;
	switch (__yaccpar_lint_hack__)
	{
		case 1: goto yyerrlab;
		case 2: goto yynewstate;
	}
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

#if YYMAXDEPTH <= 0
	if (yymaxdepth <= 0)
	{
		if ((yymaxdepth = YYEXPAND(0)) <= 0)
		{
			yyerror("yacc initialization error");
			YYABORT;
		}
	}
#endif

	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */
	goto yystack;	/* moved from 6 lines above to here to please C++ */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			int yynewmax;
#ifdef YYEXPAND
			yynewmax = YYEXPAND(yymaxdepth);
#else
			yynewmax = 2 * yymaxdepth;	/* double table size */
			if (yymaxdepth == YYMAXDEPTH)	/* first time growth */
			{
				char *newyys = (char *)YYNEW(int);
				char *newyyv = (char *)YYNEW(YYSTYPE);
				if (newyys != 0 && newyyv != 0)
				{
					yys = YYCOPY(newyys, yys, int);
					yyv = YYCOPY(newyyv, yyv, YYSTYPE);
				}
				else
					yynewmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				yys = YYENLARGE(yys, int);
				yyv = YYENLARGE(yyv, YYSTYPE);
				if (yys == 0 || yyv == 0)
					yynewmax = 0;	/* failed */
			}
#endif
			if (yynewmax <= yymaxdepth)	/* tables not expanded */
			{
				yyerror( "yacc stack overflow" );
				YYABORT;
			}
			yymaxdepth = yynewmax;

			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register const int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
			skip_init:
				yynerrs++;
				/* FALLTHRU */
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 1:
# line 158 "blif.y"
{
#ifdef DEBUG_MESSAGES
		  printf("Circuit Model successfully parsed\n");
#endif 
		} break;
case 2:
# line 164 "blif.y"
{
#ifdef DEBUG_MESSAGES
		  printf("Circuit Model successfully parsed\n");
#endif 
		} break;
case 3:
# line 170 "blif.y"
{
#ifdef DEBUG_MESSAGES
		  printf("Not a Combinational Circuit\n");
#endif 
		} break;
case 4:
# line 178 "blif.y"
{ 
#ifdef DEBUG_MESSAGES
		  printf(".model %s parsed\n", yypvt[-0].sval); 
#endif
		} break;
case 5:
# line 186 "blif.y"
{
#ifdef DEBUG_MESSAGES
                 printf(".input parsed - 4\n");
#endif
		 gate_type[PI].total++;
	        } break;
case 6:
# line 193 "blif.y"
{
#ifdef DEBUG_MESSAGES
                 printf(".input parsed - 3\n");
#endif 
		 gate_type[PI].total++;
		} break;
case 7:
# line 200 "blif.y"
{
#ifdef DEBUG_MESSAGES
		 printf(".input + signal_list parsed\n");
		 printf("Total PI's %d\n", gate_type[PI].total);
#endif 
		 signalNum =0;
		 gateInfo.GateType = nodeKind = UNKNOWN;		} break;
case 8:
# line 208 "blif.y"
{ 
#ifdef DEBUG_MESSAGES
		  printf(".input parsed\n"); 
#endif 
		 gateInfo.GateType = nodeKind = PI;
		 gateInfo.NumOfNode = 1;
		 gate_type[PI].total = 0;
		} break;
case 9:
# line 220 "blif.y"
{
#ifdef DEBUG_MESSAGES
                 printf(".output parsed - 1\n");
#endif 
		} break;
case 10:
# line 226 "blif.y"
{
#ifdef DEBUG_MESSAGES
                 printf(".output parsed - 2\n");
#endif 
		} break;
case 11:
# line 232 "blif.y"
{
#ifdef DEBUG_MESSAGES
		 printf(".output + signal_list parsed\n");
		 printf("Total PO's %d\n", gate_type[PO].total);
#endif 
		 signalNum =0;
		 gateInfo.GateType = nodeKind = UNKNOWN;		} break;
case 12:
# line 240 "blif.y"
{ 
#ifdef DEBUG_MESSAGES
		 printf(".output parsed\n"); 
#endif 
		 gateInfo.GateType = nodeKind = PO;
		 gateInfo.NumOfNode = 1;
		 gate_type[PO].total = 0;
		} break;
case 13:
# line 252 "blif.y"
{
#ifdef DEBUG_MESSAGES
		  printf("2 or more signals with line_cont parsed\n %dnd signal%s\n", signalNum + 1, yypvt[-1].sval);

#endif 
		  yyval.sval = yypvt[-1].sval;
		if((gateInfo.GateType == PI) || (gateInfo.GateType == PO)){ 
		  gateInfo.NameOfNode = (char **) malloc(
				    gateInfo.NumOfNode*sizeof(char *));
		  gateInfo.NameOfNode[0] = yypvt[-1].sval;
		  gate_type[gateInfo.GateType].total++;
#ifdef NETLIST_DUMP
		  DumpGateToFile(GateFile, &gateInfo);
#endif
                       Add_Gate(&gateInfo); 
		        free(gateInfo.NameOfNode);
		}
		 else {
		   if((signalArray = (char **) realloc(signalArray, 
					   (signalNum+1)*sizeof(char *)))
		                    == NULL)
		      perror("Realloc failed on line_cont signalArray\n");
		   signalArray[signalNum] = yypvt[-1].sval;
		   signalNum++;
		 }
		} break;
case 14:
# line 279 "blif.y"
{
#ifdef DEBUG_MESSAGES
		  printf("2 or more signal names parsed\n %dnd signal %s\n", 
			 signalNum+1, yypvt[-0].sval);
#endif 
		  yyval.sval = yypvt[-0].sval;
		if((gateInfo.GateType == PI) || (gateInfo.GateType == PO)){ 
		  gateInfo.NameOfNode = (char **) malloc(
			   gateInfo.NumOfNode*sizeof(char *));
		  gateInfo.NameOfNode[0] = yypvt[-0].sval;
		  gate_type[gateInfo.GateType].total++;
#ifdef DEBUG_MESSAGES
		  PrintGateInfo();
#endif 
#ifdef NETLIST_DUMP
		  DumpGateToFile(GateFile, &gateInfo);		  
#endif
            Add_Gate(&gateInfo);
		   free(gateInfo.NameOfNode[0]);
		   free(gateInfo.NameOfNode);
		}
		 else {
		   int i;
#ifdef DEBUG_MESSAGES
		   printf("Before realloc\n");
		   for(i=0; i<signalNum; i++){
		     printf("%d signal: %s\n", i+1, 
			    signalArray[i]);
		   }
#endif 

		   if((signalArray = (char **) realloc(signalArray, 
					   (signalNum+1)*sizeof(char *))) 
		                                 == NULL)
		      perror("Realloc failed on signalArray\n");
		   signalArray[signalNum] = yypvt[-0].sval;
#ifdef DEBUG_MESSAGES
		   printf("After realloc\n");
		   for(i=0; i<signalNum+1; i++){
		     printf("%d signal: %s\n", i+1, 
			    signalArray[i]);
		   }
#endif 
		   signalNum++;
		 }
		} break;
case 15:
# line 326 "blif.y"
{
#ifdef DEBUG_MESSAGES
		 printf("signal_name parsed %s\n", yypvt[-0].sval);
#endif 
		 yyval.sval = yypvt[-0].sval;
		 if((gateInfo.GateType == PI) || (gateInfo.GateType == PO)){
		   gateInfo.NameOfNode = (char **) malloc(
				gateInfo.NumOfNode*sizeof(char *));
		   gateInfo.NameOfNode[0] = yypvt[-0].sval;
		   gate_type[gateInfo.GateType].total++;
#ifdef DEBUG_MESSAGES
		   PrintGateInfo();
#endif 
#ifdef NETLIST_DUMP
		   DumpGateToFile(GateFile, &gateInfo);		    
#endif
		     Add_Gate(&gateInfo); 
			 free(gateInfo.NameOfNode[0]);
		    free(gateInfo.NameOfNode);
		 }
		 else {  
#ifdef DEBUG_MESSAGES
		        printf("Gate Type %s\n", 
			       gate_type[gateInfo.GateType].name);
#endif 
		   if((signalArray = (char **) malloc((signalNum+1)*
				    sizeof(char *))) == NULL)
	                perror("malloc fail on signalArray\n");
		      else{
		 	signalArray[signalNum] = yypvt[-0].sval;
#ifdef DEBUG_MESSAGES
			printf("Signal %s\n", signalArray[signalNum]);
#endif 
		      }

		 }
		 signalNum = 1;
                } break;
case 19:
# line 375 "blif.y"
{
                if(signalNum == 1){
                   if( yypvt[-0].sval == (char *)0){
#ifdef DEBUG_MESSAGES
/*		       printf("CONSTANT 0 node %s\n", $2); */
		       printf("PO_GND node %s\n", yypvt[-1].sval);
#endif 
                		/* constant node 0 */
/*		         gateInfo.GateType = nodeKind = CONSTANT_0; */
		         gateInfo.GateType = nodeKind = PO_GND;
                   } 
                   else{
                    free(yypvt[-0].sval);  /* freeing the cover storage for "1" */
#ifdef DEBUG_MESSAGES
/*			printf("CONSTANT 1 node %s\n", $2); */
			printf("PO_VCC node %s\n", yypvt[-1].sval);
#endif 
                		/* constant node 1 */
/*		 	gateInfo.GateType = nodeKind = CONSTANT_1; */
		 	gateInfo.GateType = nodeKind = PO_VCC;
                   }
		 	 gateInfo.NumOfNode = 1;		 
		         gateInfo.NameOfNode = signalArray;
		   	 gate_type[gateInfo.GateType].total++;
#ifdef DEBUG_MESSAGES
		         PrintGateInfo();
#endif 
#ifdef NETLIST_DUMP
			 DumpGateToFile(GateFile, &gateInfo);   
#endif
                         Add_Gate(&gateInfo); 
		   	  free(gateInfo.NameOfNode[0]);
		   	  free(gateInfo.NameOfNode);
                }
                else 
                if(signalNum > 1){
                int i;
				if(nodeKind != NAND && nodeKind != OR)
                    free(yypvt[-0].sval);  /* freeing the cover storage */  
		 	gateInfo.NumOfNode = signalNum;		 
		   	gateInfo.NameOfNode = signalArray; 
		 	gateInfo.GateType = nodeKind;
			/* need to check for UNKNOWN gate and if it comes up
			 * may need to exit
			 */
#ifdef DEBUG_MESSAGES
			PrintGateInfo();
#endif 
#ifdef NETLIST_DUMP
			DumpGateToFile(GateFile, &gateInfo);
#endif
			if(signalNum > 3 || nodeKind == UNKNOWN){
				printf("Invalid Gate in Circuit\n");
				exit(-1);
			}
		    	  Add_Gate(&gateInfo); 

			  for(i=0; i<signalNum; i++){
		   		 free(gateInfo.NameOfNode[i]);
			  }
		   	 free(gateInfo.NameOfNode);
                }
#ifdef DEBUG_MESSAGES
		  printf("logic gate parsed\n");
#endif 
		  gateInfo.GateType = nodeKind = UNKNOWN;
		  signalNum = 0;
		  signalArray = NULL;

		} break;
case 20:
# line 449 "blif.y"
{
#ifdef DEBUG_MESSAGES
                  switch(nodeKind){
                    case OR:
		       printf("OR gate parsed\n%s\n%s\n", yypvt[-1].sval, yypvt[-0].sval);
                    break;
                    case NAND:
		       printf("NAND gate parsed\n%s\n%s\n", yypvt[-1].sval, yypvt[-0].sval);
                    break;
                  }
#endif 
                   free(yypvt[-1].sval);
                   free(yypvt[-0].sval);

		} break;
case 21:
# line 465 "blif.y"
{
		  int i;

		  yyval.sval = yypvt[-0].sval;

		  /***************************************************\ 
		   * check till only gates that actually have a cover
		   * such as 
		   *			.names [1]  or  .names [2] [4] [5]
 		   *			1 				11 1
		   * These gates are gates 0 (AND) to 5 (BUF) and
		   * node PO_VCC
		   * PO_GND appears as follows 
		   *			.names [1]  
		   *
		   * This is because we decide the logic gate by actually
		   * doing a table lookup (compare the cover with the
		   * one in the table to decide the nodeKind)
		   \***************************************************/
/*		  for(i=0; i<MAX_GATE_TYPES-4; i++){ */
		  for(i=0; i<MAX_GATE_TYPES-5; i++){
		  	if(!strcmp(funcCover, gate_type[i].cover)){
#ifdef DEBUG_MESSAGES
		   	    printf("%s gate %s\n", gate_type[i].name, yypvt[-0].sval);
#endif 
		   	    nodeKind = i;
		   	    break;
		  	}
		  }
		  if(i == MAX_GATE_TYPES - 5){ /* ie loop exits wo breaking */
		  	if(!strcmp(funcCover, gate_type[PO_VCC].cover)){
#ifdef DEBUG_MESSAGES
		   	    printf("%s gate %s\n", gate_type[i].name, yypvt[-0].sval);
#endif 
				nodeKind = PO_VCC;
			}
		  }
		} break;
case 22:
# line 504 "blif.y"
{ yyval.sval = (char *) 0; } break;
case 23:
# line 508 "blif.y"
{
					printf("Sequential ckt\n");
				} break;
case 24:
# line 512 "blif.y"
{
					printf("FSM ckt\n");
				} break;
# line	531 "/usr/ccs/bin/yaccpar"
	}
	goto yystack;		/* reset registers in driver code */
}

