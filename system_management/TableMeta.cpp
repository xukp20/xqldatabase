#include "TableMeta.h"
#include "Constraint.h"

using namespace std;

TableMeta::~TableMeta(){
    for(int i = 0; i < this->columns.size(); i++){
        if (this->columns[i] != nullptr){
            delete this->columns[i];
        }
    }
    this->columns.clear();

    for(int i = 0; i < this->foreignKeys.size(); i++){
        if (this->foreignKeys[i] != nullptr){
            delete this->foreignKeys[i];
        }
    }
    this->foreignKeys.clear();

    if(this->primaryKey != nullptr){
        delete this->primaryKey;
        this->primaryKey = nullptr;
    }
}

bool TableMeta::addColumn(string name, unsigned int kind, unsigned int len, DefaultValue* defaultValue, bool notNull){
    // check if name no duplicate
    for(int i = 0; i < this->columns.size(); i++){
        if(this->columns[i]->name == name){
            cout<<"column name duplicate"<<endl;
            return false;
        }
    }
    // check if type matches the length and default value
    if(defaultValue != nullptr && kind != defaultValue->kind){
        cout<<"default value type not match"<<endl;
        return false;
    }
    if(kind == 0 && len != 4){
        cout<<"int type length not match"<<endl;
        return false;
    }
    if(kind == 1 && len != 4){
        cout<<"float type length not match"<<endl;
        return false;
    }

    DefaultConstraint* defaultConstraint = nullptr;
    if(defaultValue != nullptr){
        if(kind == 0){
            defaultConstraint = new DefaultIntConstraint(defaultValue->integer);
        }
        else if(kind == 1){
            defaultConstraint = new DefaultFloatConstraint(defaultValue->floating);
        }
        else if(kind == 2){
            defaultConstraint = new DefaultCharConstraint(defaultValue->str, len);
        }
    }

    ColumnMeta* column = new ColumnMeta(name, kind, len, defaultConstraint, notNull);
    this->columns.push_back(column);
    return true;
}

bool TableMeta::addColumn(ColumnMeta* column){
    // check if name no duplicate
    for(int i = 0; i < this->columns.size(); i++){
        if(this->columns[i]->name == column->name){
            cout<<"column name duplicate"<<endl;
            return false;
        }
    }

    this->columns.push_back(column);
    return true;
}

bool TableMeta::addForeignKey(ForeignKeyConstraint* foreignKey){
    // check if name no duplicate
    for(int i = 0; i < this->foreignKeys.size(); i++){
        if(this->foreignKeys[i]->name == foreignKey->name){
            cout<<"foreign key name duplicate"<<endl;
            return false;
        } else if(*this->foreignKeys[i] == *foreignKey){
            cout<<"foreign key already exists"<<endl;
            return false;
        }
    }

    // check columns exist
    for(int i=0; i < foreignKey->source_columns.size(); i++){
        if(foreignKey->source_columns[i] >= this->columns.size()){
            cout<<"column " <<foreignKey->source_columns[i]<<" not exists in this table"<<endl;
            return false;
        }
    }
    // no duplicate columns
    for(int i=0; i < foreignKey->source_columns.size(); i++){
        for(int j=i+1; j < foreignKey->source_columns.size(); j++){
            if(foreignKey->source_columns[i] == foreignKey->source_columns[j]){
                cout<<"duplicate columns in foreign key"<<endl;
                return false;
            }
        }
    }

    // to check the keys in the reference table, needs to be done outside

    // add new foreign key
    this->foreignKeys.push_back(foreignKey);

    return true;
}

bool TableMeta::addPrimaryKey(PrimaryKeyConstraint* primaryKey){
    // check if exists
    if(this->primaryKey != nullptr){
        cout<<"primary key already exists"<<endl;
        return false;
    }

    // check columns exist and no duplicate
    for(int i=0; i < primaryKey->getColumns().size(); i++){
        if(primaryKey->getColumns()[i] >= this->columns.size()){
            cout<<"column " <<primaryKey->getColumns()[i]<<" not exists in this table"<<endl;
            return false;
        }
    }

    for(int i=0; i < primaryKey->getColumns().size(); i++){
        for(int j=i+1; j < primaryKey->getColumns().size(); j++){
            if(primaryKey->getColumns()[i] == primaryKey->getColumns()[j]){
                cout<<"duplicate columns in primary key"<<endl;
                return false;
            }
        }
    }

    // add new primary key
    this->primaryKey = primaryKey;

    return true;
}

Record TableMeta::getExampleRecord(){
    Record record;
    for(int i = 0; i < this->columns.size(); i++){
        unsigned int kind = this->columns[i]->kind;
        unsigned int len = this->columns[i]->len;
        if(kind == 0){
            record.append(true, int(0));
        } else if(kind == 1){
            record.append(true, float(0));
        } else if(kind == 2){
            record.append(true, string(len, ' ').c_str(), len);
        }
    }
    return record;
}

void TableMeta::print(){
    cout<<"TableMeta: "<<this->name<<endl;
    cout<<"columns: "<<endl;
    for(int i = 0; i < this->columns.size(); i++){
        this->columns[i]->print();
    }
    cout<<"foreign keys: "<<endl;
    for(int i = 0; i < this->foreignKeys.size(); i++){
        this->foreignKeys[i]->print();
        cout<<endl;
    }
    if(this->primaryKey != nullptr){
        this->primaryKey->print();
    }
}

