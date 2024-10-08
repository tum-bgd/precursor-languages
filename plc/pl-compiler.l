/*
subc lexer,
adapted for Computational Foundations in TUM by Martin Werner
changes: 2024 M.Werner
*/
%{
#include "pl-compiler.tab.h"

char *_strdup(const char *);

%}
alpha [a-zA-Z]
digit [0-9]

%%
[ \t]		;
[ \n]   { yylineno = yylineno + 1;}
"int"	return INT;
"unsigned int"	return UINT;
"case"	return CASE;
"break"	return BREAK;
"return"	return RETURN;
"continue"	return CONTINUE;
"void" return VOID;
"while"	return WHILE;
"switch"	return SWITCH;
"default" return DEFAULT;
"true"	{yylval.name = _strdup("true");return TRUE;}
"false"	return FALSE;
"if"	return IF;
"else"	return ELSE;
^"#include ".+ ;
{digit}+       return NUM;
{alpha}({alpha}|{digit}|[_])* {yylval.name = _strdup(yytext); return ID;};
"="     return ASGN;
"<="    return LE;
">="    return GE;
"=="    return EQ;
"!="    return NE;
">"	return GT;
"<"	return LT;
"||" 	return LOR;
"&&"  	return LAND;
"|" 	return BOR;
"&"	return BAND;
"^"	return BXOR;
\/\/.* ;
\/\*(.*\n)*.*\*\/ ;
.       return yytext[0];
%%
