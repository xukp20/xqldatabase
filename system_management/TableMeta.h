#ifndef RECORDMETA_H
#define RECORDMETA_H

#include <vector>
#include <string>
#include "Constraint.h"
#include "../record_management/Record.h"
using namespace std;

// Column's meta data
class ColumnMeta{
public:
    string name;
    unsigned int kind;
    unsigned int len;
    DefaultConstraint* defaultValue;
    bool notNull;

    ColumnMeta(string name, unsigned int kind, unsigned int len, DefaultConstraint* defaultValue, bool notNull): 
        name(name), kind(kind), len(len), defaultValue(defaultValue), notNull(notNull){
            if(defaultValue != nullptr){
                if(defaultValue->kind != kind){
                    cout<<"default value type not match"<<endl;
                }
            }
        }
    ColumnMeta(){}
    void print(){
        cout<<endl<<"name: "<<this->name<<endl;
        cout<<"type: "<<this->kind<<endl;
        cout<<"len: "<<this->len<<endl; 
        if(this->defaultValue != NULL){
            switch (this->defaultValue->kind)
            {
                case 0:
                    ((DefaultIntConstraint*)this->defaultValue)->print();
                    break;
                case 1:
                    ((DefaultFloatConstraint*)this->defaultValue)->print();
                    break;
                case 2:
                    ((DefaultCharConstraint*)this->defaultValue)->print();
            }
            cout<<endl;
        }
        cout<<"not null: "<<this->notNull<<endl;
    }
    ~ColumnMeta(){
        if(this->defaultValue != NULL){
            delete this->defaultValue;
        }
    }
};

struct DefaultValue{
    unsigned int kind;   // 0 for int, 1 for float, 2 for char
    union{
        int integer;
        float floating;
        char* str;
    };
    ~DefaultValue(){
        if(this->kind == 2){
            if(this->str != nullptr){
                delete this->str;
            }
        }
    }
};

// Table's meta data
class TableMeta {
private:
    string name;
    unsigned int column_num;
    unsigned int foreign_key_num;
    vector<ColumnMeta*> columns;
    vector<ForeignKeyConstraint*> foreignKeys;
    PrimaryKeyConstraint* primaryKey;
    friend class Serializer;
    friend class SystemManager;
    friend class SystemTester;
    friend class Executor;
    friend class QueryManager;
public:
    TableMeta(string name, unsigned int column_num, unsigned int foreign_key_num): 
        name(name), column_num(column_num), foreign_key_num(foreign_key_num){
            this->primaryKey = nullptr;
        }
    TableMeta(){
        this->primaryKey = nullptr;
    }
    ~TableMeta();
    // check if name no duplicate, and type matches the length and default value
    bool addColumn(string name, unsigned int kind, unsigned int len = 4, DefaultValue* defaultValue = nullptr, bool notNull = false);
    bool addColumn(ColumnMeta* column);
    // check if name no duplicate, content different
    // columns numbers matches
    // caller should check if the columns are in the target table and are primary keys
    bool addForeignKey(ForeignKeyConstraint* foreignKey);
    // check if only one Primary key, and the columns exist
    // needs to add all columns before adding foreign key
    bool addPrimaryKey(PrimaryKeyConstraint* primaryKey);

    // get an example record of this table
    Record getExampleRecord();
    void print();
};

#endif