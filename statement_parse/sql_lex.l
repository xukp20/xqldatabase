%option caseless

%{
#include <string>
#include <cstring>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include "sql_yacc.tab.h"
%}

Identifier      [a-zA-Z_][a-zA-Z_0-9]*
IntegerVal      [0-9]+
String          '[^']*'
FloatVal        -?[0-9]+(\.[0-9]+)?
Whitespace      [ \t\n\r]+
Annotation      --([^;])+

EqualOrAssign   =
Less            <
LessEqual       <=
Greater         >
GreaterEqual    >=
NotEqual        <>

Count           "COUNT"
Average         "AVG"
Sum             "SUM"
Max             "MAX"
Min             "MIN"

%%

{Count}             { return COUNT; }
{Average}           { return AVG; }
{Sum}               { return SUM; }
{Max}               { return MAX; }
{Min}               { return MIN; }

";"             { return SEMICOLON; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"*"             { return ASTERISK; }
"."             { return DOT; }
","             { return COMMA; }

CREATE          { return CREATE; }
DATABASE        { return DATABASE; }
DROP            { return DROP; }
SHOW            { return SHOW; }
DATABASES       { return DATABASES; }
USE             { return USE; }
TABLE           { return TABLE; }
TABLES          { return TABLES; }
INDEX           { return INDEX; }
INDEXES         { return INDEXES; }

LOAD            { return LOAD; }
FROM            { return FROM; }
FILE            { return FILETK; }
TO              { return TO; }
DUMP            { return DUMP; }

DESC            { return DESC; }
INSERT          { return INSERT; }
INTO            { return INTO; }
VALUES          { return VALUES; }
DELETE          { return DELETE; }
WHERE           { return WHERE; }
UPDATE          { return UPDATE; }
SET             { return SET; }

SELECT          { return SELECT; }
GROUP           { return GROUP; }
BY              { return BY; }
LIMIT           { return LIMIT; }
OFFSET          { return OFFSET; }

ALTER           { return ALTER; }
ADD             { return ADD; }
PRIMARY         { return PRIMARY; }
KEY             { return KEY; }
FOREIGN         { return FOREIGN; }
CONSTRAINT      { return CONSTRAINT; }
REFERENCES      { return REFERENCES; }
UNIQUE          { return UNIQUE; }

NOT             { return NOT; }
DEFAULT         { return DEFAULT; }

NULL            { return NULLVAL; }
INT             { return TOKENINT; }
FLOAT           { return FLOAT; }
VARCHAR         { return VARCHAR; }

AND             { return AND; }
IN              { return IN; }
IS              { return IS; }
LIKE            { return LIKE; }

DONE            { return DONE; }
<<EOF>>         { return DONE; }

{EqualOrAssign}     { return EQ; }
{Less}              { return LT; }
{LessEqual}         { return LE; }
{Greater}           { return GT; }
{GreaterEqual}      { return GE; }
{NotEqual}          { return NE; }

{Identifier}        { 
    yylval.string = new char[strlen(yytext) + 1];  // needs to be deleted after use in the yacc file
    memcpy(yylval.string, yytext, strlen(yytext) + 1);
    return IDENTIFIER; }
{IntegerVal}        { yylval.integer = atoi(yytext); return INTEGERVAL; }
{FloatVal}          { yylval.floating = atof(yytext); return FLOATVAL; }
{String}            { 
    yylval.string = new char[strlen(yytext) - 1];
    memcpy(yylval.string, yytext + 1, strlen(yytext)-1);
    yylval.string[strlen(yytext) - 2] = '\0';
    return STRING;
    }
{Whitespace}        { }
{Annotation}        { return ANNOTATION; }

.               { return ERROR; }
%%

int yywrap()
{
	return 1;
}