/*
pl parser,
adapted for Computational Foundations in TUM by Martin Werner
changes: 2024 M. Werner
*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
extern char *yytext; // for generating error message in yyerror


/*Prototypes*/
 extern FILE *yyin;
extern int yylex (void);
 extern int yylineno;

 void yyrestart(FILE *);
void yyerror(char *s);
 int yywrap(){return(1);} // we do only compile a single file.


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
    char *ret = buffers[buffer_top++] = (char *) malloc(n);
    // TODO: abort out of mem in both pointers and...
    printf("Allocated %zd slot %d", n, buffer_top-1);
    return ret;
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




%}


%union{
int ival;
double dval;
char *name;
}

%token <name> ID TRUE


%token INT VOID UINT 
%token WHILE 
%token IF ELSE SWITCH CASE BREAK DEFAULT CONTINUE RETURN
%token NUM
%token INCLUDE
%token  FALSE
%right ASGN 
%left LOR
%left LAND
%left BOR
%left BXOR
%left BAND
%left EQ NE 
%left LE GE LT GT
%left '+' '-' 
%left '*' '/' '@'
%left '~'

%nonassoc IFX IFX1
%nonassoc ELSE

%type <name> STMT_CALL STMT STMT1 STMTS STMT_WHILE WHILEBODY STMT_IF EXP pgmstart function 

%%
pgmstart                        : function          {cg_append($1);} |
                                  function pgmstart {cg_append($1);};
function 			: TYPE ID '(' ')' STMTS { $$=cg_function($2,$5);}
				;

STMTS 	: '{' STMT1 '}'{$$=$2;} 
				;
STMT1			: STMT  STMT1 {$$=cg_sequential($1,$2);}
				| {$$=strdup("");}
				;

STMT 			:          STMT_IF {$$ = $1;}
				 | STMT_WHILE {$$=$1;}
				 | STMT_CALL {$$ = $1;}
			         | BREAK ';' {$$=strdup("BREAK\n");}
				 | RETURN ';' {$$=strdup("RET\n");}
				 | CONTINUE ';' {$$=strdup("CONT\n");}
				 | ';' {$$= strdup("");}
				;

				

EXP 			:     ID {$$ = cg_loadport($1);} |
                              TRUE {$$=strdup("true");}  
			      | FALSE {$$=strdup("false");};
				;


STMT_IF 			:IF '(' EXP ')'  STMT ELSE STMT {$$ = cg_if_else($3,$5,$7);}
				 |IF '(' EXP ')'  STMTS ELSE STMT {$$ = cg_if_else($3,$5,$7);}
				 |IF '(' EXP ')'  STMTS ELSE STMTS {$$ = cg_if_else($3,$5,$7);}
				 |IF '(' EXP ')'  STMT ELSE STMTS {$$ = cg_if_else($3,$5,$7);}
				 | IF '(' EXP ')'  STMTS {$$ = cg_if($3,$5);}
                                 |IF '(' EXP ')'  STMT {$$ = cg_if($3,$5);}
				 
				;


STMT_WHILE		: WHILE '(' EXP ')' WHILEBODY  { $$ = cg_while($3,$5);};

WHILEBODY		: STMTS { $$ = $1; }
				| STMT  {$$ = $1;}
				;


				
STMT_CALL		: ID '(' ')' ';'{ 
					$$=cg_call($1);
                                     }  



TYPE			: INT
				| VOID
				| UINT
				;

%%

#include <ctype.h>

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


#ifdef __EMSCRIPTEN__
/*For emscripten, we provide a string compiler*/
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern void yy_flush_buffer( YY_BUFFER_STATE buffer );
extern void YY_FLUSH_BUFFER;
void compile(char *source){
   // clean output (memory leaks!)
    final = _alloc(100);
   strcpy(final,"CALL Lmain\nHALT\n"); // boot loader
   /*Parsing String*/
    yyrestart(NULL);
//    yy_flush_buffer( YY_CURRENT_BUFFER ); // discard what we have
    YY_FLUSH_BUFFER;
    printf("Debugging: source=<%s>",source);
    YY_BUFFER_STATE buffer = yy_scan_string(source);
    yyparse();
    yy_delete_buffer(buffer);
}

void yyerror(char *s)
{
   int len = 1024+strlen(yytext)+strlen(s);
   final = _alloc(len);
   snprintf(final,len, "SYNTAX ERROR\n============\nSyntax Error in line number : %d : %s %s\n", yylineno, s, yytext );
}


#else
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
	printf("Syntax Error in line number : %d : %s %s\n", yylineno, s, yytext );
}
         
#endif


