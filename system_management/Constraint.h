#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <vector> 
#include <cstring>
#include <iostream>
using namespace std;


class Constraint {
public:
    enum Constraint_Type {
        PRIMARY_KEY = 0,
        FOREIGN_KEY = 1,
        DEFAULT = 2,
        NOT_NULL = 3
    };
    Constraint_Type type;
public:
    Constraint(Constraint_Type type): type(type){}
    Constraint_Type getType() const{
        return this->type;
    }
    void print(){
        cout<<"print Constraint"<<endl;
    };
};

class DefaultConstraint: public Constraint {
public:
    unsigned int kind;   // 0 for int, 1 for float, 2 for char
    DefaultConstraint(unsigned int kind): Constraint(DEFAULT), kind(kind){}
};

class DefaultIntConstraint : public DefaultConstraint {
public:
    int integer;
public:
    DefaultIntConstraint(int integer): DefaultConstraint(0), integer(integer){}
    int getInteger() const{
        return this->integer;
    }
    void print(){
        cout<<"default value: int "<<this->integer;
    }
};

class DefaultFloatConstraint : public DefaultConstraint {
public:
    float floating;

    DefaultFloatConstraint(float floating): DefaultConstraint(1), floating(floating){}
    float getFloating() const{
        return this->floating;
    }
    void print(){
        cout<<"default value: float "<<this->floating;
    }
};

class DefaultCharConstraint : public DefaultConstraint {
public:
    char* str;
    unsigned int len;

    DefaultCharConstraint(const char* str, unsigned int len): DefaultConstraint(2), len(len){
        this->str = new char[len];
        memset(this->str, 0, len);      // reset
        memcpy(this->str, str, strlen(str) < len ? strlen(str):len);
    }
    ~DefaultCharConstraint(){
        if(this->str)
            delete[] this->str;
    }
    const char* getStr() const{
        return this->str;
    }
    unsigned int getLen() const{
        return this->len;
    }
    void print(){
        cout<<"default value: char "<<this->str;
    }
};
    
class ForeignKeyConstraint : public Constraint {
public:
    string name;                
    unsigned int target_table;  // index of target table
    vector<unsigned int> target_columns; // index of target columns
    vector<unsigned int> source_columns; // index of source columns

    ForeignKeyConstraint(string name, unsigned int target_table, vector<unsigned int> target_columns, vector<unsigned int> source_columns): 
        Constraint(FOREIGN_KEY), name(name), target_table(target_table), target_columns(target_columns), source_columns(source_columns){}
    string getName() const{
        return this->name;
    }
    unsigned int getTargetTable() const{
        return this->target_table;
    }
    vector<unsigned int> getTargetColumns() const{
        return this->target_columns;
    }
    vector<unsigned int> getSourceColumns() const{
        return this->source_columns;
    }
    void print(){
        cout<<"foreign key: "<<this->name<<endl;
        cout<<"target table: "<<this->target_table<<endl;
        cout<<"target columns: ";
        for(unsigned int i = 0; i < this->target_columns.size(); i++){
            cout<<this->target_columns[i]<<" ";
        }
        cout<<endl;
        cout<<"source columns: ";
        for(unsigned int i = 0; i < this->source_columns.size(); i++){
            cout<<this->source_columns[i]<<" ";
        }
        cout<<endl;
    }
    // has the same content
    bool operator==(const ForeignKeyConstraint& other) const{
        if(this->target_table != other.target_table)
            return false;
        if(this->target_columns.size() != other.target_columns.size())
            return false;
        for(unsigned int i = 0; i < this->target_columns.size(); i++){
            if(this->target_columns[i] != other.target_columns[i])
                return false;
        }
        if(this->source_columns.size() != other.source_columns.size())
            return false;
        for(unsigned int i = 0; i < this->source_columns.size(); i++){
            if(this->source_columns[i] != other.source_columns[i])
                return false;
        }
        return true;
    }
};

class PrimaryKeyConstraint : public Constraint {
public:
    vector<unsigned int> columns;

    PrimaryKeyConstraint(vector<unsigned int> columns): Constraint(PRIMARY_KEY), columns(columns){}
    vector<unsigned int> getColumns() const{
        return this->columns;
    }
    void print(){
        cout<<"primary key: ";
        for(unsigned int i = 0; i < this->columns.size(); i++){
            cout<<this->columns[i]<<" ";
        }
        cout<<endl;
    }
};

#endif