/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 7 "pl-compiler.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
char *yytext; // for generating error message in yyerror


/*Prototypes*/

extern int yylex (void);


void yyerror(char *s);


/* Globals*/

char *final=NULL;

char *buffers[10000]; // maximal program length 10k
int buffer_top=0;
int lno=0;


/*Some utilities


These are not yet used all over the place, but should be. 
A quick glance:

MEMORY
_alloc will allocate a new buffer and register it in a NULL slot in buffers
_freeall will free all allocated buffers
TODO: initialize buffers to be NULL and remove all malloc and free all over the place




*/


char *_alloc(size_t n){
    buffers[buffer_top++] = (char *) malloc(n);
    // TODO: abort out of mem in both pointers and...
    printf("Allocated %d slot %d", n, buffer_top-1);
}

// replace needle with replacement in haystack, reallocating buffers as needed. reporting if new buffer
char *_str_replace(char *haystack, char *needle, char *replacement, int *replaced){
    char *p=strstr(haystack, needle);
    if (p != NULL){
        int len = strlen(haystack) + strlen(replacement); // could be tighter, but we dont need it tight
	char *ret = (char*) malloc(len);
	strncpy(ret,haystack, (int) (p - haystack));
	strcat(ret,replacement);
	strcat(ret,(char *) (p+strlen(needle)));
	*replaced = 1; // we did replace
	return ret;

    }else{
	*replaced=0;
        return haystack;
    }

}

// string replace in a new copy. allocates new copy, but not more.
char *str_replace(char *haystack, char *needle, char *replacement){
    if (haystack == NULL || needle == NULL || replacement == NULL)
      return NULL;
    // create a working copy
    char *working_copy = (char *) malloc(strlen(haystack) +1);
    strcpy(working_copy,haystack); 
    int replaced=1;
    while (replaced){
        char *p = _str_replace(working_copy, needle, replacement, &replaced);
	if (replaced){
	    // new buffer allocated, take ownership
	    free(working_copy);
	    working_copy = p; 
	}
    }
    return working_copy;
}


/*
Immediate Code Generation Routines

In contrast to advanced compilers, the Precursor Language Compiler can generate its assembly code
directly. Therefore, all functions with prefix cg_ are used to generate code for named symbol.
*/


/*
Code Generation for Function Calls
- intrinstics move, turn, pick, deposit, return translate directly
- everything else is a CALL
*/

char *cg_call(char *name){
   assert(name != NULL);
   
   char *ret = malloc(1024);
   if (strcmp(name, "move") == 0)
   {
      snprintf(ret,10124,"MOVE\n");
   }else if (strcmp(name, "turn") == 0)
   {
      snprintf(ret,1024, "TURN\n");
   }else if (strcmp(name, "pick") == 0)
   {
      snprintf(ret,1024, "PICK\n");
   }else if (strcmp(name, "deposit") == 0)
   {
      snprintf(ret,1024, "DEPOSIT\n");
   }else if (strcmp(name, "return") == 0)
   {
      snprintf(ret,1024, "RET\n");
   }else
      snprintf(ret,1024, "CALL L%s\n", name);
   return ret;
}

/*
Code Generation for Sequential Execution

Not much mroe than concatenate of assembly
*/

char *cg_sequential(char *left, char *right){
   assert(left != NULL && right != NULL);
   int len = strlen(left)+strlen(right)+1;
   char *ret = malloc(len+1);
   snprintf(ret,len, "%s%s", left,right);
   return ret;
}

/*
SENSOR INSTRUCTIONS

Expressions can only contain identifiers. This is called from
such expressions. 

Emit LOADFB or LOADHI to read the two sensors.
*/

char *cg_loadport(char *id){
  char *ret = (char*)malloc(100);
  if (strcmp(id, "front_blocked")== 0) { // special var  
      strncpy(ret, "LOADFB\n",100);
   }else if (strcmp(id, "has_item")== 0) { // special var  
      strncpy(ret, "LOADHI\n",100);
   }else{
      printf("Unknown identifier: %s\n",id);
      exit(-5);
   }
   return ret;
}

/*
Emit a while loop
  while true: omit the condition entirely and use JMP
  while(exp): compute exp and use conditional jumps

  replace breaks and continues in the body with JMP to the right label. 
*/

char *cg_while(char *exp, char *body){
   assert(exp != NULL && body != NULL);
   int start_label,end_label;
   int len = 1024+strlen(exp)+strlen(body);
   char *ret = malloc(len);
   if (strcmp(exp, "true")== 0) { // special in pl1
       start_label = lno++;
       end_label = lno++;
      snprintf(ret,len, "L%d:\n%sJMP L%d\nL%d:\n", start_label,body, start_label,end_label);
   }else{
     // exp is code that evaluates and sets the CPU flags
     // as we do not have numbers in Niki, logical ops just need to store the result in the cpu flag
     // as we finally need to negate it for the jump, we should add a NOT, but we flip the jump
      start_label = lno++;
      end_label = lno++;
      snprintf(ret,len, "L%d:\n%sJZ L%d\n%sJMP L%d\nL%d:\n", start_label,exp,end_label,body, start_label,end_label);
    }

   
   /*IF THERE IS A BREAK, replace it with JMP */
   char jumpcommand[15];
   char *ret2;
   snprintf(jumpcommand,15,"JMP L%d",end_label);
   ret2 = str_replace(ret, "BREAK",jumpcommand);
   free(ret);
   ret = ret2;
   /*IF THERE IS A CONT, replace it with JMP */
   snprintf(jumpcommand,15,"JMP L%d",start_label);
   ret2 = str_replace(ret, "CONT",jumpcommand);
   free(ret);
   ret = ret2;

   return ret;
}

/*
Code generation for if without else

Executing the assembly of the expression loads one of the sensors...
*/

char *cg_if (char *exp, char *body){
   assert(exp!= NULL && body != NULL);
   int len=30+strlen(exp)+strlen(body);
   char *ret = malloc(len);
   snprintf(ret,len,"%sJZ L%d\n%sL%d:\n",exp,lno++,body,lno);
   return ret;
}
/*
Code generation for if with else

Executing the assembly of the expression loads one of the sensors...

Implementation sketch
    (exp)
    JZ LE
       (body)
       JMP OUT
    LE
       (else)
    LOUT

*/


char *cg_if_else (char *exp, char *body, char *elsecode){
   assert(exp!= NULL && body != NULL && elsecode != NULL);
   int len=60+strlen(exp)+strlen(body)+strlen(elsecode);
   
   char *ret = malloc(len);
   int label_else = lno++;
   int label_out = lno++;
   snprintf(ret,len,"%sJZ L%d\n%sJMP L%d\nL%d:\n%sL%d:\n",exp,label_else,body,label_out,label_else,elsecode,label_out );
   return ret;
}

/* 
code generation for sequences of function implementations

just concantenate assembly in the global variable final.
*/
void cg_append(char *s){
   int len = strlen(s)+10;
   if (final != NULL)
     len += strlen(final);
   char *ret = (char *) malloc(len);
   ret[0] = '\0'; // empty
   if (final != NULL)
     strcpy(ret, final);
   strcat(ret,s);
   free(final);
   final = ret; // new buffer
}
/*
Code generation for function implementation
Emit label, body, RET

*/
char *cg_function(char *name, char *body)
{
     printf("Implemented CG_FUNCTION for name %s\n",name, body);
     int len = 20+strlen(name)+strlen(body);
     char *ret = malloc(len);
     snprintf(ret,len,"L%s:\n%sRET\n",name,body);
     return ret;
}

/*
Linking Stage


*/
int link_labelsunique(){
   printf("Linker Aspect - Labels Unique not yet implemented\n");
   return 1;
}
int link_resolvecalls(){
   printf("Linker Aspect - Resolving Symbols not implemented\n");
   return 1;
}





#line 366 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    TRUE = 259,                    /* TRUE  */
    INT = 260,                     /* INT  */
    VOID = 261,                    /* VOID  */
    UINT = 262,                    /* UINT  */
    WHILE = 263,                   /* WHILE  */
    IF = 264,                      /* IF  */
    ELSE = 265,                    /* ELSE  */
    SWITCH = 266,                  /* SWITCH  */
    CASE = 267,                    /* CASE  */
    BREAK = 268,                   /* BREAK  */
    DEFAULT = 269,                 /* DEFAULT  */
    CONTINUE = 270,                /* CONTINUE  */
    RETURN = 271,                  /* RETURN  */
    NUM = 272,                     /* NUM  */
    INCLUDE = 273,                 /* INCLUDE  */
    FALSE = 274,                   /* FALSE  */
    ASGN = 275,                    /* ASGN  */
    LOR = 276,                     /* LOR  */
    LAND = 277,                    /* LAND  */
    BOR = 278,                     /* BOR  */
    BXOR = 279,                    /* BXOR  */
    BAND = 280,                    /* BAND  */
    EQ = 281,                      /* EQ  */
    NE = 282,                      /* NE  */
    LE = 283,                      /* LE  */
    GE = 284,                      /* GE  */
    LT = 285,                      /* LT  */
    GT = 286,                      /* GT  */
    IFX = 287,                     /* IFX  */
    IFX1 = 288                     /* IFX1  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define ID 258
#define TRUE 259
#define INT 260
#define VOID 261
#define UINT 262
#define WHILE 263
#define IF 264
#define ELSE 265
#define SWITCH 266
#define CASE 267
#define BREAK 268
#define DEFAULT 269
#define CONTINUE 270
#define RETURN 271
#define NUM 272
#define INCLUDE 273
#define FALSE 274
#define ASGN 275
#define LOR 276
#define LAND 277
#define BOR 278
#define BXOR 279
#define BAND 280
#define EQ 281
#define NE 282
#define LE 283
#define GE 284
#define LT 285
#define GT 286
#define IFX 287
#define IFX1 288

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 303 "pl-compiler.y"

int ival;
double dval;
char *name;

#line 488 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);


/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_TRUE = 4,                       /* TRUE  */
  YYSYMBOL_INT = 5,                        /* INT  */
  YYSYMBOL_VOID = 6,                       /* VOID  */
  YYSYMBOL_UINT = 7,                       /* UINT  */
  YYSYMBOL_WHILE = 8,                      /* WHILE  */
  YYSYMBOL_IF = 9,                         /* IF  */
  YYSYMBOL_ELSE = 10,                      /* ELSE  */
  YYSYMBOL_SWITCH = 11,                    /* SWITCH  */
  YYSYMBOL_CASE = 12,                      /* CASE  */
  YYSYMBOL_BREAK = 13,                     /* BREAK  */
  YYSYMBOL_DEFAULT = 14,                   /* DEFAULT  */
  YYSYMBOL_CONTINUE = 15,                  /* CONTINUE  */
  YYSYMBOL_RETURN = 16,                    /* RETURN  */
  YYSYMBOL_NUM = 17,                       /* NUM  */
  YYSYMBOL_INCLUDE = 18,                   /* INCLUDE  */
  YYSYMBOL_FALSE = 19,                     /* FALSE  */
  YYSYMBOL_ASGN = 20,                      /* ASGN  */
  YYSYMBOL_LOR = 21,                       /* LOR  */
  YYSYMBOL_LAND = 22,                      /* LAND  */
  YYSYMBOL_BOR = 23,                       /* BOR  */
  YYSYMBOL_BXOR = 24,                      /* BXOR  */
  YYSYMBOL_BAND = 25,                      /* BAND  */
  YYSYMBOL_EQ = 26,                        /* EQ  */
  YYSYMBOL_NE = 27,                        /* NE  */
  YYSYMBOL_LE = 28,                        /* LE  */
  YYSYMBOL_GE = 29,                        /* GE  */
  YYSYMBOL_LT = 30,                        /* LT  */
  YYSYMBOL_GT = 31,                        /* GT  */
  YYSYMBOL_32_ = 32,                       /* '+'  */
  YYSYMBOL_33_ = 33,                       /* '-'  */
  YYSYMBOL_34_ = 34,                       /* '*'  */
  YYSYMBOL_35_ = 35,                       /* '/'  */
  YYSYMBOL_36_ = 36,                       /* '@'  */
  YYSYMBOL_37_ = 37,                       /* '~'  */
  YYSYMBOL_IFX = 38,                       /* IFX  */
  YYSYMBOL_IFX1 = 39,                      /* IFX1  */
  YYSYMBOL_40_ = 40,                       /* '('  */
  YYSYMBOL_41_ = 41,                       /* ')'  */
  YYSYMBOL_42_ = 42,                       /* '{'  */
  YYSYMBOL_43_ = 43,                       /* '}'  */
  YYSYMBOL_44_ = 44,                       /* ';'  */
  YYSYMBOL_YYACCEPT = 45,                  /* $accept  */
  YYSYMBOL_pgmstart = 46,                  /* pgmstart  */
  YYSYMBOL_function = 47,                  /* function  */
  YYSYMBOL_STMTS = 48,                     /* STMTS  */
  YYSYMBOL_STMT1 = 49,                     /* STMT1  */
  YYSYMBOL_STMT = 50,                      /* STMT  */
  YYSYMBOL_EXP = 51,                       /* EXP  */
  YYSYMBOL_STMT_IF = 52,                   /* STMT_IF  */
  YYSYMBOL_STMT_WHILE = 53,                /* STMT_WHILE  */
  YYSYMBOL_WHILEBODY = 54,                 /* WHILEBODY  */
  YYSYMBOL_STMT_CALL = 55,                 /* STMT_CALL  */
  YYSYMBOL_TYPE = 56                       /* TYPE  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   50

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  30
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  54

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   288


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      40,    41,    34,    32,     2,    33,     2,    35,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    44,
       2,     2,     2,     2,    36,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    42,     2,    43,    37,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    38,    39
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   336,   336,   337,   338,   341,   343,   344,   347,   348,
     349,   350,   351,   352,   353,   358,   359,   360,   364,   365,
     366,   367,   368,   369,   374,   376,   377,   382,   388,   389,
     390
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "TRUE", "INT",
  "VOID", "UINT", "WHILE", "IF", "ELSE", "SWITCH", "CASE", "BREAK",
  "DEFAULT", "CONTINUE", "RETURN", "NUM", "INCLUDE", "FALSE", "ASGN",
  "LOR", "LAND", "BOR", "BXOR", "BAND", "EQ", "NE", "LE", "GE", "LT", "GT",
  "'+'", "'-'", "'*'", "'/'", "'@'", "'~'", "IFX", "IFX1", "'('", "')'",
  "'{'", "'}'", "';'", "$accept", "pgmstart", "function", "STMTS", "STMT1",
  "STMT", "EXP", "STMT_IF", "STMT_WHILE", "WHILEBODY", "STMT_CALL", "TYPE", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,    43,    45,    42,    47,    64,   126,   287,   288,
      40,    41,   123,   125,    59
};
#endif

#define YYPACT_NINF (-35)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      22,   -35,   -35,   -35,     3,    22,     1,   -35,   -35,   -33,
     -30,   -34,     6,   -35,   -24,   -22,   -20,   -10,    -9,    -8,
     -35,    -6,     6,   -35,   -35,   -35,    -1,    -2,    -2,   -35,
     -35,   -35,   -35,   -35,     0,   -35,   -35,   -35,     2,     4,
     -35,    -3,    -3,   -35,   -35,   -35,    28,    32,    -3,    -3,
     -35,   -35,   -35,   -35
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    28,    29,    30,     0,     2,     0,     1,     3,     0,
       0,     0,     7,     4,     0,     0,     0,     0,     0,     0,
      14,     0,     7,     8,     9,    10,     0,     0,     0,    11,
      13,    12,     5,     6,     0,    15,    16,    17,     0,     0,
      27,     0,     0,    25,    26,    24,    22,    23,     0,     0,
      20,    19,    21,    18
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -35,    41,   -35,   -18,    25,   -16,    20,   -35,   -35,   -35,
     -35,   -35
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,    13,    21,    22,    38,    23,    24,    45,
      25,     6
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      14,    35,    36,     7,     9,    15,    16,    10,    12,    14,
      17,    11,    18,    19,    15,    16,    26,    37,    27,    17,
      28,    18,    19,    43,    46,    44,    47,     1,     2,     3,
      50,    52,    51,    53,    29,    30,    31,    32,    48,    12,
      34,    20,    49,    41,    40,    42,     8,    33,    39,     0,
      20
};

static const yytype_int8 yycheck[] =
{
       3,     3,     4,     0,     3,     8,     9,    40,    42,     3,
      13,    41,    15,    16,     8,     9,    40,    19,    40,    13,
      40,    15,    16,    41,    42,    41,    42,     5,     6,     7,
      48,    49,    48,    49,    44,    44,    44,    43,    10,    42,
      41,    44,    10,    41,    44,    41,     5,    22,    28,    -1,
      44
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     5,     6,     7,    46,    47,    56,     0,    46,     3,
      40,    41,    42,    48,     3,     8,     9,    13,    15,    16,
      44,    49,    50,    52,    53,    55,    40,    40,    40,    44,
      44,    44,    43,    49,    41,     3,     4,    19,    51,    51,
      44,    41,    41,    48,    50,    54,    48,    50,    10,    10,
      48,    50,    48,    50
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    45,    46,    46,    47,    48,    49,    49,    50,    50,
      50,    50,    50,    50,    50,    51,    51,    51,    52,    52,
      52,    52,    52,    52,    53,    54,    54,    55,    56,    56,
      56
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     5,     3,     2,     0,     1,     1,
       1,     2,     2,     2,     1,     1,     1,     1,     7,     7,
       7,     7,     5,     5,     5,     1,     1,     4,     1,     1,
       1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* pgmstart: function  */
#line 336 "pl-compiler.y"
                                                    {cg_append((yyvsp[0].name));}
#line 1562 "y.tab.c"
    break;

  case 3: /* pgmstart: function pgmstart  */
#line 337 "pl-compiler.y"
                                                    {cg_append((yyvsp[-1].name));}
#line 1568 "y.tab.c"
    break;

  case 4: /* function: TYPE ID '(' ')' STMTS  */
#line 338 "pl-compiler.y"
                                                        { (yyval.name)=cg_function((yyvsp[-3].name),(yyvsp[0].name));}
#line 1574 "y.tab.c"
    break;

  case 5: /* STMTS: '{' STMT1 '}'  */
#line 341 "pl-compiler.y"
                       {(yyval.name)=(yyvsp[-1].name);}
#line 1580 "y.tab.c"
    break;

  case 6: /* STMT1: STMT STMT1  */
#line 343 "pl-compiler.y"
                                      {(yyval.name)=cg_sequential((yyvsp[-1].name),(yyvsp[0].name));}
#line 1586 "y.tab.c"
    break;

  case 7: /* STMT1: %empty  */
#line 344 "pl-compiler.y"
                                  {(yyval.name)=strdup("");}
#line 1592 "y.tab.c"
    break;

  case 8: /* STMT: STMT_IF  */
#line 347 "pl-compiler.y"
                                           {(yyval.name) = (yyvsp[0].name);}
#line 1598 "y.tab.c"
    break;

  case 9: /* STMT: STMT_WHILE  */
#line 348 "pl-compiler.y"
                                              {(yyval.name)=(yyvsp[0].name);}
#line 1604 "y.tab.c"
    break;

  case 10: /* STMT: STMT_CALL  */
#line 349 "pl-compiler.y"
                                             {(yyval.name) = (yyvsp[0].name);}
#line 1610 "y.tab.c"
    break;

  case 11: /* STMT: BREAK ';'  */
#line 350 "pl-compiler.y"
                                             {(yyval.name)=strdup("BREAK\n");}
#line 1616 "y.tab.c"
    break;

  case 12: /* STMT: RETURN ';'  */
#line 351 "pl-compiler.y"
                                              {(yyval.name)=strdup("RET\n");}
#line 1622 "y.tab.c"
    break;

  case 13: /* STMT: CONTINUE ';'  */
#line 352 "pl-compiler.y"
                                                {(yyval.name)=strdup("CONT\n");}
#line 1628 "y.tab.c"
    break;

  case 14: /* STMT: ';'  */
#line 353 "pl-compiler.y"
                                       {(yyval.name)= strdup("");}
#line 1634 "y.tab.c"
    break;

  case 15: /* EXP: ID  */
#line 358 "pl-compiler.y"
                                 {(yyval.name) = cg_loadport((yyvsp[0].name));}
#line 1640 "y.tab.c"
    break;

  case 16: /* EXP: TRUE  */
#line 359 "pl-compiler.y"
                                   {(yyval.name)=strdup("true");}
#line 1646 "y.tab.c"
    break;

  case 17: /* EXP: FALSE  */
#line 360 "pl-compiler.y"
                                      {(yyval.name)=strdup("false");}
#line 1652 "y.tab.c"
    break;

  case 18: /* STMT_IF: IF '(' EXP ')' STMT ELSE STMT  */
#line 364 "pl-compiler.y"
                                                                {(yyval.name) = cg_if_else((yyvsp[-4].name),(yyvsp[-2].name),(yyvsp[0].name));}
#line 1658 "y.tab.c"
    break;

  case 19: /* STMT_IF: IF '(' EXP ')' STMTS ELSE STMT  */
#line 365 "pl-compiler.y"
                                                                  {(yyval.name) = cg_if_else((yyvsp[-4].name),(yyvsp[-2].name),(yyvsp[0].name));}
#line 1664 "y.tab.c"
    break;

  case 20: /* STMT_IF: IF '(' EXP ')' STMTS ELSE STMTS  */
#line 366 "pl-compiler.y"
                                                                   {(yyval.name) = cg_if_else((yyvsp[-4].name),(yyvsp[-2].name),(yyvsp[0].name));}
#line 1670 "y.tab.c"
    break;

  case 21: /* STMT_IF: IF '(' EXP ')' STMT ELSE STMTS  */
#line 367 "pl-compiler.y"
                                                                  {(yyval.name) = cg_if_else((yyvsp[-4].name),(yyvsp[-2].name),(yyvsp[0].name));}
#line 1676 "y.tab.c"
    break;

  case 22: /* STMT_IF: IF '(' EXP ')' STMTS  */
#line 368 "pl-compiler.y"
                                                         {(yyval.name) = cg_if((yyvsp[-2].name),(yyvsp[0].name));}
#line 1682 "y.tab.c"
    break;

  case 23: /* STMT_IF: IF '(' EXP ')' STMT  */
#line 369 "pl-compiler.y"
                                                       {(yyval.name) = cg_if((yyvsp[-2].name),(yyvsp[0].name));}
#line 1688 "y.tab.c"
    break;

  case 24: /* STMT_WHILE: WHILE '(' EXP ')' WHILEBODY  */
#line 374 "pl-compiler.y"
                                                       { (yyval.name) = cg_while((yyvsp[-2].name),(yyvsp[0].name));}
#line 1694 "y.tab.c"
    break;

  case 25: /* WHILEBODY: STMTS  */
#line 376 "pl-compiler.y"
                                { (yyval.name) = (yyvsp[0].name); }
#line 1700 "y.tab.c"
    break;

  case 26: /* WHILEBODY: STMT  */
#line 377 "pl-compiler.y"
                                        {(yyval.name) = (yyvsp[0].name);}
#line 1706 "y.tab.c"
    break;

  case 27: /* STMT_CALL: ID '(' ')' ';'  */
#line 382 "pl-compiler.y"
                                        { 
					(yyval.name)=cg_call((yyvsp[-3].name));
                                     }
#line 1714 "y.tab.c"
    break;


#line 1718 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 393 "pl-compiler.y"


#include <ctype.h>
#include"lex.yy.c"
int count=0;

char st[1000][10];
int top=0;
int i=0;
char temp[2]="t";

int label[200];
int lnum=0;
int ltop=0;
int switch_stack[1000];
int stop=0;
char type[10];
struct Table
{
	char id[20];
	char type[10];
}table[10000];
int tableCount=0;



int main(int argc, char *argv[])
{
	yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
            perror("File open");
            exit(-1);
        }

	// prepare the final with a boot loader
	final = malloc(100);
	strcpy(final,"CALL Lmain\nHALT\n"); // boot loader
	
       if(!yyparse())
		printf("\nParsing complete\n");
	else
	{
		printf("\nParsing failed\n");
		exit(0);

	}
	// Enter Linker Stage: we do not link together multiple files, but we should do the minimum
	// sanity check of linkers:
	/*
	    make sure all labels are unique
            make sure all functions called do exist

	    a real linker would add the bootloader, but we did already in the initialization of the output
	*/

	if (
	   !link_labelsunique() ||
	   !link_resolvecalls())
	   {
	      printf("Linker Error. Exiting without output");
	       exit (-1);
           }
	

	printf("Parsing result***********************\n");
	if (final == NULL){
	  printf("NULL pointer\n");
	  }else{
	     FILE *f = fopen("output.nasm","w");
	     fprintf(f,"%s\n",final);
	     fclose(f);
	     printf("%s\n",final);
	  }
	
	fclose(yyin);
    return 0;
}
         

void yyerror(char *s)
{
	printf("Syntex Error in line number : %d : %s %s\n", yylineno, s, yytext );
}


