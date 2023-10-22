/*
    classes of fragments of SQL statements
*/

#ifndef SQL_STATEMENT_H
#define SQL_STATEMENT_H

#include <string>
#include <vector>
using namespace std;


// statements
class DoneStatement
{
public:
    DoneStatement(){};
    ~DoneStatement(){};
};

/* DB statement */
class DBStatement
{
public:
    enum StatementType
    {
        CREATE,
        DROP,
        USE,
        SHOWDBS,
        SHOWTABLES,
        SHOWINDEXES,
    };
    StatementType type;
    std::string identifier;

    DBStatement(StatementType type, std::string identifier=""){
        this->type = type;
        this->identifier = identifier;
    };
    ~DBStatement(){};
};

/* IO Statements */
class IOStatement
{
public:
    enum StatementType
    {
        LOAD,
        DUMP
    };
    StatementType type;
    string file_path;
    string table_name;

    IOStatement(StatementType type, string file_path, string table_name){
        this->type = type;
        this->file_path = file_path;
        this->table_name = table_name;
    };
};

/* Table Statements */
class TableStatement
{
public:
    enum StatementType
    {
        CREATE,
        DROP,
        DESC,
        INSERT,
        DELETE,
        SELECT,
        UPDATE,
    };
    StatementType type;

    TableStatement(StatementType type){
        this->type = type;
    };
};

// create table
#include "../system_management/TableMeta.h"
class FieldType{
public:
    enum Type{
        INT,
        FLOAT,
        VARCHAR
    };
    Type type;
    int length;

    FieldType(Type type, int length=4){
        this->type = type;
        this->length = length;
    }
};

class Identifiers{
public:
    vector<string> identifiers;
    Identifiers(string identifier){
        identifiers.push_back(identifier);
    }
    void add(string identifier){
        identifiers.push_back(identifier);
    }
    ~Identifiers(){
        identifiers.clear();
    }
    void print(){
        cout<<"identifiers: ";
        for(int i=0; i<identifiers.size(); i++){
            cout<<identifiers[i]<<" ";
        }
        cout<<endl;
    }
};

class ForeignKeyField{
public:
    string fk_name;
    string target_table;
    Identifiers* target_columns;
    Identifiers* source_columns;

    ForeignKeyField(string fk_name, string target_table, Identifiers* target_columns, Identifiers* source_columns){
        this->fk_name = fk_name;
        this->target_table = target_table;
        this->target_columns = target_columns;
        this->source_columns = source_columns;
    }
    ~ForeignKeyField(){
        delete target_columns;
        delete source_columns;
    }
    void print(){
        cout<<"foreign key: "<<fk_name<<endl;
        cout<<"target table: "<<target_table<<endl;
        cout<<"target columns: ";
        target_columns->print();
        cout<<"source columns: ";
        source_columns->print();
    }
};

class Field{
public:
    union{
        ColumnMeta* column_meta;
        ForeignKeyField* foreign_key_constraint;
        Identifiers* primary_key_constraint;    // primary key constraint is a list of column names
    };

    enum FieldType{
        COLUMN,
        FOREIGN_KEY,
        PRIMARY_KEY
    };

    FieldType field_type;

    Field(ColumnMeta* column_meta){
        this->column_meta = column_meta;
        this->field_type = COLUMN;
    }
    Field(ForeignKeyField* foreign_key_constraint){
        this->foreign_key_constraint = foreign_key_constraint;
        this->field_type = FOREIGN_KEY;
    }
    Field(Identifiers* primary_key_constraint){
        this->primary_key_constraint = primary_key_constraint;
        this->field_type = PRIMARY_KEY;
    }

    ~Field(){
        switch(field_type){
            case COLUMN:
                delete column_meta;
                break;
            case FOREIGN_KEY:
                delete foreign_key_constraint;
                break;
            case PRIMARY_KEY:
                delete primary_key_constraint;
                break;
        }
    }

    void print(){
        switch(field_type){
            case COLUMN:
                column_meta->print();
                break;
            case FOREIGN_KEY:
                foreign_key_constraint->print();
                break;
            case PRIMARY_KEY:
                cout<<"primary key: ";
                primary_key_constraint->print();
                break;
        }
    }
};

class FieldList{
public:
    vector<Field*> fields;
    FieldList(Field* field){
        fields.push_back(field);
    }
    void add(Field* field){
        fields.push_back(field);
    }
    ~FieldList(){
        for(int i=0; i<fields.size(); i++){
            delete fields[i];
        }
        fields.clear();
    }
    void print(){
        cout<<"field list: "<<endl;
        for(int i=0; i<fields.size(); i++){
            fields[i]->print();
        }
    }
};


class CreateTableStatement : public TableStatement
{
public:
    string table_name;
    FieldList* field_list;

    CreateTableStatement(string table_name, FieldList* field_list) : TableStatement(TableStatement::CREATE){
        this->table_name = table_name;
        this->field_list = field_list;
    };
    ~CreateTableStatement(){
        delete field_list;
    }
    void print(){
        cout<<"create table: "<<table_name<<endl;
        field_list->print();
    }
};

// drop table
class DropTableStatement : public TableStatement
{
public:
    string table_name;

    DropTableStatement(string table_name) : TableStatement(TableStatement::DROP){
        this->table_name = table_name;
    };
};

// desc table
class DescTableStatement : public TableStatement
{
public:
    string table_name;

    DescTableStatement(string table_name) : TableStatement(TableStatement::DESC){
        this->table_name = table_name;
    };
};

// insert
class Value{
public:
    enum ValueType{
        INT,
        FLOAT,
        STRING,
        NULLVAL
    };
    ValueType value_type;
    union{
        int int_value;
        float float_value;
    };
    string string_value;

    Value(int int_value){
        this->int_value = int_value;
        this->value_type = INT;
    }
    Value(float float_value){
        this->float_value = float_value;
        this->value_type = FLOAT;
    }
    Value(string string_value){
        this->string_value = string_value;
        this->value_type = STRING;
    }
    Value(const char* string_value){
        cout<<"const char*"<<endl;
        // this->string_value = string_value;
        this->string_value = string(string_value);
        this->value_type = STRING;
        // cout<<"after construct"<<endl;
    }
    Value(){
        this->value_type = NULLVAL;
    }
    ~Value(){
    }
    void print(){
        switch(value_type){
            case INT:
                cout<<int_value;
                break;
            case FLOAT:
                cout<<float_value;
                break;
            case STRING:
                cout<<string_value;
                break;
            case NULLVAL:
                cout<<"NULL";
                break;
        }
    }
};

class ValueList{
public:
    vector<Value*> values;
    ValueList(Value* value){
        values.push_back(value);
    }
    void add(Value* value){
        values.push_back(value);
    }
    ~ValueList(){
        for(int i=0; i<values.size(); i++){
            delete values[i];
        }
    }
    void print(){
        cout<<"value list: ";
        for(int i=0; i<values.size(); i++){
            values[i]->print();
            cout<<" ";
        }
        cout<<endl;
    }
};

class ValueLists{
public:
    vector<ValueList*> value_lists;
    ValueLists(ValueList* value_list){
        value_lists.push_back(value_list);
    }
    void add(ValueList* value_list){
        value_lists.push_back(value_list);
    }
    ~ValueLists(){
        for(int i=0; i<value_lists.size(); i++){
            delete value_lists[i];
        }
    }
    void print(){
        cout<<"value lists: "<<endl;
        for(int i=0; i<value_lists.size(); i++){
            value_lists[i]->print();
        }
    }
};

class InsertStatement : public TableStatement
{
public:
    string table_name;
    ValueLists* value_lists;

    InsertStatement(string table_name, ValueLists* value_lists) : TableStatement(TableStatement::INSERT){
        this->table_name = table_name;
        this->value_lists = value_lists;
    };
    ~InsertStatement(){
        delete value_lists;
    }
    void print(){
        cout<<"insert into table: "<<table_name<<endl;
        value_lists->print();
    }
};

// delete
class Column{
public:
    string table_name;
    string column_name;
    Column(string table_name, string column_name){
        this->table_name = table_name;
        this->column_name = column_name;
    }
    Column(string column_name){
        this->table_name = "";
        this->column_name = column_name;
    }
    void print(){
        cout<<table_name<<"."<<column_name;
    }
};

class Operator{
public:
    enum OperatorType{
        EqualOrAssign,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
        NotEqual
    };
    OperatorType operator_type;
    Operator(OperatorType operator_type){
        this->operator_type = operator_type;
    }
    void print(){
        switch(operator_type){
            case EqualOrAssign:
                cout<<"=";
                break;
            case Less:
                cout<<"<";
                break;
            case LessEqual:
                cout<<"<=";
                break;
            case Greater:
                cout<<">";
                break;
            case GreaterEqual:
                cout<<">=";
                break;
            case NotEqual:
                cout<<"!=";
                break;
        }
    }
};

class Expression{
public:
    union{
        Column* column;
        Value* value;
    };
    enum ExpressionType{
        COLUMN,
        VALUE
    };
    ExpressionType expression_type;
    Expression(Column* column){
        this->column = column;
        this->expression_type = COLUMN;
    }
    Expression(Value* value){
        this->value = value;
        this->expression_type = VALUE;
    }
    ~Expression(){
        if(expression_type == COLUMN){
            delete column;
        }
        else{
            delete value;
        }
    }
    void print(){
        if(expression_type == COLUMN){
            column->print();
        }
        else{
            value->print();
        }
    }
};

// where clause, pass select_table for now
class WhereClause{
public:
    enum Type{
        OperatorExpr,
        OperatorSelect,
        IsNull,
        InList,
        InSelect,
        Like
    };
    Type type;
    Column* column;

    WhereClause(Type type, Column* column){
        this->type = type;
        this->column = column;
    }
    ~WhereClause(){
        delete column;
    }
};

class WhereOperatorExpr : public WhereClause{
public:
    Operator* op;
    Expression* expression;
    WhereOperatorExpr(Column* column, Operator* op, Expression* expression) : WhereClause(WhereClause::OperatorExpr, column){
        this->op = op;
        this->expression = expression;
    }
    ~WhereOperatorExpr(){
        delete op;
        delete expression;
    }
    void print(){
        column->print();
        cout<<" ";
        op->print();
        cout<<" ";
        expression->print();
        cout<<endl;
    }
};

// class whereOperatorSelect

class WhereIsNull : public WhereClause{
public:
    bool is_null;
    WhereIsNull(Column* column, bool is_null) : WhereClause(WhereClause::IsNull, column){
        this->is_null = is_null;
    }
    void print(){
        column->print();
        cout<<" ";
        if(is_null){
            cout<<"is null"<<endl;
        }
        else{
            cout<<"is not null"<<endl;
        }
    }
};

class WhereInList : public WhereClause{
public:
    ValueList* value_list;
    WhereInList(Column* column, ValueList* value_list) : WhereClause(WhereClause::InList, column){
        this->value_list = value_list;
    }
    ~WhereInList(){
        delete value_list;
    }
    void print(){
        column->print();
        cout<<" in ";
        value_list->print();
    }
};

// class whereInSelect

class WhereLike : public WhereClause{
public:
    string pattern;
    WhereLike(Column* column, string pattern) : WhereClause(WhereClause::Like, column){
        this->pattern = pattern;
    }
    void print(){
        column->print();
        cout<<" like "<<pattern<<endl;
    }
};

class WhereAndClause{
public:
    vector<WhereClause*> where_clauses;
    WhereAndClause(WhereClause* where_clause){
        where_clauses.push_back(where_clause);
    }
    void add(WhereClause* where_clause){
        where_clauses.push_back(where_clause);
    }
    ~WhereAndClause(){
        for(int i=0; i<where_clauses.size(); i++){
            delete where_clauses[i];
        }
    }
    void print(){
        for(int i=0; i<where_clauses.size(); i++){
            switch(where_clauses[i]->type){
                case WhereClause::OperatorExpr:
                    ((WhereOperatorExpr*)where_clauses[i])->print();
                    break;
                case WhereClause::IsNull:
                    ((WhereIsNull*)where_clauses[i])->print();
                    break;
                case WhereClause::InList:
                    ((WhereInList*)where_clauses[i])->print();
                    break;
                case WhereClause::Like:
                    ((WhereLike*)where_clauses[i])->print();
                    break;
            }
        }
    }
};

class DeleteStatement : public TableStatement
{
public:
    string table_name;
    WhereAndClause* where_and_clause;

    DeleteStatement(string table_name, WhereAndClause* where_and_clause) : TableStatement(TableStatement::DELETE){
        this->table_name = table_name;
        this->where_and_clause = where_and_clause;
    };
    ~DeleteStatement(){
        delete where_and_clause;
    }
    void print(){
        cout<<"delete from "<<table_name<<endl;
        where_and_clause->print();
    }
};

// update
class SetValue{
public:
    string column_name;
    Value* value;
    SetValue(string column_name, Value* value){
        this->column_name = column_name;
        this->value = value;
    }
    ~SetValue(){
        delete value;
    }
    void print(){
        cout<<column_name<<" = ";
        value->print();
        cout<<endl;
    }
};

class SetClause{
public:
    vector<SetValue*> set_values;
    SetClause(SetValue* set_value){
        set_values.push_back(set_value);
    }
    void add(SetValue* set_value){
        set_values.push_back(set_value);
    }
    ~SetClause(){
        for(int i=0; i<set_values.size(); i++){
            delete set_values[i];
        }
    }
    void print(){
        for(int i=0; i<set_values.size(); i++){
            set_values[i]->print();
        }
    }
};

class UpdateStatement : public TableStatement
{
public:
    string table_name;
    SetClause* set_clause;
    WhereAndClause* where_and_clause;

    UpdateStatement(string table_name, SetClause* set_clause, WhereAndClause* where_and_clause) : TableStatement(TableStatement::UPDATE){
        this->table_name = table_name;
        this->set_clause = set_clause;
        this->where_and_clause = where_and_clause;
    };
    ~UpdateStatement(){
        delete set_clause;
        delete where_and_clause;
    }
    void print(){
        cout<<"update "<<table_name<<endl;
        set_clause->print();
        where_and_clause->print();
    }
};

// select
class Selector{
public:
    enum SelectorType{
        COLUMN,
        AGGREGATOR,
        COUNT
    };
    SelectorType selector_type;
    Selector(SelectorType selector_type){
        this->selector_type = selector_type;
    }
};

class ColumnSelector : public Selector{
public:
    Column* column;
    ColumnSelector(Column* column) : Selector(Selector::COLUMN){
        this->column = column;
    }
    ~ColumnSelector(){
        delete column;
    }
    void print(){
        column->print();
    }
};

// aggregator
class Aggregator{
public:
    enum AggregatorType{
        SUM,
        AVG,
        MIN,
        MAX,
        COUNT
    };
    AggregatorType aggregator_type;
    Aggregator(AggregatorType aggregator_type){
        this->aggregator_type = aggregator_type;
    }
    void print(){
        switch(aggregator_type){
            case SUM:
                cout<<"sum";
                break;
            case AVG:
                cout<<"avg";
                break;
            case MIN:
                cout<<"min";
                break;
            case MAX:
                cout<<"max";
                break;
            case COUNT:
                cout<<"count";
                break;
        }
    }
};

class AggregatorSelector : public Selector{
public:
    Aggregator* aggregator;
    Column* column;     // column = nullptr for * and COUNT(*)
    AggregatorSelector(Aggregator* aggregator, Column* column) : Selector(Selector::AGGREGATOR){
        this->aggregator = aggregator;
        this->column = column;
    }
    ~AggregatorSelector(){
        delete aggregator;
        delete column;
    }
    void print(){
        aggregator->print();
        cout<<"(";
        if(column != nullptr){
            column->print();
        }
        cout<<")";
    }
};

class Selectors{
public:
    vector<Selector*> selectors;
    bool all;
    Selectors(Selector* selector){
        selectors.push_back(selector);
        all = false;
    }
    void add(Selector* selector){
        selectors.push_back(selector);
        all = false;
    }
    Selectors(bool all){
        this->all = all;
    }
    ~Selectors(){
        for(int i=0; i<selectors.size(); i++){
            delete selectors[i];
        }
    }
    void print(){
        for(int i=0; i<selectors.size(); i++){
            switch(selectors[i]->selector_type){
                case Selector::COLUMN:
                    ((ColumnSelector*)selectors[i])->print();
                    break;
                case Selector::AGGREGATOR:
                    ((AggregatorSelector*)selectors[i])->print();
                    break;
            }
            cout<<", ";
        }
    }
};

class SelectStatement : public TableStatement
{
public:
    Selectors* selectors;
    Identifiers* identifiers;
    WhereAndClause* where_and_clause;
    Column* group_by_column;
    int limit;
    int offset;
    bool hasWhere;
    bool hasGroupBy;
    bool hasLimit;
    bool hasOffset;
    SelectStatement(Selectors* selectors, Identifiers* identifiers) : TableStatement(TableStatement::SELECT){
        this->selectors = selectors;
        this->identifiers = identifiers;
        this->where_and_clause = nullptr;
        this->group_by_column = nullptr;
        this->limit = 0;
        this->offset = 0;
        this->hasWhere = false;
        this->hasGroupBy = false;
        this->hasLimit = false;
        this->hasOffset = false;
    };
    void setWhere(WhereAndClause* where_and_clause){
        this->where_and_clause = where_and_clause;
        this->hasWhere = true;
    }
    void setGroupBy(Column* group_by_column){
        this->group_by_column = group_by_column;
        this->hasGroupBy = true;
    }
    void setLimit(int limit){
        this->limit = limit;
        this->hasLimit = true;
    }
    void setOffset(int offset){
        this->offset = offset;
        this->hasOffset = true;
    }
    ~SelectStatement(){
        delete selectors;
        delete identifiers;
        if(where_and_clause != nullptr){
            delete where_and_clause;
        }
        if(group_by_column != nullptr){
            delete group_by_column;
        }
    }
    void print(){
        cout<<"select ";
        if(selectors->all){
            cout<<"* ";
        }else{
            selectors->print();
        }
        cout<<"from ";
        identifiers->print();
        if(hasWhere){
            cout<<"where ";
            where_and_clause->print();
        }
        if(hasGroupBy){
            cout<<"group by ";
            group_by_column->print();
        }
        if(hasLimit){
            cout<<"limit "<<limit;
        }
        if(hasOffset){
            cout<<"offset "<<offset;
        }
        cout<<endl;
    }
};

// alter statements
class AlterStatement
{
public:
    enum AlterType{
        ADD,
        DROP
    };
    enum TargetType{
        INDEX,
        PRIMARY,
        FOREIGN,
        UNIQUE  // not implemented
    };
    AlterType alter_type;
    TargetType target_type;
    string table_name;
    vector<string> column_names;
    AlterStatement(AlterType alter_type, TargetType target_type, string table_name){
        this->alter_type = alter_type;
        this->target_type = target_type;
        this->table_name = table_name;
    }
    void addColumn(string column_name){
        column_names.push_back(column_name);
    }
    ~AlterStatement(){
    }
    void print(){
        switch(alter_type){
            case ADD:
                cout<<"add ";
                break;
            case DROP:
                cout<<"drop ";
                break;
        }
        switch(target_type){
            case INDEX:
                cout<<"index ";
                break;
            case PRIMARY:
                cout<<"primary key ";
                break;
            case FOREIGN:
                cout<<"foreign key ";
                break;
            case UNIQUE:
                cout<<"unique ";
                break;
        }
        cout<<table_name<<"(";
        for(int i=0; i<column_names.size(); i++){
            cout<<column_names[i];
            if(i != column_names.size()-1){
                cout<<", ";
            }
        }
        cout<<") ";
    }
};

class AlterForeignKeyStatement : public AlterStatement
{
public:
    string fk_name;
    string target_table_name;
    vector<string> target_column_names;
    AlterForeignKeyStatement(AlterType alter_type, string table_name, string fk_name, string target_table_name=string("")) : AlterStatement(alter_type, FOREIGN, table_name){
        this->fk_name = fk_name;
        this->target_table_name = target_table_name;
    }
    ~AlterForeignKeyStatement(){
    }
    void addTargetColumn(string target_column_name){
        target_column_names.push_back(target_column_name);
    }
    void print(){
        AlterStatement::print();
        cout<<fk_name<<" ";
        cout<<"references "<<target_table_name<<"(";
        for(int i=0; i<target_column_names.size(); i++){
            cout<<target_column_names[i];
            if(i != target_column_names.size()-1){
                cout<<", ";
            }
        }
        cout<<")"<<endl;
    }
};



#endif 