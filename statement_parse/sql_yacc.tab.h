/* A Bison parser, made by GNU Bison 3.8.  */

/* Bison interface for Yacc-like parsers in C

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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SQL_YACC_TAB_H_INCLUDED
# define YY_YY_SQL_YACC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 21 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"

#include <cstdio>
#include <string>
#include "SqlStatement.h"
#include "Executor.h"
using namespace std;

#line 57 "sql_yacc.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    STRING = 259,                  /* STRING  */
    INTEGERVAL = 260,              /* INTEGERVAL  */
    FLOATVAL = 261,                /* FLOATVAL  */
    ANNOTATION = 262,              /* ANNOTATION  */
    COUNT = 263,                   /* COUNT  */
    AVG = 264,                     /* AVG  */
    SUM = 265,                     /* SUM  */
    MIN = 266,                     /* MIN  */
    MAX = 267,                     /* MAX  */
    SEMICOLON = 268,               /* SEMICOLON  */
    LPAREN = 269,                  /* LPAREN  */
    RPAREN = 270,                  /* RPAREN  */
    COMMA = 271,                   /* COMMA  */
    DOT = 272,                     /* DOT  */
    ASTERISK = 273,                /* ASTERISK  */
    CREATE = 274,                  /* CREATE  */
    DATABASE = 275,                /* DATABASE  */
    DROP = 276,                    /* DROP  */
    SHOW = 277,                    /* SHOW  */
    DATABASES = 278,               /* DATABASES  */
    USE = 279,                     /* USE  */
    TABLE = 280,                   /* TABLE  */
    TABLES = 281,                  /* TABLES  */
    INDEX = 282,                   /* INDEX  */
    INDEXES = 283,                 /* INDEXES  */
    LOAD = 284,                    /* LOAD  */
    FROM = 285,                    /* FROM  */
    FILETK = 286,                  /* FILETK  */
    TO = 287,                      /* TO  */
    DUMP = 288,                    /* DUMP  */
    DESC = 289,                    /* DESC  */
    INSERT = 290,                  /* INSERT  */
    INTO = 291,                    /* INTO  */
    VALUES = 292,                  /* VALUES  */
    DELETE = 293,                  /* DELETE  */
    WHERE = 294,                   /* WHERE  */
    UPDATE = 295,                  /* UPDATE  */
    SET = 296,                     /* SET  */
    SELECT = 297,                  /* SELECT  */
    GROUP = 298,                   /* GROUP  */
    BY = 299,                      /* BY  */
    LIMIT = 300,                   /* LIMIT  */
    OFFSET = 301,                  /* OFFSET  */
    ALTER = 302,                   /* ALTER  */
    ADD = 303,                     /* ADD  */
    PRIMARY = 304,                 /* PRIMARY  */
    KEY = 305,                     /* KEY  */
    FOREIGN = 306,                 /* FOREIGN  */
    CONSTRAINT = 307,              /* CONSTRAINT  */
    REFERENCES = 308,              /* REFERENCES  */
    UNIQUE = 309,                  /* UNIQUE  */
    NOT = 310,                     /* NOT  */
    DEFAULT = 311,                 /* DEFAULT  */
    NULLVAL = 312,                 /* NULLVAL  */
    TOKENINT = 313,                /* TOKENINT  */
    FLOAT = 314,                   /* FLOAT  */
    VARCHAR = 315,                 /* VARCHAR  */
    AND = 316,                     /* AND  */
    IN = 317,                      /* IN  */
    IS = 318,                      /* IS  */
    LIKE = 319,                    /* LIKE  */
    EQ = 320,                      /* EQ  */
    LT = 321,                      /* LT  */
    LE = 322,                      /* LE  */
    GT = 323,                      /* GT  */
    GE = 324,                      /* GE  */
    NE = 325,                      /* NE  */
    DONE = 326,                    /* DONE  */
    ERROR = 327                    /* ERROR  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 29 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"

    int integer;
    float floating;
    char* string;
    FieldType* fieldType;
    Field* field;
    FieldList* fieldList;
    Value* value;
    ValueList* valueList;
    ValueLists* valueLists;
    Column* column;
    Operator* oper;
    Expression* expression;
    WhereClause* whereClause;
    WhereAndClause* whereAndClause;
    SetClause* setClause;
    Selector* selector;
    Selectors* selectors;
    Aggregator* aggregator;
    Identifiers* identifiers;

#line 168 "sql_yacc.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SQL_YACC_TAB_H_INCLUDED  */
