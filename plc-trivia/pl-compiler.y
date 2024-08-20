/*
pl parser,
adapted for Computational Foundations in TUM by Martin Werner
changes: 2024 M. Werner
*/

%{
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<assert.h>
char *yytext; //debug only
/*Prototypes*/

extern int yylex (void);


void yyerror(char *s);
void push();
void push_free(char *p);


void codegen_logical();
void codegen_algebric();
void codegen_assign();
void codegen_call();
void if_label1();
void if_label2();
void if_label3();
void while_start();
void while_rep();
void while_end();
void switch_start();
void switch_case();
void switch_default();
void switch_break();
void switch_nobreak();
void switch_end();
void check();
void setType();
void STMT_DECLARE();
void intermediateCode();
/* Globals*/

extern FILE *fp;
FILE * f1;
char *final=NULL; // pointer to the parser result after parsing

char *buffers[10000]; // maximal program length 10k
int buffer_top=0;
int lno=0;



/*Some utilities*/


char *_alloc(size_t n){
    buffers[buffer_top++] = (char *) malloc(n);
    // TODO: abort out of mem in both pointers and...
    printf("Allocated %d slot %d", n, buffer_top-1);
}

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



char *cg_call(char *name){
   assert(name != NULL);
   printf("CG_CALL for <%s>",name);
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
   }else
      snprintf(ret,1024, "CALL @%s\n", name);
   return ret;
}

char *cg_sequential(char *left, char *right){
   assert(left != NULL && right != NULL);
   printf("CG_SEQ for <%s,%s>",left,right); // todo: should strip newlines
   int len = strlen(left)+strlen(right)+1;
   char *ret = malloc(len+1);
   snprintf(ret,len, "%s%s", left,right);
   return ret;
}

char *cg_while(char *exp, char *body){
   assert(exp != NULL && body != NULL);
   int start_label,end_label;
   int len = 1024+strlen(exp)+strlen(body);
   printf("CG_WHILE for %s and %s",exp,body);
   char *ret = malloc(len);
   if (strcmp(exp, "true")== 0) { // special in pl1
       start_label = lno++;
       end_label = lno++;
      snprintf(ret,len, "L%d:\n%sJMP L%d\nL%d:\n", start_label,body, start_label,end_label);
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

   // done

   return ret;
}

char *cg_if (char *exp, char *body){
   assert(exp!= NULL && body != NULL);
   int len=1024+strlen(body);
printf("CG_IF for %s and %s",exp,body);
   char *ret = malloc(len);
   if (strcmp(exp, "front_blocked")== 0) { // special var  
      snprintf(ret,len, "LOADFB\nJZ L%d\n%sL%d:\n", lno++,body, lno);
   }else    if (strcmp(exp, "has_item")== 0) { // special var  
      snprintf(ret,len, "LOADHI\nJZ L%d\n%sL%d:\n", lno++,body, lno);
   }else{
	printf("OOPS. If does not support expressions yet"); exit(-1);
   }
   return ret;
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
%token IF ELSE SWITCH CASE BREAK DEFAULT CONTINUE
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

%type <name> STMT_CALL STMT STMT1 STMTS STMT_WHILE WHILEBODY STMT_IF

%%

pgmstart 			: TYPE ID '(' ')' STMTS {if (strcmp($2,"main")!=0){ printf("Only main allowed\n");exit(3);}final=$5;}
				;

STMTS 	: '{' STMT1 '}'{$$=$2;}| 
				;
STMT1			: STMT  STMT1 {$$=cg_sequential($1,$2);}
				| {$$=strdup("NOP\n");/*TODO: NOP is for parser debugging. Can be omitted dupin "" */}
				;

STMT 			: /*STMT_DECLARE    //all types of statements
				| STMT_ASSGN  
				|*/ STMT_IF {$$ = $1;}
				 | STMT_WHILE {$$=$1;}  /*
				| STMT_SWITCH
                                 
			*/	 | STMT_CALL {$$ = $1;}
			         | BREAK ';' {$$=strdup("BREAK\n");}
				 | CONTINUE ';' {$$=strdup("CONT\n");}
				| ';' {$$= strdup("");}
				;

				

EXP 			: EXP LT{push();} EXP {codegen_logical();}
				| EXP LE{push();} EXP {codegen_logical();}
				| EXP GT{push();} EXP {codegen_logical();}
				| EXP GE{push();} EXP {codegen_logical();}
				| EXP NE{push();} EXP {codegen_logical();}
				| EXP EQ{push();} EXP {codegen_logical();}
				| EXP '+'{push();} EXP {codegen_algebric();}
				| EXP '-'{push();} EXP {codegen_algebric();}
				| EXP '*'{push();} EXP {codegen_algebric();}
				| EXP '/'{push();} EXP {codegen_algebric();}
                                | EXP {push();} LOR EXP {codegen_logical();}
				| EXP {push();} LAND EXP {codegen_logical();}
				| EXP {push();} BOR EXP {codegen_logical();}
				| EXP {push();} BXOR EXP {codegen_logical();}
				| EXP {push();} BAND EXP {codegen_logical();}
				| '(' EXP ')'
				| ID {check();push();}
				| NUM {push();}
				| TRUE {push();}
				;


STMT_IF 			: IF '(' ID ')'  STMTS {$$ = cg_if($3,$5);}
                                 |IF '(' ID ')'  STMT {$$ = cg_if($3,$5);}
				;
ELSESTMT		: ELSE {if_label2();} STMTS {if_label3();}
				| {if_label3();}
				;

STMT_SWITCH		: SWITCH '(' EXP ')' {switch_start();} '{' SWITCHBODY '}'
				;
SWITCHBODY		: CASES {switch_end();}    
				| CASES DEFAULTSTMT {switch_end();}
				;

CASES 			: CASE NUM {switch_case();} ':' SWITCHEXP BREAKSTMT
				| 
				;
BREAKSTMT		: BREAK {switch_break();} ';' CASES
				|{switch_nobreak();} CASES 
				;

DEFAULTSTMT 	: DEFAULT {switch_default();} ':' SWITCHEXP DE  
				;

DE 				: BREAK {switch_break();}';'
				|
				;


SWITCHEXP 		: STMTS
				| STMT
				;


STMT_WHILE		: WHILE '(' TRUE ')' WHILEBODY  { $$ = cg_while($3,$5);};

WHILEBODY		: STMTS { $$ = $1; }
				| STMT  {$$ = $1;}
				;

STMT_DECLARE 	: TYPE {setType();}  ID {STMT_DECLARE();} IDS   //setting type for that line
				;


IDS 			: ';'
				| ','  ID {STMT_DECLARE();} IDS 
				;

				
STMT_CALL		: ID '(' ')' ';'{ /*push_free($1); codegen_call();*/
					$$=cg_call($1);
                                     }  // MARK

STMT_ASSGN		: ID {push();} ASGN {push();} EXP {codegen_assign();} ';'
				;


TYPE			: INT
				| VOID
				| UINT
				;

%%

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
	f1=fopen("output.trash","w");
	
   if(!yyparse())
		printf("\nParsing complete\n");
	else
	{
		printf("\nParsing failed\n");
		exit(0);
	}

	printf("Parsing result***********************\n");
	if (final == NULL){
	  printf("NULL pointer\n");
	  }else{
	     FILE *f = fopen("output.nasm","w");
	     fprintf(f,"%s\n",final);
	     fprintf(f,"HALT\n"); // end of code
	     fclose(f);
	     printf("%s\n",final);
	  }
	
	fclose(yyin);
	fclose(f1);
//	intermediateCode();
    return 0;
}
         

void yyerror(char *s)
{
	printf("Syntex Error in line number : %d : %s %s\n", yylineno, s, yytext );
}


void push_free(char *p)
{
//    printf("DEBUG: push and free <%s> to %d\n", p, top+1);
    strcpy(st[++top],p);
    free(p); //strdup before
}

void push()
{
//    printf("DEBUG: push <%s> to %d\n", yytext, top+1);
  	strcpy(st[++top],yytext);
}

void codegen_logical()
{
 	sprintf(temp,"$t%d",i);
  	fprintf(f1,"%s\t=\t%s\t%s\t%s\n",temp,st[top-2],st[top-1],st[top]);
  	top-=2;
 	strcpy(st[top],temp);
 	i++;
}

void codegen_algebric()
{
 	sprintf(temp,"$t%d",i); // converts temp to reqd format
  	fprintf(f1,"%s\t=\t%s\t%s\t%s\n",temp,st[top-2],st[top-1],st[top]);
  	top-=2;
 	strcpy(st[top],temp);
 	i++;
}

void stacktrace(){
    for(int i=top; i >= 0; i--)
      printf("@%03d: %s\n", st[i]);
}

void codegen_assign()
{
 	fprintf(f1,"%s\t=\t%s\n",st[top-2],st[top]);
 	top-=3;
}

void codegen_call(){
    // builtin (intrinsics) methods are directly output
    if (strcmp(st[top],"move") == 0)
    {
	fprintf(f1,"MOVE\n", st[top]);

    }else if (strcmp(st[top],"turn") == 0)
    {
	fprintf(f1,"TURN\n", st[top]);
    }else if (strcmp(st[top],"deposit") == 0)
    {
	fprintf(f1,"DEPO\n", st[top]);
    }else if (strcmp(st[top],"pickup") == 0)
    {
	fprintf(f1,"PICK\n", st[top]);
    }else {
       // Calling possible only in PL level > 3
	fprintf(f1,"CALL @%s\n", st[top]);
	

    }
    top-=1;

}
 
void if_label1()
{
 	lnum++;
 	fprintf(f1,"\tif( not %s)",st[top]);
 	fprintf(f1,"\tgoto $L%d\n",lnum);
 	label[++ltop]=lnum;
}

void if_label2()
{
	int x;
	lnum++;
	x=label[ltop--]; 
	fprintf(f1,"\t\tgoto $L%d\n",lnum);
	fprintf(f1,"$L%d: \n",x); 
	label[++ltop]=lnum;
}

void if_label3()
{
	int y;
	y=label[ltop--];
	fprintf(f1,"$L%d: \n",y);
	top--;
}
void while_start()
{
	lnum++;
	label[++ltop]=lnum;
	fprintf(f1,"$L%d:\n",lnum);
}
void while_rep()
{
	lnum++;
	if (strcmp(st[top],"true") == 0){

	}else{
	   fprintf(f1, "CMP %s\n",st[top]);
	   fprintf(f1, "JZ $L%d \n",lnum);
	}
// 	fprintf(f1,"if( not %s)",st[top]);
// 	fprintf(f1,"\tgoto $L%d\n",lnum);
 	label[++ltop]=lnum;
}
void while_end()
{
	int x,y;
	y=label[ltop--];
	x=label[ltop--];
	fprintf(f1,"JMP $L%d\n",x);
	if(strcmp(st[top],"true") != 0)
            fprintf(f1,"$L%d: \n",y);
	top--;
}
void switch_start()
{
	lnum++;
	label[++ltop]=lnum;
	lnum++;
	label[++ltop]=lnum;
	switch_stack[++stop]=1;
}
void switch_case()
{
	int x,y,z;
	z=switch_stack[stop--];
	if(z==1)
	{
		x=label[ltop--];
	}
	else if(z==2)
	{
		y=label[ltop--];
		x=label[ltop--];
	}
	fprintf(f1,"$L%d:\n",x);
	lnum++;
	label[++ltop]=lnum;
 	fprintf(f1,"if(%s != %s)",st[top],yytext);
 	fprintf(f1,"\tgoto $L%d\n",label[ltop]);
 	if(z==2)
 	{
 		fprintf(f1,"$L%d:\n",y);
 	}
}
void switch_default()
{
	int x=label[ltop--];
	fprintf(f1,"$L%d:\n",x);
	lnum++;
	label[++ltop]=lnum;
}
void switch_break()
{
	switch_stack[++stop]=1;
	fprintf(f1,"\t\tgoto $L%d\n",label[ltop-1]);
}
void switch_nobreak()
{
	switch_stack[++stop]=2;
	lnum++;
	label[++ltop]=lnum;
	fprintf(f1,"\t\tgoto $L%d\n",label[ltop]);
}
void switch_end()
{
	int x=label[ltop--];
	fprintf(f1,"$L%d:\n",x);
	x=label[ltop--];
	fprintf(f1,"$L%d:\n",x);
	top--;
	stop--;
}


/* for symbol table*/

void check()
{
	char temp[20];
	strncpy(temp,yytext,20);
	int flag=0;
	for(i=0;i<tableCount;i++)
	{
		if(!strcmp(table[i].id,temp))
		{
			flag=1;
			break;
		}
	}
	// PL builtins
	if (strcmp("front_clear",temp) == 0)
	    flag=1;
	if (strcmp("front_blocked",temp) == 0)
	    flag=1;
	

	
	if(!flag)
	{
		yyerror("Variable not declared");
		exit(0);
	}
}

void setType()
{
	strcpy(type,yytext);
}


void STMT_DECLARE()
{
	char temp[20];
	int i,flag;
	flag=0;
	strcpy(temp,yytext);
	for(i=0;i<tableCount;i++)
	{
		if(!strcmp(table[i].id,temp))
			{
			flag=1;
			break;
				}
	}
	if(flag)
	{
		yyerror("reSTMT_DECLARE of ");
		exit(0);
	}
	else
	{
		strcpy(table[tableCount].id,temp);
		strcpy(table[tableCount].type,type);
		tableCount++;
	}
}

void intermediateCode()
{
	int Labels[100000];
	char buf[100];
	f1=fopen("output","r");
	int flag=0,lineno=1;
	memset(Labels,0,sizeof(Labels));
	while(fgets(buf,sizeof(buf),f1)!=NULL)
	{
		//printf("%s",buf);
		if(buf[0]=='$'&&buf[1]=='$'&&buf[2]=='L')
		{
			int k=atoi(&buf[3]);
			//printf("hi %d\n",k);
			Labels[k]=lineno;
		}
		else
		{
			lineno++;
		}
	}
	fclose(f1);
	f1=fopen("output","r");
	lineno=0;

	printf("\n\n\n*********************InterMediate Code***************************\n\n");
	while(fgets(buf,sizeof(buf),f1)!=NULL)
	{
		//printf("%s",buf);
		if(buf[0]=='$'&&buf[1]=='$'&&buf[2]=='L')
		{
			;
		}
		else
		{
			flag=0;
			lineno++;
			printf("%3d:\t",lineno);
			int len=strlen(buf),i,flag1=0;
			for(i=len-3;i>=0;i--)
			{
				if(buf[i]=='$'&&buf[i+1]=='$'&&buf[i+2]=='L')
				{
					flag1=1;
					break;
				}
			}
			if(flag1)
			{
				buf[i]=='\0';
				int k=atoi(&buf[i+3]),j;
				//printf("%s",buf);
				for(j=0;j<i;j++)
					printf("%c",buf[j]);
				printf(" %d\n",Labels[k]);
			}
			else printf("%s",buf);
		}
	}
	fclose(f1);
}
