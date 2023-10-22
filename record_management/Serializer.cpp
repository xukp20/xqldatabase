#include <cstring>
#include "Serializer.h"
#include "Record.h"

unsigned int Serializer::serialize_record(Record* record, char* bytes) {
    // cout<<"Serialize"<<endl;
    // bitmap of columns
    int count = record->bitmap.size();
    // cout<<"count: "<<count<<endl;
    for(int i = 0; i < count; i++){
        if(i % 8 == 0){
            bytes[i/8] = 0b00000000;
        }
        bytes[i/8] = bytes[i/8] | (record->bitmap[i] << (7-i%8));
    }

    // data
    unsigned int offset = (count + 7) / 8;
    for (int idx = 0; idx < count; idx++){
        // handle different kind of data
        switch (record->items[idx]->kind)     // int, float or string
        {
            case 0:     // int
                memcpy(bytes + offset, &((Int_Item*)record->items[idx])->integer, sizeof(int));
                offset += sizeof(int);
                break;
            
            case 1:     // float
                memcpy(bytes + offset, &((Float_Item*)record->items[idx])->floating, sizeof(float));
                offset += sizeof(float);
                break;

            case 2:
                memcpy(bytes + offset, ((Char_Item*)record->items[idx])->str, ((Char_Item*)record->items[idx])->len);
                offset += ((Char_Item*)record->items[idx])->len;
                break;

            default:
                break;
        }
    }

    return offset;      // return the total count of bytes for checking
}


void Serializer::deserialize_record(char* bytes, Record* target){
    // target needs to have the right count of columns and right content
    // this method only fills in the blanks in the target
    // cout<<"dis"<<endl;
    unsigned int count = target->bitmap.size();
    // bitmap of columns
    target->bitmap.clear();     // empty the bitmap
    for(int i = 0; i < count; i++){
        target->bitmap.push_back(
            ((bytes[i/8] >> (7-i%8)) & 0b00000001) ? true: false
        );
    }

    // for (int i=0;i<count;i++)
    //     cout<<target->bitmap[i]<<" ";

    unsigned int offset = (count + 7) / 8;
    for (int idx = 0; idx < count; idx++) {
        switch (target->items[idx]->kind)       // set by the caller
        {
            case 0:
                memcpy(&((Int_Item*)target->items[idx])->integer, bytes + offset, sizeof(int));
                offset += sizeof(int);
                break;

            case 1:
                memcpy(&((Float_Item*)target->items[idx])->floating, bytes + offset, sizeof(float));
                offset += sizeof(float);
                break;
            
            case 2:
            {
                memcpy(((Char_Item*)target->items[idx])->str, bytes + offset, ((Char_Item*)target->items[idx])->len);
                offset += ((Char_Item*)target->items[idx])->len;
                break;
            }   

            default:
                break;
        }
    }
}



// serializer for table meta
#define  MAX_TABLE_NAME_LEN 128
#define  MAX_COLUMN_NAME_LEN 48
#define  MAX_FOREIGN_KEY_NAME_LEN 48
#define  MAX_COLUMN_NUM 30
#define  MAX_FOREIGN_KEY_NUM 8
#define  MAX_COLUMN_LEN 48
#define  MAX_FOREIGN_KEY_COLUMN_NUM 8
#define  MAX_PRIMARY_KEY_COLUMN_NUM 8

#define COLUMN_NUM_WIDTH 1
#define FOREIGN_KEY_NUM_WIDTH 1
#define HAS_PRIMARY_KEY_WIDTH 1
#define COLUMN_KIND_WIDTH 1
#define COLUMN_LEN_WIDTH 1
#define COLUMN_NOTNULL_WIDTH 1
#define COLUMN_HAVE_DEFAULT_WIDTH 1
#define TARGET_TABLE_WIDTH 1
#define FOREIGN_KEY_COLUMN_NUM_WIDTH 1
#define PRIMARY_KEY_COLUMN_NUM_WIDTH 1
#define FOREIGN_KEY_COLUMN_INDEXES_WIDTH 8
#define PRIMARY_KEY_COLUMN_INDEXES_WIDTH 8

#define LINE 128
#define PAGE_SIZE 128 * 64


bool Serializer::serialize_table_meta(TableMeta* table_meta, char* bytes){
    // cout<<"Serialize"<<endl;
    // table name
    memset(bytes, 0, MAX_TABLE_NAME_LEN);
    if (table_meta->name.length() > MAX_TABLE_NAME_LEN){
        cout<<"Table name too long"<<endl;
        return false;
    }
    memcpy(bytes, table_meta->name.c_str(), table_meta->name.length());
    unsigned int offset = MAX_TABLE_NAME_LEN;

    // column num
    unsigned int column_num = table_meta->columns.size();
    if (column_num > MAX_COLUMN_NUM){
        cout<<"Too many columns"<<endl;
        return false;
    } 
    if (column_num != table_meta->column_num){
        cout<<"Column num not match"<<endl;
        return false;
    }
    
    unsigned char byte[1];
    byte[0] = column_num;
    memcpy(bytes + offset, byte, COLUMN_NUM_WIDTH);
    offset += COLUMN_NUM_WIDTH;

    // foreign key num
    unsigned int foreign_key_num = table_meta->foreignKeys.size();
    if (foreign_key_num > MAX_FOREIGN_KEY_NUM){
        cout<<"Too many foreign keys"<<endl;
        return false;
    }
    if (foreign_key_num != table_meta->foreign_key_num){
        cout<<"Foreign key num not match"<<endl;
        return false;
    }
    byte[0] = foreign_key_num;
    memcpy(bytes + offset, byte, FOREIGN_KEY_NUM_WIDTH);
    offset += FOREIGN_KEY_NUM_WIDTH;

    // has primary key
    byte[0] = table_meta->primaryKey? 1: 0;
    memcpy(bytes + offset, byte, HAS_PRIMARY_KEY_WIDTH);
    
    offset = 2 * LINE;      // empty place left

    // save columns
    for (int i = 0; i < table_meta->columns.size(); i++){
        // column name
        memset(bytes + offset, 0, MAX_COLUMN_NAME_LEN);
        if (table_meta->columns[i]->name.length() > MAX_COLUMN_NAME_LEN){
            cout<<"Column name too long"<<endl;
            return false;
        }
        memcpy(bytes + offset, table_meta->columns[i]->name.c_str(), table_meta->columns[i]->name.length());
        offset += MAX_COLUMN_NAME_LEN;

        ColumnMeta* column = table_meta->columns[i];
        // column default 
        memset(bytes + offset, 0, MAX_COLUMN_LEN);
        if(column->defaultValue){   // have default value
            if(column->defaultValue->kind != column->kind){
                cout<<"Default value type not match"<<endl;
                return false;
            }
            if(column->kind == 2){      // char
                DefaultCharConstraint* defaultStr = (DefaultCharConstraint*)column->defaultValue;
                if(defaultStr->len > MAX_COLUMN_LEN){
                    cout<<"Default value too long"<<endl;
                    return false;
                }
                memcpy(bytes + offset, defaultStr->str, defaultStr->len);
            } else if(column->kind == 0){   // int
                DefaultIntConstraint* defaultInt = (DefaultIntConstraint*)column->defaultValue;
                memcpy(bytes + offset, &defaultInt->integer, sizeof(int));
            } else if(column->kind == 1){   // float
                DefaultFloatConstraint* defaultFloat = (DefaultFloatConstraint*)column->defaultValue;
                memcpy(bytes + offset, &defaultFloat->floating, sizeof(float));
            }
        }
        offset += MAX_COLUMN_LEN;

        // column kind
        byte[0] = column->kind;
        memcpy(bytes + offset, byte, COLUMN_KIND_WIDTH);
        offset += COLUMN_KIND_WIDTH;

        // column len
        byte[0] = column->len;
        memcpy(bytes + offset, byte, COLUMN_LEN_WIDTH);
        offset += COLUMN_LEN_WIDTH;

        // column not null
        byte[0] = column->notNull? 1: 0;
        memcpy(bytes + offset, byte, COLUMN_NOTNULL_WIDTH);
        offset += COLUMN_NOTNULL_WIDTH;

        // column have default
        byte[0] = column->defaultValue? 1: 0;
        memcpy(bytes + offset, byte, COLUMN_HAVE_DEFAULT_WIDTH);
        offset += COLUMN_HAVE_DEFAULT_WIDTH;

        offset = (i + 3) * LINE;
    }

    offset = (1 + 1 + MAX_COLUMN_NUM) * LINE;

    // foreign keys
    for (int i = 0; i < table_meta->foreignKeys.size(); i++){
        ForeignKeyConstraint* foreignKey = table_meta->foreignKeys[i];
        // foreign key name
        memset(bytes + offset, 0, MAX_FOREIGN_KEY_NAME_LEN);
        if (foreignKey->target_columns.size() > MAX_FOREIGN_KEY_COLUMN_NUM){
            cout<<"Too many columns in foreign key"<<endl;
            return false;
        }
        if (foreignKey->target_columns.size() != foreignKey->source_columns.size()){
            cout<<"Foreign key column number not match"<<endl;
            return false;
        }

        if (foreignKey->name.length() > MAX_FOREIGN_KEY_NAME_LEN){
            cout<<"Foreign key target table name too long"<<endl;
            return false;
        }

        memcpy(bytes + offset, foreignKey->name.c_str(), foreignKey->name.length());
        offset += MAX_FOREIGN_KEY_NAME_LEN;

        // target_table index
        byte[0] = foreignKey->target_table;
        memcpy(bytes + offset, byte, TARGET_TABLE_WIDTH);
        offset += TARGET_TABLE_WIDTH;

        // column num
        byte[0] = foreignKey->target_columns.size();
        memcpy(bytes + offset, byte, FOREIGN_KEY_COLUMN_NUM_WIDTH);
        offset += FOREIGN_KEY_COLUMN_NUM_WIDTH;

        // source columns
        for (int j = 0; j < foreignKey->source_columns.size(); j++){
            byte[0] = foreignKey->source_columns[j];
            memcpy(bytes + offset + j, byte, 1);
        }   
        offset += FOREIGN_KEY_COLUMN_INDEXES_WIDTH;

        // target columns
        for (int j = 0; j < foreignKey->target_columns.size(); j++){
            byte[0] = foreignKey->target_columns[j];
            memcpy(bytes + offset + j, byte, 1);
        }
        offset += FOREIGN_KEY_COLUMN_INDEXES_WIDTH; 

        offset = (2 + MAX_COLUMN_NUM) * LINE + (i+1) * LINE;  
    }   
    offset = (2 + MAX_COLUMN_NUM + MAX_FOREIGN_KEY_NUM) * LINE;

    // primary key
    memset(bytes + offset, 0, PRIMARY_KEY_COLUMN_NUM_WIDTH);
    memset(bytes + offset + PRIMARY_KEY_COLUMN_NUM_WIDTH, 0, MAX_PRIMARY_KEY_COLUMN_NUM);
    if(table_meta->primaryKey){     // has primary key
        if(table_meta->primaryKey->columns.size() > MAX_PRIMARY_KEY_COLUMN_NUM){
            cout<<"Too many columns in primary key"<<endl;
            return false;
        }
        // column num
        byte[0] = table_meta->primaryKey->columns.size();
        memcpy(bytes + offset, byte, PRIMARY_KEY_COLUMN_NUM_WIDTH);
        offset += PRIMARY_KEY_COLUMN_NUM_WIDTH;

        for (int i = 0; i < table_meta->primaryKey->columns.size(); i++){
            byte[0] = table_meta->primaryKey->columns[i];
            memcpy(bytes + offset + i, byte, 1);
        }
    }

    offset += LINE;

    if(offset > PAGE_SIZE){
        cout<<"Table meta too large"<<endl;
        return false;
    }
    return true;
}


void Serializer::deserialize_table_meta(char* bytes, TableMeta* table_meta){
    // cout<<"deserialize table meta"<<endl;
    unsigned int offset = 0;
    unsigned char byte[1];

    // table name
    table_meta->name = bytes + offset;
    offset += MAX_TABLE_NAME_LEN;

    // column num
    memcpy(byte, bytes + offset, COLUMN_NUM_WIDTH);
    table_meta->column_num = byte[0];
    offset += COLUMN_NUM_WIDTH;

    // foreign key num
    memcpy(byte, bytes + offset, FOREIGN_KEY_NUM_WIDTH);
    table_meta->foreign_key_num = byte[0];
    offset += FOREIGN_KEY_NUM_WIDTH;

    // have primary key or not
    memcpy(byte, bytes + offset, HAS_PRIMARY_KEY_WIDTH);
    bool has_primary = byte[0] ? true: false;
    offset += HAS_PRIMARY_KEY_WIDTH;

    offset = 2 * LINE;

    // columns
    for (int i = 0; i < table_meta->column_num; i++){
        ColumnMeta* column = new ColumnMeta();
        // column name
        column->name = bytes + offset;
        offset += MAX_COLUMN_NAME_LEN;

        // kind 
        memcpy(byte, bytes + offset + MAX_COLUMN_NAME_LEN, COLUMN_KIND_WIDTH);
        column->kind = byte[0];

        // len
        memcpy(byte, bytes + offset + MAX_COLUMN_NAME_LEN + COLUMN_KIND_WIDTH, COLUMN_LEN_WIDTH);
        column->len = byte[0];

        // not null
        memcpy(byte, bytes + offset + MAX_COLUMN_NAME_LEN + COLUMN_KIND_WIDTH + COLUMN_LEN_WIDTH, COLUMN_NOTNULL_WIDTH);
        column->notNull = byte[0] ? true: false;

        // have default
        memcpy(byte, bytes + offset + MAX_COLUMN_NAME_LEN + COLUMN_KIND_WIDTH + COLUMN_LEN_WIDTH + COLUMN_NOTNULL_WIDTH, COLUMN_HAVE_DEFAULT_WIDTH);
        bool haveDefault = byte[0] ? true: false;

        // default value
        int int_default = 0;
        float float_default = 0;
        if(haveDefault){
            switch(column->kind){
                case 0:
                    memcpy(&int_default, bytes + offset, 4);
                    column->defaultValue = new DefaultIntConstraint(int_default);
                    break;
                case 1:
                    memcpy(&float_default, bytes + offset, 4);
                    column->defaultValue = new DefaultFloatConstraint(float_default);
                    break;
                case 2:
                    column->defaultValue = new DefaultCharConstraint(bytes + offset, column->len);
                    break;
            }
        } else {
            column->defaultValue = nullptr;
        }

        table_meta->columns.push_back(column);
        offset = (2 + i + 1) * LINE;
    }
    offset = (2 + MAX_COLUMN_NUM) * LINE;

    // foreign keys
    for(int i = 0; i < table_meta->foreign_key_num; i++){
        // name
        string name = bytes + offset;
        offset += MAX_FOREIGN_KEY_NAME_LEN;
        
        // target table
        memcpy(byte, bytes + offset, TARGET_TABLE_WIDTH);
        unsigned int target_table = byte[0];
        offset += TARGET_TABLE_WIDTH;

        // column num
        memcpy(byte, bytes + offset, FOREIGN_KEY_COLUMN_NUM_WIDTH);
        unsigned int column_num = byte[0];
        offset += FOREIGN_KEY_COLUMN_NUM_WIDTH;

        // target columns
        vector<unsigned int> target_columns;
        for (int j = 0; j < column_num; j++){
            memcpy(byte, bytes + offset + j, 1);
            target_columns.push_back(byte[0]);
        }
        offset += FOREIGN_KEY_COLUMN_INDEXES_WIDTH;
        
        // source columns
        vector<unsigned int> source_columns;
        for (int j = 0; j < column_num; j++){
            memcpy(byte, bytes + offset + j, 1);
            source_columns.push_back(byte[0]);
        }
        offset += FOREIGN_KEY_COLUMN_INDEXES_WIDTH;

        ForeignKeyConstraint* foreign_key = new ForeignKeyConstraint(name, target_table, source_columns, target_columns);
        table_meta->foreignKeys.push_back(foreign_key);

        offset = (2 + MAX_COLUMN_NUM + i + 1) * LINE;
    }
    offset = (2 + MAX_COLUMN_NUM + MAX_FOREIGN_KEY_NUM) * LINE;

    // primary key
    if(has_primary){
        // column num
        memcpy(byte, bytes + offset, PRIMARY_KEY_COLUMN_NUM_WIDTH);
        unsigned int column_num = byte[0];
        offset += PRIMARY_KEY_COLUMN_NUM_WIDTH;

        // columns
        vector<unsigned int> columns;
        for (int i = 0; i < column_num; i++){
            memcpy(byte, bytes + offset + i, 1);
            columns.push_back(byte[0]);
        }
        offset += PRIMARY_KEY_COLUMN_INDEXES_WIDTH;

        PrimaryKeyConstraint* primary_key = new PrimaryKeyConstraint(columns);
        table_meta->primaryKey = primary_key;
    } else {
        table_meta->primaryKey = nullptr;
    }
    return;
}

// table index
#define TABLE_NUM_WIDTH 4
#define TABLE_NAME_WIDTH 128
#define PAGE_ID_WIDTH 4

bool Serializer::serialize_table_indexes(TableIndex* table_indexes, char* bytes){
    /*
        page 0 of meta file of a database is for table indexes
        from pageId to table name
        | table num 4|
        | table 1 pageId 4| table 1 name 128|
        | table 2 pageId 4| table 2 name 128|
        ...    
    */
    unsigned int offset = 0;
    unsigned int table_num = table_indexes->table_num;
    memcpy(bytes + offset, &table_num, TABLE_NUM_WIDTH);
    offset += TABLE_NUM_WIDTH;

    for(auto it = table_indexes->indexToTable.begin(); it != table_indexes->indexToTable.end(); it++){
        unsigned int pageId = it->first;
        string name = it->second;
        // check name len
        if(name.length() > MAX_TABLE_NAME_LEN){
            cout<<"Error: fail to serialize table indexes, table name is too long"<<endl;
            return false;
        }
        memcpy(bytes + offset, &pageId, PAGE_ID_WIDTH);
        offset += PAGE_ID_WIDTH;
        memcpy(bytes + offset, name.c_str(), name.length());
        offset += TABLE_NAME_WIDTH;
    }
    
    if(offset >= PAGE_SIZE){
        cout<<"table indexes is too long"<<endl;
    }

    return true;
}

void Serializer::deserialize_table_indexes(char* bytes, TableIndex* table_indexes){
    unsigned int offset = 0;
    unsigned int table_num = 0;
    memcpy(&table_num, bytes + offset, TABLE_NUM_WIDTH);
    offset += TABLE_NUM_WIDTH;
    table_indexes->init();

    for(int i = 0; i < table_num; i++){
        unsigned int pageId = 0;
        memcpy(&pageId, bytes + offset, PAGE_ID_WIDTH);
        offset += PAGE_ID_WIDTH;
        string name = bytes + offset;
        offset += TABLE_NAME_WIDTH;
        table_indexes->append(pageId, name);
    }
    return;
}

// serialize db indexes
#define DB_NUM_WIDTH 4
#define DB_ID_WIDTH 4

bool Serializer::serialize_db_indexes(DBIndex* db_indexes, char* bytes){
    /*
        page 0 of meta file of database system is for db indexes
        from dbId to db name
        | db num 4|
        | db 1 dbId 4| db 1 name 128|
        | db 2 dbId 4| db 2 name 128|
        ...    
    */
    unsigned int offset = 0;
    unsigned int db_num = db_indexes->db_num;
    memcpy(bytes + offset, &db_num, DB_NUM_WIDTH);
    offset += DB_NUM_WIDTH;

    for(auto it = db_indexes->indexToDB.begin(); it != db_indexes->indexToDB.end(); it++){
        unsigned int dbId = it->first;
        string name = it->second;
        // check name len
        if(name.length() > DB_NAME_WIDTH){
            cout<<"Error: fail to serialize db indexes, db name is too long"<<endl;
            return false;
        }
        memcpy(bytes + offset, &dbId, DB_ID_WIDTH);
        offset += DB_ID_WIDTH;
        memcpy(bytes + offset, name.c_str(), name.length());
        offset += DB_NAME_WIDTH;
    }
    
    if(offset >= PAGE_SIZE){
        cout<<"db indexes is too long"<<endl;
    }

    return true;
}

void Serializer::deserialize_db_indexes(char* bytes, DBIndex* db_indexes){
    unsigned int offset = 0;
    unsigned int db_num = 0;
    memcpy(&db_num, bytes + offset, DB_NUM_WIDTH);
    offset += DB_NUM_WIDTH;
    db_indexes->init();

    for(int i = 0; i < db_num; i++){
        unsigned int dbId = 0;
        memcpy(&dbId, bytes + offset, DB_ID_WIDTH);
        offset += DB_ID_WIDTH;
        string name = bytes + offset;
        offset += DB_NAME_WIDTH;
        db_indexes->append(dbId, name);
    }
    return;
}