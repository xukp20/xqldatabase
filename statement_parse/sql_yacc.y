%code top{
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
}

%code requires {
#include <cstdio>
#include <string>
#include "SqlStatement.h"
#include "Executor.h"
using namespace std;
}

%union {
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
}

%token <string> IDENTIFIER
%token <string> STRING
%token <integer> INTEGERVAL
%token <floating> FLOATVAL

%token ANNOTATION

%token COUNT AVG SUM MIN MAX
%token SEMICOLON LPAREN RPAREN COMMA DOT ASTERISK
%token CREATE DATABASE DROP SHOW DATABASES USE TABLE TABLES INDEX INDEXES
%token LOAD FROM FILETK TO DUMP 
%token DESC INSERT INTO VALUES DELETE WHERE UPDATE SET
%token SELECT GROUP BY LIMIT OFFSET
%token ALTER ADD PRIMARY KEY FOREIGN CONSTRAINT REFERENCES UNIQUE
%token NOT DEFAULT
%token NULLVAL TOKENINT FLOAT VARCHAR 
%token AND IN IS LIKE
%token EQ LT LE GT GE NE 
%token DONE
%token ERROR

%type <fieldType> type_
%type <field> field
%type <fieldList> field_list
%type <value> value
%type <valueList> values
%type <valueList> value_list
%type <valueLists> value_lists
%type <column> column
%type <oper> oper_
%type <expression> expression
%type <whereClause> where_clause
%type <whereAndClause> where_and_clause
%type <setClause> set_clause
%type <selector> selector
%type <selectors> selector_list
%type <selectors> selectors
%type <identifiers> identifiers
%type <aggregator> aggregator

%start program

%%

program
    : DONE SEMICOLON 
    { YYACCEPT;}
    | statement 
    {}
    | program statement 
    {}
    | program DONE SEMICOLON 
    { YYACCEPT;}
    | program error SEMICOLON { yyerrok; executor->showNextPrefix(); } 
    | error SEMICOLON { yyerrok; executor->showNextPrefix(); }
    ;

statement
    : db_statement SEMICOLON
    | io_statement SEMICOLON
    | table_statement SEMICOLON
    | alter_statement SEMICOLON
    | ANNOTATION SEMICOLON
    | NULLVAL SEMICOLON
    ;

db_statement
    : CREATE DATABASE IDENTIFIER
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::CREATE, string($3));
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("create database failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
        delete[] $3;
    }
    | DROP DATABASE IDENTIFIER
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::DROP, $3);
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop database failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
        delete[] $3;
    }
    | SHOW DATABASES 
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
    | USE IDENTIFIER 
    {
        DBStatement* dbStatement = new DBStatement(DBStatement::USE, $2);
        bool result = executor->run(dbStatement);
        executor->printEnd();
        if(!result){
            yyerror("use database failed");
        }
        executor->showNextPrefix();
        delete dbStatement;
        delete[] $2;
    }
    | SHOW TABLES
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
    | SHOW INDEXES
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
    ;

io_statement
    : LOAD FROM FILETK STRING TO TABLE IDENTIFIER
    {
        IOStatement* statement = new IOStatement(IOStatement::LOAD, $4, $7);
        bool result = executor->run(statement);
        executor->printEnd();
        if(!result){
            yyerror("load table failed");
        }
        executor->showNextPrefix();
        delete statement;
        delete[] $4;
    }
    | DUMP TO FILETK STRING FROM TABLE IDENTIFIER
    {
        IOStatement* statement = new IOStatement(IOStatement::DUMP, $4, $7);
        bool result = executor->run(statement);
        executor->printEnd();
        if(!result){
            yyerror("dump table failed");
        }
        executor->showNextPrefix();
        delete statement;
        delete[] $4;
    }
    ;

table_statement
    : CREATE TABLE IDENTIFIER LPAREN field_list RPAREN
    {
        TableStatement* tableStatement = new CreateTableStatement(string($3), $5);
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("create table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] $3;
    }
    | DROP TABLE IDENTIFIER
    {
        TableStatement* tableStatement = new DropTableStatement(string($3));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] $3;
    }
    | DESC IDENTIFIER
    {
        TableStatement* tableStatement = new DescTableStatement(string($2));
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("desc table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] $2;
    }
    | INSERT INTO IDENTIFIER VALUES value_lists
    {
        TableStatement* tableStatement = new InsertStatement(string($3), $5);
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("insert into table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] $3;
    }
    | DELETE FROM IDENTIFIER WHERE where_and_clause
    {
        TableStatement* tableStatement = new DeleteStatement(string($3), $5);
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("delete from table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] $3;
    }
    | UPDATE IDENTIFIER SET set_clause WHERE where_and_clause
    {
        // cout<<"1"<<endl;
        TableStatement* tableStatement = new UpdateStatement(string($2), $4, $6);
        bool result = executor->run(tableStatement);
        executor->printEnd();
        if(!result){
            yyerror("update table failed");
        }
        executor->showNextPrefix();
        delete tableStatement;
        delete[] $2;
    }
    | select_table {}
    ;

select_table
    : SELECT selectors FROM identifiers
    {
        SelectStatement* selectStatement = new SelectStatement($2, $4);
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
    | SELECT selectors FROM identifiers WHERE where_and_clause
    {
        SelectStatement* selectStatement = new SelectStatement($2, $4);
        selectStatement->setWhere($6);
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
    | SELECT selectors FROM identifiers WHERE where_and_clause LIMIT INTEGERVAL  
    {
        SelectStatement* selectStatement = new SelectStatement($2, $4);
        selectStatement->setWhere($6);
        selectStatement->setLimit($8);
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
    | SELECT selectors FROM identifiers WHERE where_and_clause LIMIT INTEGERVAL OFFSET INTEGERVAL
    {
        SelectStatement* selectStatement = new SelectStatement($2, $4);
        selectStatement->setWhere($6);
        selectStatement->setLimit($8);
        selectStatement->setOffset($10);
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
    | SELECT selectors FROM identifiers LIMIT INTEGERVAL
    {
        SelectStatement* selectStatement = new SelectStatement($2, $4);
        selectStatement->setLimit($6);
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
    | SELECT selectors FROM identifiers LIMIT INTEGERVAL OFFSET INTEGERVAL
    {
        SelectStatement* selectStatement = new SelectStatement($2, $4);
        selectStatement->setLimit($6);
        selectStatement->setOffset($8);
        bool result = executor->run(selectStatement);
        executor->printEnd();
        if(!result){
            yyerror("select from table failed");
        }
        executor->showNextPrefix();
        delete selectStatement;
    }
    ;
// no group by for now

alter_statement
    : ALTER TABLE IDENTIFIER ADD INDEX LPAREN identifiers RPAREN
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::ADD, AlterStatement::INDEX, string($3));
        for(int i=0; i<$7->identifiers.size(); i++){
            alterStatement->addColumn($7->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add index failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete $7;
    }
    | ALTER TABLE IDENTIFIER DROP INDEX LPAREN identifiers RPAREN
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::DROP, AlterStatement::INDEX, string($3));
        for(int i=0; i<$7->identifiers.size(); i++){
            alterStatement->addColumn($7->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop index failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete $7;
    }
    | ALTER TABLE IDENTIFIER DROP PRIMARY KEY
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::DROP, AlterStatement::PRIMARY, string($3));
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
    }
    | ALTER TABLE IDENTIFIER DROP PRIMARY KEY IDENTIFIER
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::DROP, AlterStatement::PRIMARY, string($3));
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete[] $7;
    }
    | ALTER TABLE IDENTIFIER DROP FOREIGN KEY IDENTIFIER
    {
        AlterStatement* alterStatement = new AlterForeignKeyStatement(AlterStatement::DROP, string($3), string($7));
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("drop foreign key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete[] $7;
    }
    | ALTER TABLE IDENTIFIER ADD CONSTRAINT PRIMARY KEY LPAREN identifiers RPAREN
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::ADD, AlterStatement::PRIMARY, string($3));
        for(int i=0; i<$9->identifiers.size(); i++){
            alterStatement->addColumn($9->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete $9;
    }
    | ALTER TABLE IDENTIFIER ADD CONSTRAINT IDENTIFIER PRIMARY KEY LPAREN identifiers RPAREN
    {
        AlterStatement* alterStatement = new AlterStatement(AlterStatement::ADD, AlterStatement::PRIMARY, string($3));
        for(int i=0; i<$10->identifiers.size(); i++){
            alterStatement->addColumn($10->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add primary key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete[] $6;
        delete $10;
    }
    | ALTER TABLE IDENTIFIER ADD CONSTRAINT FOREIGN KEY LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN
    {  
        AlterStatement* alterStatement = new AlterForeignKeyStatement(AlterStatement::ADD, string($3), string(""), string($12));
        for(int i=0; i<$9->identifiers.size(); i++){
            alterStatement->addColumn($9->identifiers[i]);
        }
        for(int i=0; i<$14->identifiers.size(); i++){
            ((AlterForeignKeyStatement*)alterStatement)->addTargetColumn($14->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add foreign key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete[] $12;
        delete $9;
        delete $14;
    }
    | ALTER TABLE IDENTIFIER ADD CONSTRAINT IDENTIFIER FOREIGN KEY LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN
    {  
        AlterStatement* alterStatement = new AlterForeignKeyStatement(AlterStatement::ADD, string($3), string($6), string($13));
        for(int i=0; i<$10->identifiers.size(); i++){
            alterStatement->addColumn($10->identifiers[i]);
        }
        for(int i=0; i<$15->identifiers.size(); i++){
            ((AlterForeignKeyStatement*)alterStatement)->addTargetColumn($15->identifiers[i]);
        }
        bool result = executor->run(alterStatement);
        executor->printEnd();
        if(!result){
            yyerror("add foreign key failed");
        }
        executor->showNextPrefix();
        delete alterStatement;
        delete[] $3;
        delete[] $6;
        delete[] $13;
        delete $10;
        delete $15;
    }
    | ALTER TABLE IDENTIFIER ADD UNIQUE LPAREN identifiers RPAREN
    {
        cout<<"add unique not implemented"<<endl;
        executor->showNextPrefix();
        delete[] $3;
        delete $7;
    }

field_list
    : field
    {
        $$ = new FieldList($1);
    }
    | field_list COMMA field
    {
        $$ = $1;
        $1->add($3);
    }
    ;

field
    : IDENTIFIER type_
    {
        string column_name = $1;
        unsigned int kind = ($2->type == FieldType::INT) ? 0 : ($2->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = $2->length;
        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, nullptr, false);
        $$ = new Field(columnMeta);
        delete[] $1;
    }
    | IDENTIFIER type_ NOT NULLVAL
    {
        string column_name = $1;
        unsigned int kind = ($2->type == FieldType::INT) ? 0 : ($2->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = $2->length;
        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, nullptr, true);
        $$ = new Field(columnMeta);
        delete[] $1;
    }
    | IDENTIFIER type_ DEFAULT value
    {
        string column_name = $1;
        unsigned int kind = ($2->type == FieldType::INT) ? 0 : ($2->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = $2->length;
        unsigned int default_kind = ($4->value_type == Value::INT) ? 0 : 
                                    ($4->value_type == Value::FLOAT) ? 1 : 
                                    ($4->value_type == Value::STRING) ? 2 : 3;
        unsigned int default_len = 4;
        if(default_kind == 2){  // check len
            default_len = $4->string_value.length();
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
                defaultConstraint = new DefaultIntConstraint($4->int_value);
                break;
            
            case 1:
                defaultConstraint = new DefaultFloatConstraint($4->float_value);
                break;

            case 2:
                defaultConstraint = new DefaultCharConstraint($4->string_value.c_str(), default_len);
                break;
            
            case 3:     // Default Null
                defaultConstraint = new DefaultConstraint(3);
                break;

            default:
                break;
        }

        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, defaultConstraint, false);
        $$ = new Field(columnMeta);
        delete[] $1;
    }
    | IDENTIFIER type_ NOT NULLVAL DEFAULT value
    {
        string column_name = $1;
        unsigned int kind = ($2->type == FieldType::INT) ? 0 : ($2->type == FieldType::FLOAT) ? 1 : 2;
        unsigned int len = $2->length;
        unsigned int default_kind = ($6->value_type == Value::INT) ? 0 : 
                                    ($6->value_type == Value::FLOAT) ? 1 : 
                                    ($6->value_type == Value::STRING) ? 2 : 3;
        unsigned int default_len = 4;
        if(default_kind == 2){  // check len
            default_len = $6->string_value.length();
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
                defaultConstraint = new DefaultIntConstraint($6->int_value);
                break;
            
            case 1:
                defaultConstraint = new DefaultFloatConstraint($6->float_value);
                break;

            case 2:
                defaultConstraint = new DefaultCharConstraint($6->string_value.c_str(), default_len);
                break;

            default:
                break;
        }

        ColumnMeta* columnMeta = new ColumnMeta(column_name, kind, len, defaultConstraint, true);
        $$ = new Field(columnMeta);
        delete[] $1;
    }
    | PRIMARY KEY LPAREN identifiers RPAREN
    {
        $$ = new Field($4);
    }
    | FOREIGN KEY IDENTIFIER LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN
    {
        ForeignKeyField* foreignKeyField = new ForeignKeyField(string($3), string($8), $10, $5);
        $$ = new Field(foreignKeyField);
        delete[] $3;
    }
    | FOREIGN KEY LPAREN identifiers RPAREN REFERENCES IDENTIFIER LPAREN identifiers RPAREN
    {
        ForeignKeyField* foreignKeyField = new ForeignKeyField(string(""), string($7), $9, $4);
        $$ = new Field(foreignKeyField);
        delete[] $7;
    }
    ;

type_
    : TOKENINT
    {
        $$ = new FieldType(FieldType::INT, 4);
    }
    | FLOAT
    {
        $$ = new FieldType(FieldType::FLOAT, 4);
    }
    | VARCHAR LPAREN INTEGERVAL RPAREN
    {
        $$ = new FieldType(FieldType::VARCHAR, $3);
    }
    ;

value_lists
    : value_list
    {
        $$ = new ValueLists($1);
    }
    | value_lists COMMA value_list
    {
        $$ = $1;
        $1->add($3);
    }
    ;

value_list
    : LPAREN values RPAREN
    {
        $$ = $2;
    }
    ;

values
    : value
    {
        $$ = new ValueList($1);
    }
    | values COMMA value
    {
        $$ = $1;
        $1->add($3);
    }
    ;

value
    : INTEGERVAL
    {
        $$ = new Value($1);
    }
    | FLOATVAL
    {
        $$ = new Value(float($1));
    }
    | STRING
    {
        // cout<<"STRING"<<endl;
        // cout<<$1<<endl;
        // cout<<"."<<$1[0]<<"."<<$1[1]<<"."<<$1[2]<<"."<<endl;
        $$ = new Value(string($1));
        delete[] $1;
        // cout<<"after"<<endl;
    }
    | NULLVAL
    {
        $$ = new Value();
    }
    ;

where_and_clause
    : where_clause
    {
        $$ = new WhereAndClause($1);
    }
    | where_and_clause AND where_clause
    {
        $$ = $1;
        $1->add($3);
    }
    ;

where_clause
    : column oper_ expression
    {
        $$ = new WhereOperatorExpr($1, $2, $3);
    }
    // | column oper_ LPAREN select_table RPAREN
    // {
    //     $$ = new WhereOperatorSelect($1, $2, $4);
    // }
    | column IS NULLVAL
    {
        $$ = new WhereIsNull($1, true);
    }
    | column IS NOT NULLVAL
    {
        $$ = new WhereIsNull($1, false);
    }
    | column IN value_list
    {
        $$ = new WhereInList($1, $3);
    }
    // | column IN LPAREN select_table RPAREN
    // {
    //     $$ = new WhereInSelect($1, $4);
    // }
    | column LIKE STRING
    {
        $$ = new WhereLike($1, $3);
        delete[] $3;
    }
    ;

column
    : IDENTIFIER
    {
        $$ = new Column($1);
        delete[] $1;
    }
    | IDENTIFIER DOT IDENTIFIER
    {
        $$ = new Column($1, $3);
        delete[] $1;
        delete[] $3;
    }
    ;

expression
    : value
    {
        $$ = new Expression($1);
    }
    | column
    {
        $$ = new Expression($1);
    }
    ;

set_clause
    : IDENTIFIER EQ value 
    {
        cout<<"set_clause: IDENTIFIER EQ value"<<endl;
        SetValue* setValue = new SetValue($1, $3);
        $$ = new SetClause(setValue);
        delete[] $1;
    }
    | set_clause COMMA IDENTIFIER EQ value
    {
        SetValue* setValue = new SetValue($3, $5);
        $$ = $1;
        $1->add(setValue);
        delete[] $3;
    }

selectors
    : ASTERISK
    {
        $$ = new Selectors(true);   // selector for all
    }
    | selector_list
    {
        $$ = $1;
    }
    ;

selector_list
    : selector
    {
        $$ = new Selectors($1);
    }
    | selector_list COMMA selector
    {
        $$ = $1;
        $1->add($3);
    }
    ;

selector
    : column
    {
        $$ = new ColumnSelector($1);
    }
    | aggregator LPAREN column RPAREN
    {
        $$ = new AggregatorSelector($1, $3);
    }
    | COUNT LPAREN ASTERISK RPAREN
    {
        Aggregator* aggregator = new Aggregator(Aggregator::COUNT);
        $$ = new AggregatorSelector(aggregator, nullptr);
    }
    | COUNT LPAREN column RPAREN
    {
        Aggregator* aggregator = new Aggregator(Aggregator::COUNT);
        $$ = new AggregatorSelector(aggregator, $3);
    }
    ;

identifiers
    : IDENTIFIER
    {
        $$ = new Identifiers($1);
        delete[] $1;
    }
    | identifiers COMMA IDENTIFIER
    {
        $$ = $1;
        $1->add($3);
        delete[] $3;
    }
    ;

oper_
    : EQ
    {
        $$ = new Operator(Operator::EqualOrAssign);
    }
    | LT
    {
        $$ = new Operator(Operator::Less);
    }
    | LE
    {
        $$ = new Operator(Operator::LessEqual);
    }
    | GT
    {
        $$ = new Operator(Operator::Greater);
    }
    | GE
    {
        $$ = new Operator(Operator::GreaterEqual);
    }
    | NE
    {
        $$ = new Operator(Operator::NotEqual);
    }
    ;

aggregator
    : AVG
    {
        $$ = new Aggregator(Aggregator::AVG);
    }
    | MAX
    {
        $$ = new Aggregator(Aggregator::MAX);
    }
    | MIN
    {
        $$ = new Aggregator(Aggregator::MIN);
    }
    | SUM
    {
        $$ = new Aggregator(Aggregator::SUM);
    }
    ;
%%

int main(){
    executor = new Executor("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql", false);
    cout<<"---- Welcome to XQL database! ---- "<<endl;
    executor->showNextPrefix();
    yyparse();
    delete executor;
    return 0;
}