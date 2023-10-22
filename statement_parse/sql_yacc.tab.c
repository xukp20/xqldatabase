/* A Bison parser, made by GNU Bison 3.8.  */

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
#define YYBISON 30800

/* Bison version string.  */
#define YYBISON_VERSION "3.8"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 1 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"

#include <cstdio>
#include <string>
#include "SqlStatement.h"
#include "Executor.h"
using namespace std;


Executor* executor;
using namespace std;

extern int yylex();

// called in the tab.c
void yyerror(const char *s)
{
	printf("[error] %s\n", s);
}

#line 88 "sql_yacc.tab.c"




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

#include "sql_yacc.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_INTEGERVAL = 5,                 /* INTEGERVAL  */
  YYSYMBOL_FLOATVAL = 6,                   /* FLOATVAL  */
  YYSYMBOL_ANNOTATION = 7,                 /* ANNOTATION  */
  YYSYMBOL_COUNT = 8,                      /* COUNT  */
  YYSYMBOL_AVG = 9,                        /* AVG  */
  YYSYMBOL_SUM = 10,                       /* SUM  */
  YYSYMBOL_MIN = 11,                       /* MIN  */
  YYSYMBOL_MAX = 12,                       /* MAX  */
  YYSYMBOL_SEMICOLON = 13,                 /* SEMICOLON  */
  YYSYMBOL_LPAREN = 14,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 15,                    /* RPAREN  */
  YYSYMBOL_COMMA = 16,                     /* COMMA  */
  YYSYMBOL_DOT = 17,                       /* DOT  */
  YYSYMBOL_ASTERISK = 18,                  /* ASTERISK  */
  YYSYMBOL_CREATE = 19,                    /* CREATE  */
  YYSYMBOL_DATABASE = 20,                  /* DATABASE  */
  YYSYMBOL_DROP = 21,                      /* DROP  */
  YYSYMBOL_SHOW = 22,                      /* SHOW  */
  YYSYMBOL_DATABASES = 23,                 /* DATABASES  */
  YYSYMBOL_USE = 24,                       /* USE  */
  YYSYMBOL_TABLE = 25,                     /* TABLE  */
  YYSYMBOL_TABLES = 26,                    /* TABLES  */
  YYSYMBOL_INDEX = 27,                     /* INDEX  */
  YYSYMBOL_INDEXES = 28,                   /* INDEXES  */
  YYSYMBOL_LOAD = 29,                      /* LOAD  */
  YYSYMBOL_FROM = 30,                      /* FROM  */
  YYSYMBOL_FILETK = 31,                    /* FILETK  */
  YYSYMBOL_TO = 32,                        /* TO  */
  YYSYMBOL_DUMP = 33,                      /* DUMP  */
  YYSYMBOL_DESC = 34,                      /* DESC  */
  YYSYMBOL_INSERT = 35,                    /* INSERT  */
  YYSYMBOL_INTO = 36,                      /* INTO  */
  YYSYMBOL_VALUES = 37,                    /* VALUES  */
  YYSYMBOL_DELETE = 38,                    /* DELETE  */
  YYSYMBOL_WHERE = 39,                     /* WHERE  */
  YYSYMBOL_UPDATE = 40,                    /* UPDATE  */
  YYSYMBOL_SET = 41,                       /* SET  */
  YYSYMBOL_SELECT = 42,                    /* SELECT  */
  YYSYMBOL_GROUP = 43,                     /* GROUP  */
  YYSYMBOL_BY = 44,                        /* BY  */
  YYSYMBOL_LIMIT = 45,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 46,                    /* OFFSET  */
  YYSYMBOL_ALTER = 47,                     /* ALTER  */
  YYSYMBOL_ADD = 48,                       /* ADD  */
  YYSYMBOL_PRIMARY = 49,                   /* PRIMARY  */
  YYSYMBOL_KEY = 50,                       /* KEY  */
  YYSYMBOL_FOREIGN = 51,                   /* FOREIGN  */
  YYSYMBOL_CONSTRAINT = 52,                /* CONSTRAINT  */
  YYSYMBOL_REFERENCES = 53,                /* REFERENCES  */
  YYSYMBOL_UNIQUE = 54,                    /* UNIQUE  */
  YYSYMBOL_NOT = 55,                       /* NOT  */
  YYSYMBOL_DEFAULT = 56,                   /* DEFAULT  */
  YYSYMBOL_NULLVAL = 57,                   /* NULLVAL  */
  YYSYMBOL_TOKENINT = 58,                  /* TOKENINT  */
  YYSYMBOL_FLOAT = 59,                     /* FLOAT  */
  YYSYMBOL_VARCHAR = 60,                   /* VARCHAR  */
  YYSYMBOL_AND = 61,                       /* AND  */
  YYSYMBOL_IN = 62,                        /* IN  */
  YYSYMBOL_IS = 63,                        /* IS  */
  YYSYMBOL_LIKE = 64,                      /* LIKE  */
  YYSYMBOL_EQ = 65,                        /* EQ  */
  YYSYMBOL_LT = 66,                        /* LT  */
  YYSYMBOL_LE = 67,                        /* LE  */
  YYSYMBOL_GT = 68,                        /* GT  */
  YYSYMBOL_GE = 69,                        /* GE  */
  YYSYMBOL_NE = 70,                        /* NE  */
  YYSYMBOL_DONE = 71,                      /* DONE  */
  YYSYMBOL_ERROR = 72,                     /* ERROR  */
  YYSYMBOL_YYACCEPT = 73,                  /* $accept  */
  YYSYMBOL_program = 74,                   /* program  */
  YYSYMBOL_statement = 75,                 /* statement  */
  YYSYMBOL_db_statement = 76,              /* db_statement  */
  YYSYMBOL_io_statement = 77,              /* io_statement  */
  YYSYMBOL_table_statement = 78,           /* table_statement  */
  YYSYMBOL_select_table = 79,              /* select_table  */
  YYSYMBOL_alter_statement = 80,           /* alter_statement  */
  YYSYMBOL_field_list = 81,                /* field_list  */
  YYSYMBOL_field = 82,                     /* field  */
  YYSYMBOL_type_ = 83,                     /* type_  */
  YYSYMBOL_value_lists = 84,               /* value_lists  */
  YYSYMBOL_value_list = 85,                /* value_list  */
  YYSYMBOL_values = 86,                    /* values  */
  YYSYMBOL_value = 87,                     /* value  */
  YYSYMBOL_where_and_clause = 88,          /* where_and_clause  */
  YYSYMBOL_where_clause = 89,              /* where_clause  */
  YYSYMBOL_column = 90,                    /* column  */
  YYSYMBOL_expression = 91,                /* expression  */
  YYSYMBOL_set_clause = 92,                /* set_clause  */
  YYSYMBOL_selectors = 93,                 /* selectors  */
  YYSYMBOL_selector_list = 94,             /* selector_list  */
  YYSYMBOL_selector = 95,                  /* selector  */
  YYSYMBOL_identifiers = 96,               /* identifiers  */
  YYSYMBOL_oper_ = 97,                     /* oper_  */
  YYSYMBOL_aggregator = 98                 /* aggregator  */
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
typedef yytype_uint8 yy_state_t;

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

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
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
#define YYFINAL  55
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   284

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  98
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  255

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   327


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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    96,    96,    98,   100,   102,   104,   105,   109,   110,
     111,   112,   113,   114,   118,   130,   142,   153,   165,   176,
     190,   202,   217,   229,   241,   253,   265,   277,   290,   294,
     305,   317,   330,   344,   356,   373,   389,   405,   417,   430,
     443,   459,   476,   497,   519,   528,   532,   540,   549,   558,
     606,   653,   657,   663,   672,   676,   680,   687,   691,   699,
     706,   710,   718,   722,   726,   735,   742,   746,   754,   762,
     766,   770,   778,   786,   791,   800,   804,   811,   818,   827,
     831,   838,   842,   850,   854,   858,   863,   871,   876,   885,
     889,   893,   897,   901,   905,   912,   916,   920,   924
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
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER", "STRING",
  "INTEGERVAL", "FLOATVAL", "ANNOTATION", "COUNT", "AVG", "SUM", "MIN",
  "MAX", "SEMICOLON", "LPAREN", "RPAREN", "COMMA", "DOT", "ASTERISK",
  "CREATE", "DATABASE", "DROP", "SHOW", "DATABASES", "USE", "TABLE",
  "TABLES", "INDEX", "INDEXES", "LOAD", "FROM", "FILETK", "TO", "DUMP",
  "DESC", "INSERT", "INTO", "VALUES", "DELETE", "WHERE", "UPDATE", "SET",
  "SELECT", "GROUP", "BY", "LIMIT", "OFFSET", "ALTER", "ADD", "PRIMARY",
  "KEY", "FOREIGN", "CONSTRAINT", "REFERENCES", "UNIQUE", "NOT", "DEFAULT",
  "NULLVAL", "TOKENINT", "FLOAT", "VARCHAR", "AND", "IN", "IS", "LIKE",
  "EQ", "LT", "LE", "GT", "GE", "NE", "DONE", "ERROR", "$accept",
  "program", "statement", "db_statement", "io_statement",
  "table_statement", "select_table", "alter_statement", "field_list",
  "field", "type_", "value_lists", "value_list", "values", "value",
  "where_and_clause", "where_clause", "column", "expression", "set_clause",
  "selectors", "selector_list", "selector", "identifiers", "oper_",
  "aggregator", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-158)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     118,     7,    10,    39,    47,    24,    65,    12,    53,    91,
      61,    72,   103,    18,    86,   100,   111,    74,  -158,   117,
     137,   144,  -158,   173,  -158,  -158,   112,   123,   138,   143,
    -158,  -158,  -158,  -158,   156,   157,  -158,   187,   188,    79,
     142,   178,  -158,  -158,  -158,  -158,  -158,  -158,   164,   177,
    -158,   181,   193,  -158,  -158,  -158,   184,   185,  -158,  -158,
    -158,  -158,  -158,  -158,   186,  -158,  -158,   195,   197,   166,
     168,   199,   201,    36,   202,   124,   203,    -8,  -158,  -158,
       0,   176,   179,   196,   203,   147,    -5,  -158,   198,   200,
    -158,    -7,  -158,   204,    -3,   -17,    41,   170,   171,    62,
    -158,   189,   191,     1,   206,  -158,   150,  -158,   104,     1,
     214,   203,  -158,  -158,   215,   203,   218,  -158,   210,   175,
     180,   212,    22,   213,  -158,  -158,   217,    32,   219,    19,
    -158,     0,   225,   226,  -158,  -158,  -158,  -158,    75,  -158,
     196,   203,   196,   -14,   228,  -158,  -158,  -158,  -158,  -158,
    -158,    13,  -158,   172,   150,  -158,   -30,   190,   202,   231,
     232,   202,     4,   192,   194,   202,   233,   182,     1,   202,
     227,   202,  -158,  -158,  -158,  -158,     1,  -158,  -158,  -158,
     183,  -158,  -158,  -158,  -158,  -158,     1,   238,   240,    89,
    -158,  -158,   102,   205,   207,   234,   235,   107,   236,   208,
    -158,   113,   202,   128,  -158,  -158,  -158,   216,  -158,  -158,
    -158,   239,   242,   202,   202,  -158,  -158,     1,  -158,   133,
     220,   241,   202,   202,   139,   146,  -158,   221,   244,  -158,
     148,   161,  -158,   222,   247,   245,  -158,   223,   249,   246,
     202,   251,   252,   202,   163,   253,   202,   165,  -158,   202,
     167,  -158,   169,  -158,  -158
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,     0,
       0,     0,    28,     0,     7,    12,     0,     0,     0,     0,
      16,    18,    19,    17,     0,     0,    24,     0,     0,     0,
      73,     0,    95,    98,    97,    96,    79,    83,     0,    80,
      81,     0,     0,    13,     2,     1,     0,     0,     4,     8,
       9,    10,    11,    14,     0,    15,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     6,     5,
       0,     0,     0,     0,     0,     0,     0,    74,     0,     0,
      87,    29,    82,     0,     0,     0,     0,     0,     0,     0,
      45,     0,     0,     0,    25,    57,    26,    66,     0,     0,
       0,     0,    85,    86,     0,     0,     0,    84,     0,     0,
       0,     0,     0,     0,    54,    55,     0,    47,     0,     0,
      22,     0,     0,     0,    64,    62,    63,    65,     0,    60,
       0,     0,     0,     0,     0,    89,    90,    91,    92,    93,
      94,     0,    77,     0,    27,    88,    30,    33,     0,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    46,    20,    21,    59,     0,    58,    67,    71,
       0,    69,    72,    75,    76,    68,     0,     0,     0,     0,
      38,    39,     0,     0,     0,     0,     0,     0,     0,    48,
      49,     0,     0,     0,    61,    70,    78,    31,    34,    36,
      35,     0,     0,     0,     0,    44,    56,     0,    51,     0,
       0,     0,     0,     0,     0,     0,    50,     0,     0,    32,
       0,     0,    40,     0,     0,     0,    41,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    53,     0,
       0,    52,     0,    42,    43
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -158,  -158,   248,  -158,  -158,  -158,  -158,  -158,  -158,   127,
    -158,  -158,   -58,  -158,  -107,   -35,   120,   -13,  -158,  -158,
    -158,  -158,   209,  -157,  -158,  -158
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    17,    18,    19,    20,    21,    22,    23,    99,   100,
     127,   104,   105,   138,   139,   106,   107,   108,   185,    86,
      48,    49,    50,    91,   151,    51
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      47,   189,   152,    96,   192,   134,   135,   136,   197,   114,
     121,   110,   201,    94,   203,   187,    40,   134,   135,   136,
      24,    40,   170,    25,   118,   162,    41,    42,    43,    44,
      45,   141,   115,   171,   111,   122,    46,   123,   116,    40,
      95,   180,    34,   181,   183,   219,   119,    30,   120,    97,
      31,    98,    32,   193,    88,   194,   224,   225,   137,    26,
      89,   200,    47,    93,    27,   230,   231,    28,    33,   204,
     137,   163,    29,   164,    55,    56,   154,   130,   131,   206,
     156,     2,   177,   244,   179,    35,   247,   167,   168,   250,
     175,   176,   252,     3,    36,     4,     5,    37,     6,   124,
     125,   126,    38,     7,   209,   114,    39,     8,     9,    10,
     226,    52,    11,    53,    12,    63,    13,   210,   114,     1,
      71,    14,   215,   114,    54,     2,    64,    40,   218,   114,
      59,    15,    41,    42,    43,    44,    45,     3,   184,     4,
       5,    65,     6,   220,   114,    57,    66,     7,   227,   114,
      60,     8,     9,    10,   232,   114,    11,    61,    12,    72,
      13,   233,   114,   236,   114,    14,   142,   143,   144,   145,
     146,   147,   148,   149,   150,    15,   237,   114,   248,   114,
     251,   114,   253,   114,   254,   114,    62,    67,    68,    16,
      69,    70,    73,    75,    74,    76,    77,    78,    79,    81,
      80,    82,    85,    83,    87,    90,    40,    84,   101,   102,
     103,   141,   109,   112,   132,   113,   133,   153,   155,   117,
     128,   129,   140,   157,   158,   159,   161,   165,   173,   174,
     160,   166,   182,   169,   190,   191,   188,   186,   198,   199,
     205,   202,   195,   207,   196,   208,   229,   235,   213,   214,
     239,   216,   242,   222,   245,   211,   223,   212,   172,   240,
     243,   178,   221,     0,   217,    58,   246,   249,     0,     0,
       0,     0,     0,   228,   234,   238,   241,     0,     0,     0,
       0,     0,     0,     0,    92
};

static const yytype_int16 yycheck[] =
{
      13,   158,   109,     3,   161,     4,     5,     6,   165,    16,
      27,    16,   169,    21,   171,    45,     3,     4,     5,     6,
      13,     3,     3,    13,    27,     3,     8,     9,    10,    11,
      12,    61,    39,    14,    39,    52,    18,    54,    45,     3,
      48,    55,    30,    57,   151,   202,    49,    23,    51,    49,
      26,    51,    28,    49,    18,    51,   213,   214,    57,    20,
      73,   168,    75,    76,    25,   222,   223,    20,     3,   176,
      57,    49,    25,    51,     0,     1,   111,    15,    16,   186,
     115,     7,   140,   240,   142,    32,   243,    55,    56,   246,
      15,    16,   249,    19,     3,    21,    22,    36,    24,    58,
      59,    60,    30,    29,    15,    16,     3,    33,    34,    35,
     217,    25,    38,    13,    40,     3,    42,    15,    16,     1,
      41,    47,    15,    16,    13,     7,     3,     3,    15,    16,
      13,    57,     8,     9,    10,    11,    12,    19,   151,    21,
      22,     3,    24,    15,    16,    71,     3,    29,    15,    16,
      13,    33,    34,    35,    15,    16,    38,    13,    40,    17,
      42,    15,    16,    15,    16,    47,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    57,    15,    16,    15,    16,
      15,    16,    15,    16,    15,    16,    13,    31,    31,    71,
       3,     3,    14,    16,    30,    14,     3,    13,    13,     4,
      14,     4,     3,    37,     3,     3,     3,    39,    32,    30,
      14,    61,    65,    15,    25,    15,    25,     3,     3,    15,
      50,    50,    16,     5,    14,    50,    14,    14,     3,     3,
      50,    14,     4,    14,     3,     3,    46,    65,     5,    57,
      57,    14,    50,     5,    50,     5,     5,     3,    14,    14,
       3,    15,     3,    14,     3,    50,    14,    50,   131,    14,
      14,   141,    46,    -1,    56,    17,    14,    14,    -1,    -1,
      -1,    -1,    -1,    53,    53,    53,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     7,    19,    21,    22,    24,    29,    33,    34,
      35,    38,    40,    42,    47,    57,    71,    74,    75,    76,
      77,    78,    79,    80,    13,    13,    20,    25,    20,    25,
      23,    26,    28,     3,    30,    32,     3,    36,    30,     3,
       3,     8,     9,    10,    11,    12,    18,    90,    93,    94,
      95,    98,    25,    13,    13,     0,     1,    71,    75,    13,
      13,    13,    13,     3,     3,     3,     3,    31,    31,     3,
       3,    41,    17,    14,    30,    16,    14,     3,    13,    13,
      14,     4,     4,    37,    39,     3,    92,     3,    18,    90,
       3,    96,    95,    90,    21,    48,     3,    49,    51,    81,
      82,    32,    30,    14,    84,    85,    88,    89,    90,    65,
      16,    39,    15,    15,    16,    39,    45,    15,    27,    49,
      51,    27,    52,    54,    58,    59,    60,    83,    50,    50,
      15,    16,    25,    25,     4,     5,     6,    57,    86,    87,
      16,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    97,    87,     3,    88,     3,    88,     5,    14,    50,
      50,    14,     3,    49,    51,    14,    14,    55,    56,    14,
       3,    14,    82,     3,     3,    15,    16,    85,    89,    85,
      55,    57,     4,    87,    90,    91,    65,    45,    46,    96,
       3,     3,    96,    49,    51,    50,    50,    96,     5,    57,
      87,    96,    14,    96,    87,    57,    87,     5,     5,    15,
      15,    50,    50,    14,    14,    15,    15,    56,    15,    96,
      15,    46,    14,    14,    96,    96,    87,    15,    53,     5,
      96,    96,    15,    15,    53,     3,    15,    15,    53,     3,
      14,    53,     3,    14,    96,     3,    14,    96,    15,    14,
      96,    15,    96,    15,    15
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    73,    74,    74,    74,    74,    74,    74,    75,    75,
      75,    75,    75,    75,    76,    76,    76,    76,    76,    76,
      77,    77,    78,    78,    78,    78,    78,    78,    78,    79,
      79,    79,    79,    79,    79,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    81,    81,    82,    82,    82,
      82,    82,    82,    82,    83,    83,    83,    84,    84,    85,
      86,    86,    87,    87,    87,    87,    88,    88,    89,    89,
      89,    89,    89,    90,    90,    91,    91,    92,    92,    93,
      93,    94,    94,    95,    95,    95,    95,    96,    96,    97,
      97,    97,    97,    97,    97,    98,    98,    98,    98
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     2,     3,     3,     2,     2,     2,
       2,     2,     2,     2,     3,     3,     2,     2,     2,     2,
       7,     7,     6,     3,     2,     5,     5,     6,     1,     4,
       6,     8,    10,     6,     8,     8,     8,     6,     7,     7,
      10,    11,    15,    16,     8,     1,     3,     2,     4,     4,
       6,     5,    11,    10,     1,     1,     4,     1,     3,     3,
       1,     3,     1,     1,     1,     1,     1,     3,     3,     3,
       4,     3,     3,     1,     3,     1,     1,     3,     5,     1,
       1,     1,     3,     1,     4,     4,     4,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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
    YYNOMEM;
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
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
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
  case 2: /* program: DONE SEMICOLON  */
#line 97 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    { YYACCEPT;}
#line 1345 "sql_yacc.tab.c"
    break;

  case 3: /* program: statement  */
#line 99 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {}
#line 1351 "sql_yacc.tab.c"
    break;

  case 4: /* program: program statement  */
#line 101 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {}
#line 1357 "sql_yacc.tab.c"
    break;

  case 5: /* program: program DONE SEMICOLON  */
#line 103 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    { YYACCEPT;}
#line 1363 "sql_yacc.tab.c"
    break;

  case 6: /* program: program error SEMICOLON  */
#line 104 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
                              { yyerrok; executor->showNextPrefix(); }
#line 1369 "sql_yacc.tab.c"
    break;

  case 7: /* program: error SEMICOLON  */
#line 105 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
                      { yyerrok; executor->showNextPrefix(); }
#line 1375 "sql_yacc.tab.c"
    break;

  case 14: /* db_statement: CREATE DATABASE IDENTIFIER  */
#line 119 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::CREATE, string((yyvsp[0].string)));
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("create database failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
        delete[] (yyvsp[0].string);
    }
#line 1391 "sql_yacc.tab.c"
    break;

  case 15: /* db_statement: DROP DATABASE IDENTIFIER  */
#line 131 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::DROP, (yyvsp[0].string));
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop database failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
        delete[] (yyvsp[0].string);
    }
#line 1407 "sql_yacc.tab.c"
    break;

  case 16: /* db_statement: SHOW DATABASES  */
#line 143 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::SHOWDBS);
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("show databases failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
    }
#line 1422 "sql_yacc.tab.c"
    break;

  case 17: /* db_statement: USE IDENTIFIER  */
#line 154 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::USE, (yyvsp[0].string));
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("use database failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
        delete[] (yyvsp[0].string);
    }
#line 1438 "sql_yacc.tab.c"
    break;

  case 18: /* db_statement: SHOW TABLES  */
#line 166 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::SHOWTABLES);
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("show tables failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
    }
#line 1453 "sql_yacc.tab.c"
    break;

  case 19: /* db_statement: SHOW INDEXES  */
#line 177 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::SHOWINDEXES);
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("show indexes failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
    }
#line 1468 "sql_yacc.tab.c"
    break;

  case 20: /* io_statement: LOAD FROM FILETK STRING TO TABLE IDENTIFIER  */
#line 191 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        IOStatement* statement = new IOStatement(IOStatement::LOAD, (yyvsp[-3].string), (yyvsp[0].string));
        bool result = executor->run(statement);
        executor->printEnd();
        if(!result){
            yyerror("load table failed");
        }
        executor->showNextPrefix();
        delete statement;
        delete[] (yyvsp[-3].string);
    }
#line 1484 "sql_yacc.tab.c"
    break;

  case 21: /* io_statement: DUMP TO FILETK STRING FROM TABLE IDENTIFIER  */
#line 203 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        IOStatement* statement = new IOStatement(IOStatement::DUMP, (yyvsp[-3].string), (yyvsp[0].string));
        bool result = executor->run(statement);
        executor->printEnd();
        if(!result){
            yyerror("dump table failed");
        }
        executor->showNextPrefix();
        delete statement;
        delete[] (yyvsp[-3].string);
    }
#line 1500 "sql_yacc.tab.c"
    break;

  case 22: /* table_statement: CREATE TABLE IDENTIFIER LPAREN field_list RPAREN  */
#line 218 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        TableStatement* tableStatement = new CreateTableStatement(string((yyvsp[-3].string)), (yyvsp[-1].fieldList));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("create table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] (yyvsp[-3].string);
    }
#line 1516 "sql_yacc.tab.c"
    break;

  case 23: /* table_statement: DROP TABLE IDENTIFIER  */
#line 230 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        TableStatement* tableStatement = new DropTableStatement(string((yyvsp[0].string)));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] (yyvsp[0].string);
    }
#line 1532 "sql_yacc.tab.c"
    break;

  case 24: /* table_statement: DESC IDENTIFIER  */
#line 242 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        TableStatement* tableStatement = new DescTableStatement(string((yyvsp[0].string)));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("desc table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] (yyvsp[0].string);
    }
#line 1548 "sql_yacc.tab.c"
    break;

  case 25: /* table_statement: INSERT INTO IDENTIFIER VALUES value_lists  */
#line 254 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        TableStatement* tableStatement = new InsertStatement(string((yyvsp[-2].string)), (yyvsp[0].valueLists));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("insert into table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] (yyvsp[-2].string);
    }
#line 1564 "sql_yacc.tab.c"
    break;

  case 26: /* table_statement: DELETE FROM IDENTIFIER WHERE where_and_clause  */
#line 266 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        TableStatement* tableStatement = new DeleteStatement(string((yyvsp[-2].string)), (yyvsp[0].whereAndClause));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("delete from table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] (yyvsp[-2].string);
    }
#line 1580 "sql_yacc.tab.c"
    break;

  case 27: /* table_statement: UPDATE IDENTIFIER SET set_clause WHERE where_and_clause  */
#line 278 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        // cout<<"1"<<endl;
        TableStatement* tableStatement = new UpdateStatement(string((yyvsp[-4].string)), (yyvsp[-2].setClause), (yyvsp[0].whereAndClause));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("update table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] (yyvsp[-4].string);
    }
#line 1597 "sql_yacc.tab.c"
    break;

  case 28: /* table_statement: select_table  */
#line 290 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
                   {}
#line 1603 "sql_yacc.tab.c"
    break;

  case 29: /* select_table: SELECT selectors FROM identifiers  */
#line 295 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        SelectStatement* selectStatement = new SelectStatement((yyvsp[-2].selectors), (yyvsp[0].identifiers));
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
#line 1618 "sql_yacc.tab.c"
    break;

  case 30: /* select_table: SELECT selectors FROM identifiers WHERE where_and_clause  */
#line 306 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        SelectStatement* selectStatement = new SelectStatement((yyvsp[-4].selectors), (yyvsp[-2].identifiers));
        selectStatement->setWhere((yyvsp[0].whereAndClause));
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
#line 1634 "sql_yacc.tab.c"
    break;

  case 31: /* select_table: SELECT selectors FROM identifiers WHERE where_and_clause LIMIT INTEGERVAL  */
#line 318 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        SelectStatement* selectStatement = new SelectStatement((yyvsp[-6].selectors), (yyvsp[-4].identifiers));
        selectStatement->setWhere((yyvsp[-2].whereAndClause));
        selectStatement->setLimit((yyvsp[0].integer));
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
#line 1651 "sql_yacc.tab.c"
    break;

  case 32: /* select_table: SELECT selectors FROM identifiers WHERE where_and_clause LIMIT INTEGERVAL OFFSET INTEGERVAL  */
#line 331 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        SelectStatement* selectStatement = new SelectStatement((yyvsp[-8].selectors), (yyvsp[-6].identifiers));
        selectStatement->setWhere((yyvsp[-4].whereAndClause));
        selectStatement->setLimit((yyvsp[-2].integer));
        selectStatement->setOffset((yyvsp[0].integer));
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
#line 1669 "sql_yacc.tab.c"
    break;

  case 33: /* select_table: SELECT selectors FROM identifiers LIMIT INTEGERVAL  */
#line 345 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        SelectStatement* selectStatement = new SelectStatement((yyvsp[-4].selectors), (yyvsp[-2].identifiers));
        selectStatement->setLimit((yyvsp[0].integer));
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
#line 1685 "sql_yacc.tab.c"
    break;

  case 34: /* select_table: SELECT selectors FROM identifiers LIMIT INTEGERVAL OFFSET INTEGERVAL  */
#line 357 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        SelectStatement* selectStatement = new SelectStatement((yyvsp[-6].selectors), (yyvsp[-4].identifiers));
        selectStatement->setLimit((yyvsp[-2].integer));
        selectStatement->setOffset((yyvsp[0].integer));
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
#line 1702 "sql_yacc.tab.c"
    break;

  case 35: /* alter_statement: ALTER TABLE IDENTIFIER ADD INDEX LPAREN identifiers RPAREN  */
#line 374 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::ADD, AlterStatement::INDEX, string((yyvsp[-5].string)));
        for(int i=0; i<(yyvsp[-1].identifiers)->identifiers.size(); i++){
            alterStatement->addColumn((yyvsp[-1].identifiers)->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add index failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-5].string);
        delete (yyvsp[-1].identifiers);
    }
#line 1722 "sql_yacc.tab.c"
    break;

  case 36: /* alter_statement: ALTER TABLE IDENTIFIER DROP INDEX LPAREN identifiers RPAREN  */
#line 390 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::DROP, AlterStatement::INDEX, string((yyvsp[-5].string)));
        for(int i=0; i<(yyvsp[-1].identifiers)->identifiers.size(); i++){
            alterStatement->addColumn((yyvsp[-1].identifiers)->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop index failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-5].string);
        delete (yyvsp[-1].identifiers);
    }
#line 1742 "sql_yacc.tab.c"
    break;

  case 37: /* alter_statement: ALTER TABLE IDENTIFIER DROP PRIMARY KEY  */
#line 406 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::DROP, AlterStatement::PRIMARY, string((yyvsp[-3].string)));
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-3].string);
    }
#line 1758 "sql_yacc.tab.c"
    break;

  case 38: /* alter_statement: ALTER TABLE IDENTIFIER DROP PRIMARY KEY IDENTIFIER  */
#line 418 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::DROP, AlterStatement::PRIMARY, string((yyvsp[-4].string)));
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-4].string);
        delete[] (yyvsp[0].string);
    }
#line 1775 "sql_yacc.tab.c"
    break;

  case 39: /* alter_statement: ALTER TABLE IDENTIFIER DROP FOREIGN KEY IDENTIFIER  */
#line 431 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        AlterStatement* alterStatement = new AlterForeignKeyStatement(AlterStatement::DROP, string((yyvsp[-4].string)), string((yyvsp[0].string)));
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop foreign key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-4].string);
        delete[] (yyvsp[0].string);
    }
#line 1792 "sql_yacc.tab.c"
    break;

  case 40: /* alter_statement: ALTER TABLE IDENTIFIER ADD CONSTRAINT PRIMARY KEY LPAREN identifiers RPAREN  */
#line 444 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::ADD, AlterStatement::PRIMARY, string((yyvsp[-7].string)));
        for(int i=0; i<(yyvsp[-1].identifiers)->identifiers.size(); i++){
            alterStatement->addColumn((yyvsp[-1].identifiers)->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-7].string);
        delete (yyvsp[-1].identifiers);
    }
#line 1812 "sql_yacc.tab.c"
    break;

  case 41: /* alter_statement: ALTER TABLE IDENTIFIER ADD CONSTRAINT IDENTIFIER PRIMARY KEY LPAREN identifiers RPAREN  */
#line 460 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::ADD, AlterStatement::PRIMARY, string((yyvsp[-8].string)));
        for(int i=0; i<(yyvsp[-1].identifiers)->identifiers.size(); i++){
            alterStatement->addColumn((yyvsp[-1].identifiers)->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-8].string);
        delete[] (yyvsp[-5].string);
        delete (yyvsp[-1].identifiers);
    }
#line 1833 "sql_yacc.tab.c"
    break;

  case 42: /* alter_statement: ALTER TABLE IDENTIFIER ADD CONSTRAINT FOREIGN KEY LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN  */
#line 477 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {  
        AlterStatement* alterStatement = new AlterForeignKeyStatement(AlterStatement::ADD, string((yyvsp[-12].string)), string(""), string((yyvsp[-3].string)));
        for(int i=0; i<(yyvsp[-6].identifiers)->identifiers.size(); i++){
            alterStatement->addColumn((yyvsp[-6].identifiers)->identifiers[i]);
        }
        for(int i=0; i<(yyvsp[-1].identifiers)->identifiers.size(); i++){
            ((AlterForeignKeyStatement*)alterStatement)->addTargetColumn((yyvsp[-1].identifiers)->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add foreign key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-12].string);
        delete[] (yyvsp[-3].string);
        delete (yyvsp[-6].identifiers);
        delete (yyvsp[-1].identifiers);
    }
#line 1858 "sql_yacc.tab.c"
    break;

  case 43: /* alter_statement: ALTER TABLE IDENTIFIER ADD CONSTRAINT IDENTIFIER FOREIGN KEY LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN  */
#line 498 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {  
        AlterStatement* alterStatement = new AlterForeignKeyStatement(AlterStatement::ADD, string((yyvsp[-13].string)), string((yyvsp[-10].string)), string((yyvsp[-3].string)));
        for(int i=0; i<(yyvsp[-6].identifiers)->identifiers.size(); i++){
            alterStatement->addColumn((yyvsp[-6].identifiers)->identifiers[i]);
        }
        for(int i=0; i<(yyvsp[-1].identifiers)->identifiers.size(); i++){
            ((AlterForeignKeyStatement*)alterStatement)->addTargetColumn((yyvsp[-1].identifiers)->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add foreign key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] (yyvsp[-13].string);
        delete[] (yyvsp[-10].string);
        delete[] (yyvsp[-3].string);
        delete (yyvsp[-6].identifiers);
        delete (yyvsp[-1].identifiers);
    }
#line 1884 "sql_yacc.tab.c"
    break;

  case 44: /* alter_statement: ALTER TABLE IDENTIFIER ADD UNIQUE LPAREN identifiers RPAREN  */
#line 520 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        cout<<"add unique not implemented"<<endl;
        executor->showNextPrefix();
        delete[] (yyvsp[-5].string);
        delete (yyvsp[-1].identifiers);
    }
#line 1895 "sql_yacc.tab.c"
    break;

  case 45: /* field_list: field  */
#line 529 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.fieldList) = new FieldList((yyvsp[0].field));
    }
#line 1903 "sql_yacc.tab.c"
    break;

  case 46: /* field_list: field_list COMMA field  */
#line 533 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.fieldList) = (yyvsp[-2].fieldList);
        (yyvsp[-2].fieldList)->add((yyvsp[0].field));
    }
#line 1912 "sql_yacc.tab.c"
    break;

  case 47: /* field: IDENTIFIER type_  */
#line 541 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        string column_name = (yyvsp[-1].string);
        unsigned int kind = ((yyvsp[0].fieldType)->type == FieldType::INT) ? 0 : ((yyvsp[0].fieldType)->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = (yyvsp[0].fieldType)->length;
        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, nullptr, false);
        (yyval.field) = new Field(columnMeta);
        delete[] (yyvsp[-1].string);
    }
#line 1925 "sql_yacc.tab.c"
    break;

  case 48: /* field: IDENTIFIER type_ NOT NULLVAL  */
#line 550 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        string column_name = (yyvsp[-3].string);
        unsigned int kind = ((yyvsp[-2].fieldType)->type == FieldType::INT) ? 0 : ((yyvsp[-2].fieldType)->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = (yyvsp[-2].fieldType)->length;
        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, nullptr, true);
        (yyval.field) = new Field(columnMeta);
        delete[] (yyvsp[-3].string);
    }
#line 1938 "sql_yacc.tab.c"
    break;

  case 49: /* field: IDENTIFIER type_ DEFAULT value  */
#line 559 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        string column_name = (yyvsp[-3].string);
        unsigned int kind = ((yyvsp[-2].fieldType)->type == FieldType::INT) ? 0 : ((yyvsp[-2].fieldType)->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = (yyvsp[-2].fieldType)->length;
        unsigned int default_kind = ((yyvsp[0].value)->value_type == Value::INT) ? 0 : 
                                    ((yyvsp[0].value)->value_type == Value::FLOAT) ? 1 : 
                                    ((yyvsp[0].value)->value_type == Value::STRING) ? 2 : 3;
        unsigned int default_len = 4;
        if(default_kind == 2){  // check len
            default_len = (yyvsp[0].value)->string_value.length();
            if(default_len > len){
                yyerror("default value length exceeds field length");
                YYERROR;
            }
        }
        if(!(default_kind == kind || default_kind == 3)){  // check type
            yyerror("default value type does not match field type");
            YYERROR;
        }
        
        DefaultConstraint* defaultConstraint;
        switch (default_kind)
        {
            case 0:
                defaultConstraint = new DefaultIntConstraint((yyvsp[0].value)->int_value);
                break;
            
            case 1:
                defaultConstraint = new DefaultFloatConstraint((yyvsp[0].value)->float_value);
                break;

            case 2:
                defaultConstraint = new DefaultCharConstraint((yyvsp[0].value)->string_value.c_str(), default_len);
                break;
            
            case 3:     // Default Null
                defaultConstraint = new DefaultConstraint(3);
                break;

            default:
                break;
        }

        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, defaultConstraint, false);
        (yyval.field) = new Field(columnMeta);
        delete[] (yyvsp[-3].string);
    }
#line 1990 "sql_yacc.tab.c"
    break;

  case 50: /* field: IDENTIFIER type_ NOT NULLVAL DEFAULT value  */
#line 607 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        string column_name = (yyvsp[-5].string);
        unsigned int kind = ((yyvsp[-4].fieldType)->type == FieldType::INT) ? 0 : ((yyvsp[-4].fieldType)->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = (yyvsp[-4].fieldType)->length;
        unsigned int default_kind = ((yyvsp[0].value)->value_type == Value::INT) ? 0 : 
                                    ((yyvsp[0].value)->value_type == Value::FLOAT) ? 1 : 
                                    ((yyvsp[0].value)->value_type == Value::STRING) ? 2 : 3;
        unsigned int default_len = 4;
        if(default_kind == 2){  // check len
            default_len = (yyvsp[0].value)->string_value.length();
            if(default_len > len){
                yyerror("default value length exceeds field length");
                YYERROR;
            }
        } else if (default_kind == 3){
            yyerror("default value cannot be null");
            YYERROR;
        }
        if(!(default_kind == kind)){  // check type
            yyerror("default value type does not match field type");
            YYERROR;
        }
        
        DefaultConstraint* defaultConstraint;
        switch (default_kind)
        {
            case 0:
                defaultConstraint = new DefaultIntConstraint((yyvsp[0].value)->int_value);
                break;
            
            case 1:
                defaultConstraint = new DefaultFloatConstraint((yyvsp[0].value)->float_value);
                break;

            case 2:
                defaultConstraint = new DefaultCharConstraint((yyvsp[0].value)->string_value.c_str(), default_len);
                break;

            default:
                break;
        }

        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, defaultConstraint, true);
        (yyval.field) = new Field(columnMeta);
        delete[] (yyvsp[-5].string);
    }
#line 2041 "sql_yacc.tab.c"
    break;

  case 51: /* field: PRIMARY KEY LPAREN identifiers RPAREN  */
#line 654 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.field) = new Field((yyvsp[-1].identifiers));
    }
#line 2049 "sql_yacc.tab.c"
    break;

  case 52: /* field: FOREIGN KEY IDENTIFIER LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN  */
#line 658 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        ForeignKeyField* foreignKeyField = new ForeignKeyField(string((yyvsp[-8].string)), string((yyvsp[-3].string)), (yyvsp[-1].identifiers), (yyvsp[-6].identifiers));
        (yyval.field) = new Field(foreignKeyField);
        delete[] (yyvsp[-8].string);
    }
#line 2059 "sql_yacc.tab.c"
    break;

  case 53: /* field: FOREIGN KEY LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN  */
#line 664 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        ForeignKeyField* foreignKeyField = new ForeignKeyField(string(""), string((yyvsp[-3].string)), (yyvsp[-1].identifiers), (yyvsp[-6].identifiers));
        (yyval.field) = new Field(foreignKeyField);
        delete[] (yyvsp[-3].string);
    }
#line 2069 "sql_yacc.tab.c"
    break;

  case 54: /* type_: TOKENINT  */
#line 673 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.fieldType) = new FieldType(FieldType::INT, 4);
    }
#line 2077 "sql_yacc.tab.c"
    break;

  case 55: /* type_: FLOAT  */
#line 677 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.fieldType) = new FieldType(FieldType::FLOAT, 4);
    }
#line 2085 "sql_yacc.tab.c"
    break;

  case 56: /* type_: VARCHAR LPAREN INTEGERVAL RPAREN  */
#line 681 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.fieldType) = new FieldType(FieldType::VARCHAR, (yyvsp[-1].integer));
    }
#line 2093 "sql_yacc.tab.c"
    break;

  case 57: /* value_lists: value_list  */
#line 688 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.valueLists) = new ValueLists((yyvsp[0].valueList));
    }
#line 2101 "sql_yacc.tab.c"
    break;

  case 58: /* value_lists: value_lists COMMA value_list  */
#line 692 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.valueLists) = (yyvsp[-2].valueLists);
        (yyvsp[-2].valueLists)->add((yyvsp[0].valueList));
    }
#line 2110 "sql_yacc.tab.c"
    break;

  case 59: /* value_list: LPAREN values RPAREN  */
#line 700 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.valueList) = (yyvsp[-1].valueList);
    }
#line 2118 "sql_yacc.tab.c"
    break;

  case 60: /* values: value  */
#line 707 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.valueList) = new ValueList((yyvsp[0].value));
    }
#line 2126 "sql_yacc.tab.c"
    break;

  case 61: /* values: values COMMA value  */
#line 711 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.valueList) = (yyvsp[-2].valueList);
        (yyvsp[-2].valueList)->add((yyvsp[0].value));
    }
#line 2135 "sql_yacc.tab.c"
    break;

  case 62: /* value: INTEGERVAL  */
#line 719 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.value) = new Value((yyvsp[0].integer));
    }
#line 2143 "sql_yacc.tab.c"
    break;

  case 63: /* value: FLOATVAL  */
#line 723 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.value) = new Value(float((yyvsp[0].floating)));
    }
#line 2151 "sql_yacc.tab.c"
    break;

  case 64: /* value: STRING  */
#line 727 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        // cout<<"STRING"<<endl;
        // cout<<$1<<endl;
        // cout<<"."<<$1[0]<<"."<<$1[1]<<"."<<$1[2]<<"."<<endl;
        (yyval.value) = new Value(string((yyvsp[0].string)));
        delete[] (yyvsp[0].string);
        // cout<<"after"<<endl;
    }
#line 2164 "sql_yacc.tab.c"
    break;

  case 65: /* value: NULLVAL  */
#line 736 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.value) = new Value();
    }
#line 2172 "sql_yacc.tab.c"
    break;

  case 66: /* where_and_clause: where_clause  */
#line 743 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.whereAndClause) = new WhereAndClause((yyvsp[0].whereClause));
    }
#line 2180 "sql_yacc.tab.c"
    break;

  case 67: /* where_and_clause: where_and_clause AND where_clause  */
#line 747 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.whereAndClause) = (yyvsp[-2].whereAndClause);
        (yyvsp[-2].whereAndClause)->add((yyvsp[0].whereClause));
    }
#line 2189 "sql_yacc.tab.c"
    break;

  case 68: /* where_clause: column oper_ expression  */
#line 755 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.whereClause) = new WhereOperatorExpr((yyvsp[-2].column), (yyvsp[-1].oper), (yyvsp[0].expression));
    }
#line 2197 "sql_yacc.tab.c"
    break;

  case 69: /* where_clause: column IS NULLVAL  */
#line 763 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.whereClause) = new WhereIsNull((yyvsp[-2].column), true);
    }
#line 2205 "sql_yacc.tab.c"
    break;

  case 70: /* where_clause: column IS NOT NULLVAL  */
#line 767 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.whereClause) = new WhereIsNull((yyvsp[-3].column), false);
    }
#line 2213 "sql_yacc.tab.c"
    break;

  case 71: /* where_clause: column IN value_list  */
#line 771 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.whereClause) = new WhereInList((yyvsp[-2].column), (yyvsp[0].valueList));
    }
#line 2221 "sql_yacc.tab.c"
    break;

  case 72: /* where_clause: column LIKE STRING  */
#line 779 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.whereClause) = new WhereLike((yyvsp[-2].column), (yyvsp[0].string));
        delete[] (yyvsp[0].string);
    }
#line 2230 "sql_yacc.tab.c"
    break;

  case 73: /* column: IDENTIFIER  */
#line 787 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.column) = new Column((yyvsp[0].string));
        delete[] (yyvsp[0].string);
    }
#line 2239 "sql_yacc.tab.c"
    break;

  case 74: /* column: IDENTIFIER DOT IDENTIFIER  */
#line 792 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.column) = new Column((yyvsp[-2].string), (yyvsp[0].string));
        delete[] (yyvsp[-2].string);
        delete[] (yyvsp[0].string);
    }
#line 2249 "sql_yacc.tab.c"
    break;

  case 75: /* expression: value  */
#line 801 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.expression) = new Expression((yyvsp[0].value));
    }
#line 2257 "sql_yacc.tab.c"
    break;

  case 76: /* expression: column  */
#line 805 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.expression) = new Expression((yyvsp[0].column));
    }
#line 2265 "sql_yacc.tab.c"
    break;

  case 77: /* set_clause: IDENTIFIER EQ value  */
#line 812 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        cout<<"set_clause: IDENTIFIER EQ value"<<endl;
        SetValue* setValue = new SetValue((yyvsp[-2].string), (yyvsp[0].value));
        (yyval.setClause) = new SetClause(setValue);
        delete[] (yyvsp[-2].string);
    }
#line 2276 "sql_yacc.tab.c"
    break;

  case 78: /* set_clause: set_clause COMMA IDENTIFIER EQ value  */
#line 819 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        SetValue* setValue = new SetValue((yyvsp[-2].string), (yyvsp[0].value));
        (yyval.setClause) = (yyvsp[-4].setClause);
        (yyvsp[-4].setClause)->add(setValue);
        delete[] (yyvsp[-2].string);
    }
#line 2287 "sql_yacc.tab.c"
    break;

  case 79: /* selectors: ASTERISK  */
#line 828 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.selectors) = new Selectors(true);   // selector for all
    }
#line 2295 "sql_yacc.tab.c"
    break;

  case 80: /* selectors: selector_list  */
#line 832 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.selectors) = (yyvsp[0].selectors);
    }
#line 2303 "sql_yacc.tab.c"
    break;

  case 81: /* selector_list: selector  */
#line 839 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.selectors) = new Selectors((yyvsp[0].selector));
    }
#line 2311 "sql_yacc.tab.c"
    break;

  case 82: /* selector_list: selector_list COMMA selector  */
#line 843 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.selectors) = (yyvsp[-2].selectors);
        (yyvsp[-2].selectors)->add((yyvsp[0].selector));
    }
#line 2320 "sql_yacc.tab.c"
    break;

  case 83: /* selector: column  */
#line 851 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.selector) = new ColumnSelector((yyvsp[0].column));
    }
#line 2328 "sql_yacc.tab.c"
    break;

  case 84: /* selector: aggregator LPAREN column RPAREN  */
#line 855 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.selector) = new AggregatorSelector((yyvsp[-3].aggregator), (yyvsp[-1].column));
    }
#line 2336 "sql_yacc.tab.c"
    break;

  case 85: /* selector: COUNT LPAREN ASTERISK RPAREN  */
#line 859 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        Aggregator* aggregator = new Aggregator(Aggregator::COUNT);
        (yyval.selector) = new AggregatorSelector(aggregator, nullptr);
    }
#line 2345 "sql_yacc.tab.c"
    break;

  case 86: /* selector: COUNT LPAREN column RPAREN  */
#line 864 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        Aggregator* aggregator = new Aggregator(Aggregator::COUNT);
        (yyval.selector) = new AggregatorSelector(aggregator, (yyvsp[-1].column));
    }
#line 2354 "sql_yacc.tab.c"
    break;

  case 87: /* identifiers: IDENTIFIER  */
#line 872 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.identifiers) = new Identifiers((yyvsp[0].string));
        delete[] (yyvsp[0].string);
    }
#line 2363 "sql_yacc.tab.c"
    break;

  case 88: /* identifiers: identifiers COMMA IDENTIFIER  */
#line 877 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.identifiers) = (yyvsp[-2].identifiers);
        (yyvsp[-2].identifiers)->add((yyvsp[0].string));
        delete[] (yyvsp[0].string);
    }
#line 2373 "sql_yacc.tab.c"
    break;

  case 89: /* oper_: EQ  */
#line 886 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.oper) = new Operator(Operator::EqualOrAssign);
    }
#line 2381 "sql_yacc.tab.c"
    break;

  case 90: /* oper_: LT  */
#line 890 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.oper) = new Operator(Operator::Less);
    }
#line 2389 "sql_yacc.tab.c"
    break;

  case 91: /* oper_: LE  */
#line 894 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.oper) = new Operator(Operator::LessEqual);
    }
#line 2397 "sql_yacc.tab.c"
    break;

  case 92: /* oper_: GT  */
#line 898 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.oper) = new Operator(Operator::Greater);
    }
#line 2405 "sql_yacc.tab.c"
    break;

  case 93: /* oper_: GE  */
#line 902 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.oper) = new Operator(Operator::GreaterEqual);
    }
#line 2413 "sql_yacc.tab.c"
    break;

  case 94: /* oper_: NE  */
#line 906 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.oper) = new Operator(Operator::NotEqual);
    }
#line 2421 "sql_yacc.tab.c"
    break;

  case 95: /* aggregator: AVG  */
#line 913 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.aggregator) = new Aggregator(Aggregator::AVG);
    }
#line 2429 "sql_yacc.tab.c"
    break;

  case 96: /* aggregator: MAX  */
#line 917 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.aggregator) = new Aggregator(Aggregator::MAX);
    }
#line 2437 "sql_yacc.tab.c"
    break;

  case 97: /* aggregator: MIN  */
#line 921 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.aggregator) = new Aggregator(Aggregator::MIN);
    }
#line 2445 "sql_yacc.tab.c"
    break;

  case 98: /* aggregator: SUM  */
#line 925 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"
    {
        (yyval.aggregator) = new Aggregator(Aggregator::SUM);
    }
#line 2453 "sql_yacc.tab.c"
    break;


#line 2457 "sql_yacc.tab.c"

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
  ++yynerrs;

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
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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

#line 929 "/mnt/e/2022autumn/数据库系统概论/xqldatabase/statement_parse/sql_yacc.y"


int main(){
    executor = new Executor("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql", false);
    cout<<"---- Welcome to XQL database! ---- "<<endl;
    executor->showNextPrefix();
    yyparse();
    delete executor;
    return 0;
}
