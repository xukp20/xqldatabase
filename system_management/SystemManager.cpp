#include "SystemManager.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
using namespace std::filesystem;

#define PAGE_SIZE 8192      // bytes
// don't needs to use file manager because meta data is not large-scaled

SystemManager::SystemManager(string base_path) {
    this->base_path = base_path;
    this->current_db = "";
}

SystemManager::~SystemManager() {
}

// tools functions for reading and writing a page
bool SystemManager::readPage(filesystem::path file_path, char* page, unsigned int page_id) {
    fstream file(file_path, ios::binary|ios::in);
    file.seekg(page_id * PAGE_SIZE);
    file.read(page, PAGE_SIZE);
    file.close();
    return !file.fail();
}

bool SystemManager::writePage(char* page, filesystem::path file_path, unsigned int page_id) {
    fstream file(file_path, ios::binary|ios::out|ios::in);
    file.seekp(page_id * PAGE_SIZE, ios::beg);
    file.write(page, PAGE_SIZE);
    file.close();
    return !file.fail();
}


// tools for getting meta
void printPage(char* page){
    // print a page's bytes as hex
    for(int i = 0; i < PAGE_SIZE; i++){
        printf("%02x ", (unsigned char)page[i]);
        if(i % 16 == 15){
            printf("\n");
        }
    }
}

void printPageHead(char* page){
    // print a page's first 8 * 128bytes as hex
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 128; j++){
            printf("%02x ", (unsigned char)page[i * 128 + j]);
        }
        printf("\n");
    }
}


bool SystemManager::getDBIndex(DBIndex* db_index) {
    char page[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    path file_path = path(this->base_path) / "global" / "$db_meta";
    if(!exists(file_path)){
        cout<<"db meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    if (!readPage(file_path, page, 0)) {
        cout << "read db_meta failed" << endl;
        return false;
    }
    Serializer::deserialize_db_indexes(page, db_index);
    return true;
}

bool SystemManager::setDBIndex(DBIndex* db_index) {
    char page[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    path file_path = path(this->base_path) / "global" / "$db_meta";
    if(!exists(file_path)){
        cout<<"db index file not found, please check if the system has been modified"<<endl;
        return false;
    }
    Serializer::serialize_db_indexes(db_index, page);
    if (!writePage(page, file_path, 0)) {
        cout << "write db_index failed" << endl;
        return false;
    }
    return true;
}

bool SystemManager::getTableIndex(TableIndex* table_index) {
    char page[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    if(this->current_db == ""){
        cout<<"no database selected"<<endl;
        return false;
    }
    path file_path = path(this->base_path) / this->current_db / "$table_meta";
    // cout<<"get index from table meta file path: "<<file_path<<endl;
    if(!exists(file_path)){
        cout<<"table meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    if (!readPage(file_path, page, 0)) {
        cout << "read table_meta failed" << endl;
        return false;
    }
    // cout<<"read page"<<endl;
    // printPageHead(page);
    Serializer::deserialize_table_indexes(page, table_index);
    return true;
}

bool SystemManager::setTableIndex(TableIndex* table_index) {
    char page[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    if(this->current_db == ""){
        cout<<"no database selected"<<endl;
        return false;
    }
    path file_path = path(this->base_path) / this->current_db / "$table_meta";
    // cout<<"set index to table meta file path: "<<file_path<<endl;
    if(!exists(file_path)){
        cout<<"table meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    // cout<<"set table index:"<<endl;
    // table_index->print();
    bool result = Serializer::serialize_table_indexes(table_index, page);
    if(!result){
        cout<<"serialize table index failed"<<endl;
        return false;
    }
    if (!writePage(page, file_path, 0)) {
        cout << "write table_meta failed" << endl;
        return false;
    }
    // cout<<"after write page"<<endl;
    // printPageHead(page);
    return true;
}

bool SystemManager::getIndexMap(IndexMap* index_map){
    char page[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    if(this->current_db == ""){
        cout<<"no database selected"<<endl;
        return false;
    }
    path file_path = path(this->base_path) / this->current_db / "$index_meta";
    if(!exists(file_path)){
        cout<<"index meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    if (!readPage(file_path, page, 0)) {
        cout << "read index_meta failed" << endl;
        return false;
    }
    return index_map->deserialize(page); 
}

bool SystemManager::setIndexMap(IndexMap* index_map){
    char page[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    if(this->current_db == ""){
        cout<<"no database selected"<<endl;
        return false;
    }
    path file_path = path(this->base_path) / this->current_db / "$index_meta";
    if(!exists(file_path)){
        cout<<"index meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    index_map->serialize(page);
    if (!writePage(page, file_path, 0)) {
        cout << "write index_meta failed" << endl;
        return false;
    }
    return true;
}

bool SystemManager::getTableMeta(TableMeta* table_meta, string table_name) {
    char page[PAGE_SIZE];
    if(this->current_db == ""){
        cout<<"no database selected"<<endl;
        return false;
    }
    path file_path = path(this->base_path) / this->current_db / "$table_meta";
    if(!exists(file_path)){
        cout<<"table meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    TableIndex table_index;
    if (!getTableIndex(&table_index)) {
        cout << "get table meta failed" << endl;
        return false;
    }
    unsigned int page_id = table_index.getIndex(table_name);
    if(page_id == 0){
        cout<<"table not found"<<endl;
        return false;
    }
    if (!readPage(file_path, page, page_id)) {
        cout << "read table_meta failed" << endl;
        return false;
    }
    Serializer::deserialize_table_meta(page, table_meta);
    return true;
}

bool SystemManager::setTableMeta(TableMeta* table_meta, string table_name) {
    char page[PAGE_SIZE];
    if(this->current_db == ""){
        cout<<"no database selected"<<endl;
        return false;
    }
    path file_path = path(this->base_path) / this->current_db / "$table_meta";
    if(!exists(file_path)){
        cout<<"table meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    TableIndex table_index;
    if (!getTableIndex(&table_index)) {
        cout << "get table meta failed" << endl;
        return false;
    }
    unsigned int page_id = table_index.getIndex(table_name);
    if(page_id == 0){
        cout<<"table not found"<<endl;
        return false;
    }
    // cout<<"set table meta: "<<page_id<<endl;
    bool result = Serializer::serialize_table_meta(table_meta, page);
    if(!result){
        cout<<"serialize table meta failed"<<endl;
        return false;
    }
    if (!writePage(page, file_path, page_id)) {
        cout << "write table_meta failed" << endl;
        return false;
    }
    return true;
}

bool SystemManager::clearTableMeta(string table_name) {
    char page[PAGE_SIZE];
    if(this->current_db == ""){
        cout<<"no database selected"<<endl;
        return false;
    }
    path file_path = path(this->base_path) / this->current_db / "$table_meta";
    if(!exists(file_path)){
        cout<<"table meta file not found, please check if the system has been modified"<<endl;
        return false;
    }
    TableIndex table_index;
    if (!getTableIndex(&table_index)) {
        cout << "get table index failed" << endl;
        return false;
    }
    unsigned int page_id = table_index.getIndex(table_name);
    if(page_id == 0){
        cout<<"table not found"<<endl;
        return false;
    }
    memset(page, 0, PAGE_SIZE);
    if (!writePage(page, file_path, page_id)) {
        cout << "write table_meta failed" << endl;
        return false;
    }
    return true;
}


bool SystemManager::initSys() {
    // create the base path if not exists
    path db_base_path(this->base_path);
    if(is_regular_file(db_base_path)){
        cout<<"can't init system because the target path is a file"<<endl;
        return false;
    }
    if (!exists(db_base_path)) {
        create_directory(this->base_path);
        cout<<"creating system root directory: "<<this->base_path<<endl;
    } else {
        cout<<"the base path already exists, continue initializing the databases"<<endl;
    }

    // create global dir
    path global_dir(db_base_path / "global");
    if (!exists(global_dir)) {
        create_directory(global_dir);
        cout<<"creating global dir"<<endl;
    } else {
        cout<<"Error: the global dir already exists, initializing stopped"<<endl;
        return false;
    }

    // create the global db index file
    path global_index_file(global_dir / "$db_meta");
    if (!exists(global_index_file)) {
        cout<<"creating meta file for databases"<<endl;
        ofstream global_index(global_index_file);   // create the file
        global_index.close();
        // write the db index to the first page
        // only zero db now
        char page[PAGE_SIZE];
        memset(page, 0, PAGE_SIZE);
        DBIndex* db_index = new DBIndex();     // empty db index
        Serializer::serialize_db_indexes(db_index, page);
        delete db_index;
        bool result = writePage(page, global_index_file, 0);
        if(!result){
            cout<<"Error: failed to create db meta file for the system"<<endl;
            return false;
        }
    } else {
        cout<<"Error: the global meta file already exists, initializing stopped"<<endl;
    }

    cout<<"system initialized successfully"<<endl;
    return true;
}

bool SystemManager::initPath(){
    cout<<"change to system base path: "<<this->base_path<<endl;
    filesystem::current_path(this->base_path);
    return true;
}

bool SystemManager::createDatabase(string db_name){
    // if current db is not empty, move to the base path
    if(this->current_db != ""){
        cout<<"stop using the current database"<<endl;
        this->current_db = "";
        filesystem::current_path(this->base_path);
    }

    // check if the db name is valid
    if(db_name.length() > DB_NAME_WIDTH){
        cout<<"Error: the database name is too long"<<endl;
        return false;
    }
    path db_base_path(this->base_path);
    path db_dir(db_base_path / db_name);
    
    // check if the db already exists
    DBIndex index;
    if(!getDBIndex(&index)){
        cout<<"Error: failed to get the db index"<<endl;
        return false;
    }
    if(index.hasDB(db_name)){
        cout<<"Error: the database already exists"<<endl;
        return false;
    }

    // create the db dir
    create_directory(db_dir);
    cout<<"creating database directory: "<<db_dir<<endl;
    // create the index file for tables
    path table_index_path = db_dir / "$table_meta";
    ofstream table_index_file(table_index_path);
    table_index_file.close();
    // create the index file for indexes
    path index_index_path = db_dir / "$index_meta";
    ofstream index_index_file(index_index_path);
    index_index_file.close();

    // write the table index to the first page
    // only zero table now
    char page[PAGE_SIZE];
    memset(page, 0, PAGE_SIZE);
    TableIndex* table_index = new TableIndex();     // empty table index
    Serializer::serialize_table_indexes(table_index, page);
    bool result = writePage(page, table_index_path, 0);
    delete table_index;
    if(!result){
        cout<<"Error: failed to create table meta file for the database"<<endl;
        return false;
    }

    // write the index index to the first page
    // only zero index now
    memset(page, 0, PAGE_SIZE);
    IndexMap* index_index = new IndexMap();     // empty index index
    index_index->index_map.clear();
    index_index->serialize(page);
    result = writePage(page, index_index_path, 0);
    delete index_index;
    if(!result){
        cout<<"Error: failed to create index meta file for the database"<<endl;
        return false;
    }

    // add the db to the global db index
    DBIndex* db_index = new DBIndex();
    if(!getDBIndex(db_index)){
        cout<<"Error: failed to get the global db index"<<endl;
        return false;
    }
    db_index->append(db_index->nextIndex(), db_name);
    if(!setDBIndex(db_index)){
        cout<<"Error: failed to set the global db index"<<endl;
        return false;
    }
    delete db_index;

    cout<<"create database done";
    return true;
}

bool SystemManager::dropDatabase(string db_name){
    // check if the db exists
    path db_dir = path(this->base_path) / db_name;
    DBIndex db_index;
    if(!getDBIndex(&db_index)){
        cout<<"Error: failed to get the db index"<<endl;
        return false;
    }
    if(!db_index.hasDB(db_name)){
        cout<<"Error: the database does not exist"<<endl;
        return false;
    }

    // check before deleting
    cout<<"Are you sure to delete the database? (y/others)"<<endl;
    string check;
    cin>>check;
    if(check != "y"){
        cout<<"delete canceled"<<endl;
        return false;
    }

    // set current db
    if(this->current_db == db_name){
        this->current_db = "";
        filesystem::current_path(this->base_path);    // move back to the base path
    }
    // delete the db dir
    remove_all(db_dir);

    //delete the db from the global index
    if(!getDBIndex(&db_index)){
        cout<<"Error: failed to get the db index"<<endl;
        return false;
    }
    bool result = db_index.deleteIndex(db_index.getIndex(db_name));
    if(!result){
        cout<<"Error: failed to delete the db index"<<endl;
        return false;
    }

    result = setDBIndex(&db_index);
    if(!result){
        cout<<"Error: failed to set the db index"<<endl;
        return false;
    }

    cout<<"delete database done";
    return true;
}

bool SystemManager::useDatabase(string db_name){
    // check if the db exists
    path db_dir(path(this->base_path) / db_name);
    DBIndex db_index;
    if(!getDBIndex(&db_index)){
        cout<<"Error: failed to get the db index"<<endl;
        return false;
    }
    if(!db_index.hasDB(db_name)){
        cout<<"Error: the database doesn't exist"<<endl;
        cout<<"Do you want to create the database? (y/others)"<<endl;
        string check;
        cin>>check;
        if(check == "y"){
            bool result = createDatabase(db_name);
            if(result){
                current_db = db_name;
            }
            return result;
        } else {
            return false;
        }
    }

    // check if the db is already in use
    if(this->current_db == db_name){
        cout<<"the database is already in use"<<endl;
    } else if (this->current_db == ""){
        cout<<"using database: "<<db_name<<endl;
        this->current_db = db_name;
        filesystem::current_path(db_dir);
    } else {
        cout<<"switch from database "<<this->current_db<<" to "<<db_name<<endl;
        this->current_db = db_name;
        filesystem::current_path(db_dir);
    }

    cout<<"use database done";
    return true;
}

bool SystemManager::showDatabases(){
    DBIndex db_index;
    if(!getDBIndex(&db_index)){
        cout<<"Error: failed to get the db index"<<endl;
        return false;
    }
    cout<<endl<<"+"<<setw(7)<<setfill('-')<<""<<"+"<<setw(63)<<setfill('-')<<""<<"+"<<endl;
    cout<<"| DB id | DB name"<<setfill(' ')<<setw(55)<<""<<"|"<<endl;
    cout<<endl<<"+"<<setw(7)<<setfill('-')<<""<<"+"<<setw(63)<<setfill('-')<<""<<"+"<<endl;
    for(auto it = db_index.indexToDB.begin(); it != db_index.indexToDB.end(); it++){
        cout<<"|"<<right<<setw(5)<<setfill(' ')<<(unsigned int)it->first<<"  |  "<<setw(61)<<setfill(' ')<<left<<it->second<<"|"<<endl;
    }
    cout<<endl<<"+"<<setw(7)<<setfill('-')<<""<<"+"<<setw(63)<<setfill('-')<<""<<"+"<<endl;
    cout<<"Total DB num: "<<(unsigned int)db_index.db_num<<endl;

    cout<<"show databases done";
    
    return true;
}

bool SystemManager::showTables(){
    // check current db
    if(this->current_db == ""){
        cout<<"Error: no database is in use"<<endl;
        return false;
    }
    TableIndex table_index;
    if(!getTableIndex(&table_index)){
        cout<<"Error: failed to get the table index"<<endl;
        return false;
    }
    // cout<<"get table index:"<<endl;
    // table_index.print();
    cout<<endl<<setw(73)<<setfill('-')<<""<<endl;
    cout<<"| Table id | Table name"<<setfill(' ')<<setw(49)<<""<<"|"<<endl;
    cout<<setw(73)<<setfill('-')<<""<<endl;
    for(auto it = table_index.indexToTable.begin(); it != table_index.indexToTable.end(); it++){
        cout<<"|"<<right<<setw(8)<<setfill(' ')<<(unsigned int)it->first<<"  |  "<<setw(58)<<setfill(' ')<<left<<it->second<<"|"<<endl;
    }
    cout<<setw(73)<<setfill('-')<<""<<endl;
    cout<<"Total table num: "<<(unsigned int)table_index.table_num<<endl;

    cout<<"show tables done";
    return true;
}

// create table
bool SystemManager::createRawTable(TableMeta* table_meta, RecordManager* record_manager){
    // check current db
    if(this->current_db == ""){
        cout<<"Error: no database is in use"<<endl;
        return false;
    }
    // check if the table exists
    path table_file(path(this->base_path) / this->current_db / table_meta->name);
    if(exists(table_file)){
        cout<<"Error: the table already exists"<<endl;
        return false;
    }

    // use record manager to create the table file
    // assemble an example record to get the record size
    Record record = table_meta->getExampleRecord();
    // create file
    bool result = record_manager->createTableFile(table_meta->name.c_str(), &record);
    if(!result){
        cout<<"Error: failed to create the table file"<<endl;
        return false;
    }

    // add the table to the global index
    TableIndex table_index;
    if(!getTableIndex(&table_index)){
        cout<<"Error: failed to get the table index"<<endl;
        return false;
    }
    result = table_index.append(table_index.nextIndex(), table_meta->name);
    if(!result){
        cout<<"Error: failed to append the table index"<<endl;
        return false;
    }
    result = setTableIndex(&table_index);
    if(!result){
        cout<<"Error: failed to set the table index"<<endl;
        return false;
    }

    // write the table meta
    result = setTableMeta(table_meta, table_meta->name);
    if(!result){
        cout<<"Error: failed to write the table meta"<<endl;
        return false;
    }

    return true;
}


bool SystemManager::createTable(TableMeta* table_meta, vector<ForeignKeyField*>& foreign_keys, RecordManager* record_manager){
    // check current db
    cout<<"create table"<<endl;
    if(this->current_db == ""){
        cout<<"Error: no database is in use"<<endl;
        return false;
    }
    // check if the table exists
    path table_file(path(this->base_path) / this->current_db / table_meta->name);
    TableIndex exist_table_index;
    if(!getTableIndex(&exist_table_index)){
        cout<<"Error: failed to get the table index"<<endl;
        return false;
    }
    if(exist_table_index.hasTable(table_meta->name)){
        cout<<"Error: the table already exists"<<endl;
        return false;
    }

    // check primary key less or equal than 2 fields
    if(table_meta->primaryKey != nullptr){
        if(table_meta->primaryKey->columns.size() > 2){
            cout<<"Error: primary key can only be less or equal than 2 fields"<<endl;
            return false;
        }
    }

    // check primary key columns not conflict
    if(table_meta->primaryKey != nullptr){
        for(int i = 0; i < table_meta->primaryKey->columns.size(); i++){
            for(int j = i + 1; j < table_meta->primaryKey->columns.size(); j++){
                if(table_meta->primaryKey->columns[i] == table_meta->primaryKey->columns[j]){
                    cout<<"Error: primary key columns conflict"<<endl;
                    return false;
                }
            }
        }
    }

    // check foreign key
    vector<string> foreign_key_names;
    // get not anonymous foreign key names
    for(int i = 0; i < foreign_keys.size(); i++){
        ForeignKeyField* foreign_key = foreign_keys[i];
        if(foreign_key->fk_name != ""){
            // check if the foreign key name is unique
            for(int j = 0; j < foreign_key_names.size(); j++){
                if(foreign_key_names[j] == foreign_key->fk_name){
                    cout<<"Error: the foreign key name is not unique"<<endl;
                    return false;
                }
            }
            foreign_key_names.push_back(foreign_key->fk_name);
        }
    }

    for(int i = 0; i < foreign_keys.size(); i++){
        ForeignKeyField* foreign_key = foreign_keys[i];
        
        // generate anonymous foreign key name
        if(foreign_key->fk_name == ""){
            foreign_key->fk_name = "fk_" + to_string(i);
            bool found = true;
            while(found){
                found = false;
                for(int j = 0; j < foreign_key_names.size(); j++){
                    if(foreign_key_names[j] == foreign_key->fk_name){
                        found = true;
                        break;
                    }
                }
                if(found){
                    foreign_key->fk_name = foreign_key->fk_name + "_" + to_string(i);
                }
            }
        }

        // check if the foreign key table exists
        string target_table_name = foreign_key->target_table;
        TableMeta* target_table_meta = new TableMeta();
        bool result = getTableMeta(target_table_meta, target_table_name);
        if(!result){
            cout<<"Error: the foreign key table doesn't exist"<<endl;
            delete target_table_meta;
            return false;
        }
        // get the index of the target table
        TableIndex* table_index = new TableIndex();
        result = getTableIndex(table_index);
        if(!result){
            cout<<"Error: failed to get the table index"<<endl;
            delete target_table_meta;
            delete table_index;
            return false;
        }
        unsigned int target_table_id = table_index->getIndex(target_table_name);

        // check if the foreign key target field exists
        vector<string> target_field_names = foreign_key->target_columns->identifiers;
        vector<unsigned int> target_field_ids;
        for(int j = 0; j < target_field_names.size(); j++){
            string target_field_name = target_field_names[j];
            bool found = false;
            for(int k = 0; k < target_table_meta->columns.size(); k++){
                if(target_table_meta->columns[k]->name == target_field_name){
                    found = true;
                    target_field_ids.push_back(k);
                    break;
                }
            }
            if(!found){
                cout<<"Error: the foreign key target field doesn't exist"<<endl;
                delete target_table_meta;
                delete table_index;
                return false;
            }
        }

        // check if the foreign key source field exists
        vector<string> source_field_names = foreign_key->source_columns->identifiers;
        vector<unsigned int> source_field_ids;
        for(int j = 0; j < source_field_names.size(); j++){
            string source_field_name = source_field_names[j];
            bool found = false;
            for(int k = 0; k < table_meta->columns.size(); k++){
                if(table_meta->columns[k]->name == source_field_name){
                    found = true;
                    source_field_ids.push_back(k);
                    break;
                }
            }
            if(!found){
                cout<<"Error: the foreign key source field doesn't exist"<<endl;
                delete target_table_meta;
                delete table_index;
                return false;
            }
        }

        // check if the foreign key field is primary key of the target table
        if(target_table_meta->primaryKey != nullptr){
            if(target_table_meta->primaryKey->columns.size() != target_field_ids.size()){
                cout<<"Error: the foreign key field is not primary key of the target table"<<endl;
                delete target_table_meta;
                delete table_index;
                return false;
            }
            // find the primary key field in the target table
            for(int j = 0; j < target_field_ids.size(); j++){
                unsigned int target_field_id = target_field_ids[j];
                bool found = false;
                for(int k = 0; k < target_table_meta->primaryKey->columns.size(); k++){
                    if(target_table_meta->primaryKey->columns[k] == target_field_id){
                        found = true;
                        break;
                    }
                }
                if(!found){
                    cout<<"Error: the foreign key field is not primary key of the target table"<<endl;
                    delete target_table_meta;
                    delete table_index;
                    return false;
                }
            }
        } else {
            cout<<"Error: the foreign key field is not primary key of the target table"<<endl;
            delete target_table_meta;
            delete table_index;
            return false;
        }

        // check type same
        for(int j = 0; j < source_field_ids.size(); j++){
            unsigned int source_field_id = source_field_ids[j];
            unsigned int target_field_id = target_field_ids[j];
            if(table_meta->columns[source_field_id]->kind != target_table_meta->columns[target_field_id]->kind){
                cout<<"Error: the foreign key field type is not same as the target table"<<endl;
                delete target_table_meta;
                delete table_index;
                return false;
            }
        }

        // assemble the foreign key and add it to the table meta
        ForeignKeyConstraint* foreign_key_constraint = new ForeignKeyConstraint(
            foreign_key->fk_name,
            target_table_id,
            target_field_ids,
            source_field_ids
        );
        result = table_meta->addForeignKey(foreign_key_constraint);
        if(!result){
            delete target_table_meta;
            delete table_index;
            return false;
        }
        delete target_table_meta;
        delete table_index;
    }

    // finish adding the foreign key constraints
    table_meta->foreign_key_num = table_meta->foreignKeys.size();

    // create the table file
    bool result = createRawTable(table_meta, record_manager);
    if(!result){
        return false;
    }

    // if has primary key, create the index file
    if(table_meta->primaryKey != nullptr){
        vector<unsigned int> primary_key_ids = table_meta->primaryKey->columns;
        string index_name = table_meta->name + "_pri";
        result = createIndex(table_meta->name, index_name, primary_key_ids, PRIMARY_INDEX);
        if(!result){
            //remove the table file
            dropTable(table_meta->name);
            return false;
        }
    }

    cout<<"create table done";
    return true;
}

bool SystemManager::dropTable(string table_name){
    if(this->current_db == ""){
        cout<<"Error: no database is in use"<<endl;
        return false;
    }
    // check if the table exists
    path table_file(path(this->base_path) / this->current_db / table_name);
    TableIndex existing_table_index;
    if(!getTableIndex(&existing_table_index)){
        cout<<"Error: failed to get the table index"<<endl;
        return false;
    }
    if(!existing_table_index.hasTable(table_name)){
        cout<<"Error: the table doesn't exist"<<endl;
        return false;
    }

    // check if the table is referenced by other tables
    TableMeta* table_meta = new TableMeta();
    bool result = getTableMeta(table_meta, table_name);
    if(!result){
        cout<<"Error: failed to get the table meta"<<endl;
        delete table_meta;
        return false;
    }
    if(table_meta->primaryKey != nullptr){
        // check if the table is referenced by other tables
        TableIndex* table_index = new TableIndex();
        if(!getTableIndex(table_index)){
            cout<<"Error: failed to get the table index"<<endl;
            delete table_meta;
            return false;
        }
        // id of this table
        unsigned int table_id = table_index->getIndex(table_name);
        // cout<<"this table id is "<<table_id<<endl;
        for(auto iter = table_index->indexToTable.begin(); iter != table_index->indexToTable.end(); iter++){
            string table_name = iter->second;
            // cout<<"check table "<<table_name<<endl;
            TableMeta* other_meta = new TableMeta();
            bool result = getTableMeta(other_meta, table_name);
            if(!result){
                cout<<"Error: failed to get the table meta"<<endl;
                delete table_meta;
                delete other_meta;
                return false;
            }
            // cout<<"foreign key num is "<<other_meta->foreign_key_num<<endl;
            for(int j = 0; j < other_meta->foreign_key_num; j++){
                // cout<<"check foreign key "<<j<<endl;
                ForeignKeyConstraint* foreign_key = other_meta->foreignKeys[j];
                // cout<<"target table id is "<<foreign_key->target_table<<endl;
                if(foreign_key->target_table == table_id){
                    cout<<"Error: the table is referenced by other tables"<<endl;
                    delete table_meta;
                    delete other_meta;
                    return false;
                }
            }
        }
        delete table_index;
    }
    delete table_meta;

    // check before deleting
    // cout<<"Are you sure to delete the table? (y/others)"<<endl;
    // string check;
    // cin>>check;
    // if(check != "y"){
    //     cout<<"delete canceled"<<endl;
    //     return false;
    // } else 
    {
        // delete the table from the table index
        TableIndex table_index;
        if(!getTableIndex(&table_index)){
            cout<<"Error: failed to get the table index"<<endl;
            return false;
        }
        unsigned int table_id = table_index.getIndex(table_name);
        bool result = table_index.deleteIndex(table_id);
        if(!result){
            cout<<"Error: failed to delete the table index"<<endl;
            return false;
        }

        result = setTableIndex(&table_index);
        if(!result){
            cout<<"Error: failed to set the table index"<<endl;
            return false;
        }
        
        // delete the table file
        remove(table_file);

        // delete the related indexes
        // get the index map
        IndexMap index_map;
        if(!getIndexMap(&index_map)){
            cout<<"Error: failed to get the index map"<<endl;
            return false;
        }
        // delete the related indexes
        // cout<<"delete table's indexes "<<endl;
        // cout<<index_map.index_map.size()<<endl;
        vector<string> to_delete = index_map.deleteIndexes(table_id);
        // cout<<index_map.index_map.size()<<endl;
        for(int i = 0; i < to_delete.size(); i++){
            // cout<<"delete index "<<to_delete[i]<<endl;
            string index_file_name = to_delete[i] + "_idx";
            path index_file_path(path(this->base_path) / this->current_db / index_file_name);
            remove(index_file_path);
        }
        
        // set the index map
        result = setIndexMap(&index_map);
        if(!result){
            cout<<"Error: failed to set the index map"<<endl;
            return false;
        }
    }

    cout<<"drop table done";
    return true;
}

bool SystemManager::descTable(string table_name){
    if(this->current_db == ""){
        cout<<"Error: no database is in use"<<endl;
        return false;
    }
    // check if the table exists
    path table_file(path(this->base_path) / this->current_db / table_name);
    TableIndex existing_table_index;
    if(!getTableIndex(&existing_table_index)){
        cout<<"Error: failed to get the table index"<<endl;
        return false;
    }
    if(!existing_table_index.hasTable(table_name)){
        cout<<"Error: the table doesn't exist"<<endl;
        return false;
    }

    // read the table meta from table_index file
    TableMeta* table_meta = new TableMeta;
    // cout<<"before getTableMeta"<<endl;
    if(!getTableMeta(table_meta, table_name)){
        cout<<"Error: failed to get the table meta"<<endl;
        return false;
    }
    // cout<<"after getTableMeta"<<endl;

    // print the table meta
    unsigned int max_name_len = 13; // title length
    unsigned int max_default_len = 15;
    for(int i = 0; i < table_meta->columns.size(); i++){
        if(table_meta->columns[i]->name.length() > max_name_len){
            max_name_len = table_meta->columns[i]->name.length();
        }
        if(table_meta->columns[i]->defaultValue != nullptr){
            unsigned int len = 0;
            DefaultConstraint *default_value = table_meta->columns[i]->defaultValue;
            switch (default_value->kind)
            {
                case 0:
                    len = to_string(((DefaultIntConstraint*)default_value)->integer).length() + 2;
                    break;
                case 1:
                    len = to_string(((DefaultFloatConstraint*)default_value)->floating).length() + 2;
                    break;
                case 2:
                    len = ((DefaultCharConstraint*)default_value)->len + 2;
                    break;
            }
            if(len > max_default_len){
                max_default_len = len;
            }
        }
    }

    // cout<<"max default len: "<<max_default_len<<endl;
    unsigned int type_width = 15;
    unsigned int null_width = 6;
    unsigned int total_width = max_name_len + max_default_len + type_width + null_width + 5;
    if (table_meta->name.length() + 3 > total_width)
        total_width = table_meta->name.length() + 3;
    cout<<'+'<<setw(total_width - 2)<<setfill('-')<<""<<'+'<<endl;
    cout<<"| "<<left<<setfill(' ')<<setw(total_width-3)<<table_meta->name<<"|"<<endl;
    cout<<'+'<<setw(max_name_len)<<setfill('-')<<""<<'+'<<setw(15)<<setfill('-')<<""<<'+'<<setw(6)<<setfill('-')<<""<<'+'<<setw(max_default_len)<<setfill('-')<<""<<'+'<<endl;
    cout<<"| Field       "<<setfill(' ')<<setw(max_name_len-13)<<""<<"| Type          | Null | Default"<<setw(max_default_len-8)<<setfill(' ')<<""<<"|"<<endl;
    cout<<'+'<<setw(max_name_len)<<setfill('-')<<""<<'+'<<setw(15)<<setfill('-')<<""<<'+'<<setw(6)<<setfill('-')<<""<<'+'<<setw(max_default_len)<<setfill('-')<<""<<'+'<<endl;

    for(int i = 0; i < table_meta->columns.size(); i++){
        cout<<"|"<<table_meta->columns[i]->name<<setfill(' ')<<setw(max_name_len-table_meta->columns[i]->name.length())<<""<<"|";
        switch (table_meta->columns[i]->kind)
        {
            case 0:
                cout<<" INT           ";
                break;
            case 1:
                cout<<" FLOAT         ";
                break;
            case 2:
                cout<<" VARCHAR("<<table_meta->columns[i]->len<<")"<<setfill(' ')<<setw(5-to_string(table_meta->columns[i]->len).length())<<"";
                break;
        }
        cout<<"|";

        // null
        if(table_meta->columns[i]->notNull){
            cout<<" NO   ";
        } else {
            cout<<" YES  ";
        }
        cout<<"|";

        if(table_meta->columns[i]->defaultValue != nullptr){
            DefaultConstraint *default_value = table_meta->columns[i]->defaultValue;
            switch (default_value->kind)
            {
                case 0:
                    cout<<" "<<to_string(((DefaultIntConstraint*)default_value)->integer)
                    <<setfill(' ')<<setw(max_default_len - 2 - to_string(((DefaultIntConstraint*)default_value)->integer).length())<<"";
                    break;
                case 1:
                    cout<<" "<<to_string(((DefaultFloatConstraint*)default_value)->floating)
                    <<setfill(' ')<<setw(max_default_len - 2 -to_string(((DefaultFloatConstraint*)default_value)->floating).length())<<"";
                    break;
                case 2:
                    char* str = new char[((DefaultCharConstraint*)default_value)->len + 1];
                    memcpy(str, ((DefaultCharConstraint*)default_value)->str, ((DefaultCharConstraint*)default_value)->len);
                    str[((DefaultCharConstraint*)default_value)->len] = '\0';
                    cout<<" "<<setfill(' ')<<left<<setw(((DefaultCharConstraint*)default_value)->len)<<str
                    <<setfill(' ')<<setw(max_default_len - 2 -((DefaultCharConstraint*)default_value)->len)<<"";
                    delete[] str;
                    // cout<<"fixlen :"<<max_default_len-((DefaultCharConstraint*)default_value)->len<<endl;
                    break;
            }
        } else {
            cout<<" NULL"<<setfill(' ')<<setw(max_default_len - 6)<<"";
        }
        cout<<" |"<<endl;

        if(i == table_meta->columns.size() - 1)
            cout<<'+'<<setw(max_name_len)<<setfill('-')<<""<<'+'<<setw(15)<<setfill('-')<<""<<'+'<<setw(6)<<setfill('-')<<""<<'+'<<setw(max_default_len)<<setfill('-')<<""<<'+'<<endl;
    }

    // print primary key if exists
    if(table_meta->primaryKey != nullptr){
        cout<<"PRIMARY KEY (";
        for(int i = 0; i < table_meta->primaryKey->columns.size(); i++){
            unsigned int column_idx = table_meta->primaryKey->columns[i];
            cout<<table_meta->columns[column_idx]->name;
            if(i != table_meta->primaryKey->columns.size() - 1)
                cout<<", ";
        }
        cout<<");"<<endl;
    }

    // print foreign key if exists
    for(int i=0; i<table_meta->foreignKeys.size(); i++){
        ForeignKeyConstraint *foreign_key = table_meta->foreignKeys[i];
        cout<<"FOREIGN KEY "<<foreign_key->name<<"(";
        for(int j=0; j<foreign_key->source_columns.size(); j++){
            unsigned int column_idx = foreign_key->source_columns[j];
            cout<<table_meta->columns[column_idx]->name;
            if(j != foreign_key->source_columns.size() - 1)
                cout<<", ";
        }
        cout<<") REFERENCES ";
        // get target table name
        string target_table_name = existing_table_index.indexToTable[foreign_key->target_table];
        TableMeta *target_table_meta = new TableMeta();
        if(!getTableMeta(target_table_meta, target_table_name)){
            cout<<"Error: cannot get table meta"<<endl;
            return false;
        }
        cout<<target_table_name<<"(";
        for(int j=0; j<foreign_key->target_columns.size(); j++){
            unsigned int column_idx = foreign_key->target_columns[j];
            cout<<target_table_meta->columns[column_idx]->name;
            if(j != foreign_key->target_columns.size() - 1)
                cout<<", ";
        }
        delete target_table_meta;
        cout<<");"<<endl;
    }

    // print all the indexes 
    IndexMap *index_map = new IndexMap();
    if(!getIndexMap(index_map)){
        cout<<"Error: cannot get index map"<<endl;
        return false;
    }
    unsigned int table_id = existing_table_index.getIndex(table_name);
    vector<IndexMeta> indexes = index_map->getIndexMetas(table_id);
    for(int i=0; i<indexes.size(); i++){
        IndexMeta index_meta = indexes[i];
        cout<<"INDEX (";
        for(int j=0; j<index_meta.column_idx.size(); j++){
            unsigned int column_idx = index_meta.column_idx[j];
            cout<<table_meta->columns[column_idx]->name;
            if(j != index_meta.column_idx.size() - 1)
                cout<<", ";
        }
        cout<<");"<<endl;
    }
    // cout<<"before delete"<<endl;
    delete table_meta;
    // cout<<"after delete"<<endl;
    cout<<"desc table done";
    return true;
}


// indexes

// create index, called by the executor from outside
bool SystemManager::createIndex(AlterStatement* statement, RecordManager* record_manager, FileManager* index_fm, BufPageManager* index_bpm){
    if(this->current_db == ""){
        cout<<"Error: no database selected"<<endl;
        return false;
    }

    // get a index name and the column ids
    string table_name = statement->table_name;
    vector<string> column_names = statement->column_names;
    string index_name = table_name + "_";
    for(int i=0; i<column_names.size(); i++){
        index_name += column_names[i];
        if(i != column_names.size() - 1)
            index_name += "_";
    }

    // get the column ids
    TableMeta* table_meta = new TableMeta();    
    if(!getTableMeta(table_meta, table_name)){
        cout<<"Error: cannot get table meta"<<endl;
        return false;
    }
    vector<unsigned int> column_ids;
    for(int i=0; i<column_names.size(); i++){
        int column_id = -1;
        for(int j=0; j<table_meta->columns.size(); j++){
            if(table_meta->columns[j]->name == column_names[i]){
                column_id = j;
                break;
            }
        }
        if(column_id == -1){
            cout<<"Error: column "<<column_names[i]<<" does not exist"<<endl;
            delete table_meta;
            return false;
        } else {
            column_ids.push_back((unsigned int)column_id);
        }
    }

    // try to create index file
    // will check index already exists
    bool result = createIndex(table_name, index_name, column_ids, 0);   // 0 for normal kind index
    if(!result){
        delete table_meta;
        return false;
    }

    // insert all the records from the table into the index
    // use record manager
    vector<Record*> records;
    int fileID = record_manager->openTableFile(table_name.c_str());
    vector<Condition*> conds;
    Record example = table_meta->getExampleRecord();
    records = record_manager->selectRecords(fileID, conds, &example);
    record_manager->closeTableFile(fileID);
    
    // open the index tree
    Tree* tree = new Tree(index_name + "_idx", index_fm, index_bpm);
    tree->initTree();

    // insert all the indexes of the records
    for(int i=0; i<records.size(); i++){
        // assemble the index
        vector<int> key_vals;
        for(int j=0; j<column_ids.size(); j++){
            key_vals.push_back(((Int_Item*)records[i]->items[column_ids[j]])->integer);
        }
        Key key(key_vals);
        Index index(key, records[i]->getRID());
        result = tree->insertIndex(index);

        // check if insert successfully
        if(!result){
            cout<<"Error: cannot insert index into index tree"<<endl;
            delete table_meta;
            // free the records
            record_manager->freeRecordList(records);
            // close the index tree
            delete tree;
            return false;
        }
    }

    // free the records
    record_manager->freeRecordList(records);
    // close the index tree
    delete tree;
    // free the table meta
    delete table_meta;

    cout<<"create index done";
    return true;
}

// create index file, called by the system manager
bool SystemManager::createIndex(string table_name, string index_name, vector<unsigned int> columns, unsigned int index_type){
    // check if table exists
#ifdef DEBUG
    cout<<"create index "<<index_name<<" on "<<table_name<<endl;
#endif
    TableIndex table_index;
    if(!getTableIndex(&table_index)){
        cout<<"Error: cannot get table index"<<endl;
        return false;
    }
    unsigned int table_idx = table_index.getIndex(table_name);
    if(table_idx == 0){
        cout<<"Error: table "<<table_name<<" does not exist"<<endl;
        return false;
    }

    // check if index exists
    IndexMap index_map;
    if(!getIndexMap(&index_map)){
        cout<<"Error: cannot get index map"<<endl;
        return false;
    }

    // first check if a index with the same columns exists
    int same_idx = index_map.getSameColumns(columns, table_idx);
    if(same_idx != -1){
        // if the type is different, update the index type and Info
        int origin_type = index_map.index_map[same_idx].index_type;
        if(origin_type != index_type && index_type >= 1){
            if(origin_type == 0){
                index_map.index_map[same_idx].index_type = index_type;
                index_map.index_map[same_idx].index_name = index_name;
                cout<<"Index with the same columns exists, reuse the index"<<endl;
                return setIndexMap(&index_map);
            } else if (origin_type + index_type == 3){
                index_map.index_map[same_idx].index_type = 3;   
                cout<<"Index with the same columns exists, reuse the index"<<endl;
                return setIndexMap(&index_map);
            }
        } else {
            cout<<"Error: index with the same columns exists"<<endl;
            return false;
        }
    }

    if(index_map.hasIndexMeta(index_name)){
        cout<<"Error: index name exists"<<endl;
        return false;
    }

    IndexMeta index_meta;
    index_meta.index_name = index_name;
    index_meta.table_idx = table_idx;
    index_meta.index_type = index_type;

    // check columns 
    if(columns.size() == 0 || columns.size() > 2){
        cout<<"Error: invalid column number"<<endl;
        return false;
    }

    // check if columns exist
    TableMeta* table_meta = new TableMeta;
    if(!getTableMeta(table_meta, table_name)){
        delete table_meta;
        cout<<"Error: cannot get table meta"<<endl;
        return false;
    }
    for(int i=0; i<columns.size(); i++){
        if(columns[i] >= table_meta->columns.size()){
            cout<<"Error: column "<<columns[i]<<" does not exist"<<endl;
            delete table_meta;
            return false;
        }
        ColumnMeta *column_meta = table_meta->columns[columns[i]];
        if(!column_meta->kind == 0){
            cout<<"Error: column "<<columns[i]<<" is not int"<<endl;
            delete table_meta;
            return false;
        }
        index_meta.column_idx.push_back(columns[i]);
    }
    delete table_meta;

    index_map.addIndex(index_meta);

    // create the index file
    string index_file_name = index_name + "_idx";
    FileManager* file_manager = new FileManager();
    BufPageManager* buf_page_manager = new BufPageManager(file_manager);
    if(!Tree::createTree(index_file_name, file_manager, buf_page_manager)){
        cout<<"Error: cannot create index file"<<endl;
        delete file_manager;
        delete buf_page_manager;
        return false;
    }
    delete file_manager;
    delete buf_page_manager;

    if(!setIndexMap(&index_map)){
        cout<<"Error: cannot set index map"<<endl;
        return false;
    }
    return true;
}

// called by the executor from outside
bool SystemManager::dropIndex(AlterStatement* statement){
    if(this->current_db == ""){
        cout<<"Error: no database selected"<<endl;
        return false;
    }
    // get the index name by searching for the columns in the index map
    // get table_id and column_ids
    string table_name = statement->table_name;
    vector<string> column_names = statement->column_names;
    vector<unsigned int> column_ids;
    unsigned int table_id;

    TableIndex table_index;
    if(!getTableIndex(&table_index)){
        cout<<"Error: cannot get table index"<<endl;
        return false;
    }
    table_id = table_index.getIndex(table_name);

    TableMeta* table_meta = new TableMeta;
    if(!getTableMeta(table_meta, table_name)){
        delete table_meta;
        cout<<"Error: cannot get table meta"<<endl;
        return false;
    }

    // get the column ids and existance
    for(int i=0; i<column_names.size(); i++){
        int column_id = -1;
        for(int j=0; j<table_meta->columns.size(); j++){
            if(table_meta->columns[j]->name == column_names[i]){
                column_id = j;
                break;
            }
        }
        if(column_id == -1){
            cout<<"Error: column "<<column_names[i]<<" does not exist"<<endl;
            delete table_meta;
            return false;
        } else {
            column_ids.push_back((unsigned int)column_id);
        }
    }
    delete table_meta;

    // get the index name from the index map
    IndexMap index_map;
    if(!getIndexMap(&index_map)){
        cout<<"Error: cannot get index map"<<endl;
        return false;
    }
    int same_idx = index_map.getSameColumns(column_ids, table_id);
    if(same_idx == -1){
        cout<<"Error: index does not exist"<<endl;
        return false;
    }  else {
        string index_name = index_map.index_map[same_idx].index_name;
        return dropIndex(index_name, 0);
    }
}

bool SystemManager::dropIndex(string index_name, unsigned int kind){
    IndexMap index_map;
    if(!getIndexMap(&index_map)){
        cout<<"Error: cannot get index map"<<endl;
        return false;
    }
    if(!index_map.hasIndexMeta(index_name)){
        cout<<"Error: index "<<index_name<<" does not exist"<<endl;
        return false;
    }
    if(this->current_db == ""){
        cout<<"Error: no database selected"<<endl;
        return false;
    }
    IndexMeta index_meta = index_map.getIndexMeta(index_name);
    if(kind < index_meta.index_type){
        cout<<"Error: cannot drop index "<<index_name<<endl;
        cout<<"There is other constraint on the index"<<endl;
        return false;
    }

    // cout<<"Are you sure to drop index "<<index_name<<"? (y/others)"<<endl;
    // string input;
    // cin>>input;
    // if(input != "y"){
    //     cout<<"Drop index "<<index_name<<" canceled"<<endl;
    //     return false;
    // }
    index_map.deleteIndex(index_name);

    // delete the index file
    string index_file_name = index_name + "_idx";
    path index_file_path(path(this->base_path) / this->current_db / index_file_name);
    remove(index_file_path);

    if(!setIndexMap(&index_map)){
        cout<<"Error: cannot set index map"<<endl;
        return false;
    }
    return true;
}

// add primary key
bool SystemManager::addPrimaryKey(AlterStatement* statement, RecordManager* record_manager, FileManager* index_fm, BufPageManager* index_bpm){
    // get the column names
    vector<string> column_names = statement->column_names;

    // get the table name
    string table_name = statement->table_name;

    // get the table meta
    TableMeta* table_meta = new TableMeta;
    if(!getTableMeta(table_meta, table_name)){
        delete table_meta;
        cout<<"Error: cannot get table meta"<<endl;
        return false;
    }

    // check if primary key exists
    if(table_meta->primaryKey != nullptr){
        cout<<"Error: primary key already exists"<<endl;
        delete table_meta;
        return false;
    }

    // check if the columns exist
    vector<unsigned int> column_ids;
    for(int i=0; i<column_names.size(); i++){
        int column_id = -1;
        for(int j=0; j<table_meta->columns.size(); j++){
            if(table_meta->columns[j]->name == column_names[i]){
                column_id = j;
                break;
            }
        }
        if(column_id == -1){
            cout<<"Error: column "<<column_names[i]<<" does not exist"<<endl;
            delete table_meta;
            return false;
        } else {
            column_ids.push_back((unsigned int)column_id);
        }
    }

    // check if the columns are unique
    for(int i=0; i<column_ids.size(); i++){
        for(int j=i+1; j<column_ids.size(); j++){
            if(column_ids[i] == column_ids[j]){
                cout<<"Error: column "<<table_meta->columns[column_ids[i]]->name<<" is not unique"<<endl;
                delete table_meta;
                return false;
            }
        }
    }

    // check columns dimensions and kind
    if(column_ids.size() == 0 || column_ids.size() > 2){
        cout<<"Error: primary key must be 1 or 2 columns"<<endl;
        delete table_meta;
        return false;
    }
    for(int i=0; i<column_ids.size(); i++){
        if(table_meta->columns[column_ids[i]]->kind != 0){
            cout<<"Error: primary key must be integer or (int, int)"<<endl;
            delete table_meta;
            return false;
        }
    }

    // assemble the primary key
    PrimaryKeyConstraint* primary_key = new PrimaryKeyConstraint(column_ids);

    // set the primary key
    table_meta->primaryKey = primary_key;

    // create the index file for the primary key
    string index_name = table_name + "_pri";
    bool result = createIndex(table_name, index_name, column_ids, 1);   // primary type is 1
    if(!result){
        cout<<"Error: cannot create index for primary key"<<endl;
        delete table_meta;
        return false;
    }

    // insert all the records into the index
    vector<Record*> records;
    int fileID = record_manager->openTableFile(table_name.c_str());
    vector<Condition*> conds;
    Record example = table_meta->getExampleRecord();
    records = record_manager->selectRecords(fileID, conds, &example);
    record_manager->closeTableFile(fileID);
    
    // open the index tree
    Tree* tree = new Tree(index_name + "_idx", index_fm, index_bpm);
    tree->initTree();

    // insert all the indexes of the records
    for(int i=0; i<records.size(); i++){
        // assemble the index
        vector<int> key_vals;
        for(int j=0; j<column_ids.size(); j++){
            key_vals.push_back(((Int_Item*)records[i]->items[column_ids[j]])->integer);
        }
        Key key(key_vals);
        Index index(key, records[i]->getRID());

        // search for the index before insert, if exists, return false
        Index search_index = tree->searchIndex(key);
        if(search_index.key == key && (search_index.rid.pageIdx != 0)){
            cout<<"Error: primary key value conflicts"<<endl;
            delete table_meta;
            // free the records
            record_manager->freeRecordList(records);
            // close the index tree
            delete tree;

            // needs to delete the index file
            dropIndex(index_name, 1);   // primary type is 1
            return false;
        }

        result = tree->insertIndex(index);

        // check if insert successfully
        if(!result){
            cout<<"Error: cannot insert index into index tree"<<endl;
            delete table_meta;
            // free the records
            record_manager->freeRecordList(records);
            // close the index tree
            delete tree;
            return false;
        }
    } 

    // update the table meta
    result = setTableMeta(table_meta, table_name);
    if(!result){
        cout<<"Error: cannot set table meta"<<endl;
        delete table_meta;
        // free the records
        record_manager->freeRecordList(records);
        // close the index tree
        delete tree;
        return false;
    }


    // free the records
    record_manager->freeRecordList(records);
    // close the index tree
    delete tree;
    delete table_meta;

    cout<<"add primary done";
    return true;
}

// drop primary key
bool SystemManager::dropPrimaryKey(AlterStatement* statement){
    // get the table name
    string table_name = statement->table_name;

    // get the table meta
    TableMeta* table_meta = new TableMeta;
    if(!getTableMeta(table_meta, table_name)){
        delete table_meta;
        cout<<"Error: cannot get table meta"<<endl;
        return false;
    }

    // check if primary key exists
    if(table_meta->primaryKey == nullptr){
        cout<<"Error: primary key does not exist"<<endl;
        delete table_meta;
        return false;
    }

    // check if referenced by foreign key
    TableIndex* table_index = new TableIndex;
    if(!getTableIndex(table_index)){
        cout<<"Error: cannot get table index"<<endl;
        delete table_meta;
        delete table_index;
        return false;
    }
    unsigned int table_id = table_index->getIndex(table_name);
    // go through each table's foreign key
    for(map<unsigned int, string>::iterator it = table_index->indexToTable.begin(); it != table_index->indexToTable.end(); it++){
        // skip the table itself
        if(it->first == table_id){
            continue;
        }
        // get the table meta
        TableMeta* foreign_table_meta = new TableMeta;
        if(!getTableMeta(foreign_table_meta, it->second)){
            cout<<"Error: cannot get table meta"<<endl;
            delete table_meta;
            delete table_index;
            delete foreign_table_meta;
            return false;
        }
        // go through each foreign key
        for(int i=0; i<foreign_table_meta->foreignKeys.size(); i++){
            ForeignKeyConstraint* foreign_key = foreign_table_meta->foreignKeys[i];
            // check if the foreign key references the table
            if(foreign_key->target_table == table_id){
                cout<<"Error: cannot drop primary key, referenced by foreign key"<<endl;
                delete table_meta;
                delete table_index;
                delete foreign_table_meta;
                return false;
            }
        }
        delete foreign_table_meta;
    }

    // delete the index file
    string index_name = table_name + "_pri";
    bool result = dropIndex(index_name, 1);  // primary type is 1
    if(!result){
        cout<<"Error: cannot drop index for primary key"<<endl;
        delete table_meta;
        delete table_index;
        return false;
    }

    // delete the primary key
    delete table_meta->primaryKey;
    table_meta->primaryKey = nullptr;
    if(!setTableMeta(table_meta, table_name)){
        cout<<"Error: cannot set new table meta"<<endl;
        delete table_meta;
        delete table_index;
        return false;
    }

    delete table_meta;
    delete table_index;

    cout<<"drop primary done";
    return true;
}

bool SystemManager::addForeignKey(AlterForeignKeyStatement* statement, RecordManager* record_manager, FileManager* index_fm, BufPageManager* index_bpm)
{
    // get the table name
    string table_name = statement->table_name;
    vector<string> source_columns = statement->column_names;
    string target_table_name = statement->target_table_name;
    vector<string> target_columns = statement->target_column_names;
    string fk_name = statement->fk_name;

    // check if the tables exists
    TableIndex* table_index = new TableIndex;
    if(!getTableIndex(table_index)){
        cout<<"Error: cannot get table index"<<endl;
        delete table_index;
        return false;
    }
    unsigned int table_id = table_index->getIndex(table_name);
    unsigned int target_table_id = table_index->getIndex(target_table_name);

    // check if the tables exists
    if(table_id == 0){
        cout<<"Error: table "<<table_name<<" does not exist"<<endl;
        delete table_index;
        return false;
    }
    if(target_table_id == 0){
        cout<<"Error: table "<<target_table_name<<" does not exist"<<endl;
        delete table_index;
        return false;
    }

    // check if the foreign key name exists
    TableMeta* table_meta = new TableMeta;
    if(!getTableMeta(table_meta, table_name)){
        cout<<"Error: cannot get table meta"<<endl;
        delete table_index;
        delete table_meta;
        return false;
    }

    if(fk_name == ""){
        // get a name for the foreign key
        fk_name = "fk_" + to_string(table_meta->foreignKeys.size());
        bool found = true;
        while(found){
            found = false;
            for(int j = 0; j < table_meta->foreignKeys.size(); j++){
                if(fk_name == table_meta->foreignKeys[j]->name){
                    found = true;
                    break;
                }
            }
            if(found){
                fk_name = fk_name + "_" + to_string(table_meta->foreignKeys.size());
            }
        }
    } else {
        // check if name conflicts
        for(int j = 0; j < table_meta->foreignKeys.size(); j++){
            if(fk_name == table_meta->foreignKeys[j]->name){
                cout<<"Error: foreign key name conflicts"<<endl;
                delete table_index;
                delete table_meta;
                return false;
            }
        }
    }

    // check if the source columns exists
    // also get the ids;
    vector<unsigned int> source_column_ids;
    for(int i = 0; i < source_columns.size(); i++){
        bool found = false;
        for(int j = 0; j < table_meta->columns.size(); j++){
            if(source_columns[i] == table_meta->columns[j]->name){
                // check the type is int
                if(table_meta->columns[j]->kind != 0){
                    cout<<"Error: foreign key source column type must be int"<<endl;
                    delete table_index;
                    delete table_meta;
                    return false;
                }
                found = true;
                source_column_ids.push_back(j);
                break;
            }
        }
        if(!found){
            cout<<"Error: column "<<source_columns[i]<<" does not exist"<<endl;
            delete table_index;
            delete table_meta;
            return false;
        }
    }

    // check if the target columns exists
    // also get the column ids if exists
    vector<unsigned int> target_column_ids;
    TableMeta* target_table_meta = new TableMeta;
    if(!getTableMeta(target_table_meta, target_table_name)){
        cout<<"Error: cannot get table meta"<<endl;
        delete table_index;
        delete table_meta;
        delete target_table_meta;
        return false;
    }
    for(int i = 0; i < target_columns.size(); i++){
        bool found = false;
        for(int j = 0; j < target_table_meta->columns.size(); j++){
            if(target_columns[i] == target_table_meta->columns[j]->name){
                found = true;
                target_column_ids.push_back(j);
                break;
            }
        }
        if(!found){
            cout<<"Error: column "<<target_columns[i]<<" does not exist"<<endl;
            delete table_index;
            delete table_meta;
            delete target_table_meta;
            return false;
        }
    }

    // check if the targer columns are the primary key
    // also get the column_ids if they are the primary key
    if(target_table_meta->primaryKey == nullptr){
        cout<<"Error: target table does not have a primary key"<<endl;
        delete table_index;
        delete table_meta;
        delete target_table_meta;
        return false;
    } else {
        if(target_table_meta->primaryKey->columns.size() != target_columns.size()){
            cout<<"Error: target columns are not primary key"<<endl;
            delete table_index;
            delete table_meta;
            delete target_table_meta;
            return false;
        }
        for(int i = 0; i < target_table_meta->primaryKey->columns.size(); i++){
            if(target_table_meta->primaryKey->columns[i] != target_column_ids[i]){
                cout<<"Error: target columns are not primary key"<<endl;
                delete table_index;
                delete table_meta;
                delete target_table_meta;
                return false;
            }
        }
    }

    // check if every foreign key exists in the target table
    // get all the records from this table
    int fileID = record_manager->openTableFile(table_name.c_str());
    if(fileID == -1){
        cout<<"Error: cannot open table file"<<endl;
        delete table_index;
        delete table_meta;
        delete target_table_meta;
        return false;
    }
    Record example_record = table_meta->getExampleRecord();
    vector<Condition*> conds;
    vector<Record*> records = record_manager->selectRecords(fileID, conds, &example_record);

    // get the index map 
    IndexMap index_map;
    if(!getIndexMap(&index_map)){
        cout<<"Error: cannot get index map"<<endl;
        delete table_index;
        delete table_meta;
        delete target_table_meta;
        // free records
        record_manager->freeRecordList(records);
        // close
        record_manager->closeTableFile(fileID);
        return false;
    }
    // get the primary key index of the target table
    IndexMeta primary_index = index_map.getIndexMeta(target_table_meta->name + "_pri");
    if(primary_index.index_name == ""){
        cout<<"Error: cannot get primary key index"<<endl;
        delete table_index;
        delete table_meta;
        delete target_table_meta;
        // free records
        record_manager->freeRecordList(records);
        // close
        record_manager->closeTableFile(fileID);
        return false;
    }
    // get the index tree
    Tree* index_tree = new Tree(primary_index.index_name + "_idx", index_fm, index_bpm);
    bool result = index_tree->initTree();
    if(!result){
        cout<<"Error: cannot init index tree"<<endl;
        delete table_index;
        delete table_meta;
        delete target_table_meta;
        // free records
        record_manager->freeRecordList(records);
        // close
        record_manager->closeTableFile(fileID);
        delete index_tree;
        return false;
    }

    // go through the records
    for(int i=0; i<records.size(); i++){
        vector<int> vals;
        for(int j=0; j<source_column_ids.size(); j++){
            // check not null
            if(records[i]->bitmap[source_column_ids[j]] == 0){
                cout<<"Error: foreign key cannot be null"<<endl;
                delete table_index;
                delete table_meta;
                delete target_table_meta;
                // free records
                record_manager->freeRecordList(records);
                // close
                record_manager->closeTableFile(fileID);
                delete index_tree;
                return false;
            }

            // assemble the key
            int value = ((Int_Item*)records[i]->items[source_column_ids[j]])->integer;
            vals.push_back(value);
        }

        Key key = Key(vals);

        // check if the key exists in the target table
        Index index = index_tree->searchIndex(key);

        if(!(index.key == key) || (index.rid.pageIdx == 0 && index.rid.slotIdx == 0)){
            cout<<"Error: foreign key does not exist in target table"<<endl;
            delete table_index;
            delete table_meta;
            delete target_table_meta;
            // free records
            record_manager->freeRecordList(records);
            // close
            record_manager->closeTableFile(fileID);
            delete index_tree;
            return false;
        }
    }

    // pass check, assemble the foreign key
    ForeignKeyConstraint* foreign_key = new ForeignKeyConstraint(fk_name, target_table_id, target_column_ids, source_column_ids);
    table_meta->foreignKeys.push_back(foreign_key);
    table_meta->foreign_key_num++;
    // save
    result = setTableMeta(table_meta, table_name);
    if(!result){
        cout<<"Error: cannot save table meta"<<endl;
        delete table_index;
        delete table_meta;
        delete target_table_meta;
        // free records
        record_manager->freeRecordList(records);
        // close
        record_manager->closeTableFile(fileID);
        delete index_tree;
        return false;
    }

    // free records
    record_manager->freeRecordList(records);
    // close
    record_manager->closeTableFile(fileID);
    delete index_tree;
    delete table_index;
    delete table_meta;
    delete target_table_meta;

    cout<<"add foreign done";
    return true;
}

bool SystemManager::dropForeignKey(AlterForeignKeyStatement* statement){
    // get the table meta
    TableMeta* table_meta = new TableMeta();
    if(!getTableMeta(table_meta, statement->table_name)){
        cout<<"Error: cannot get table meta"<<endl;
        delete table_meta;
        return false;
    }

    // check if the foreign key exists
    bool found = false;
    for(int i=0; i<table_meta->foreignKeys.size(); i++){
        if(table_meta->foreignKeys[i]->name == statement->fk_name){
            found = true;
            table_meta->foreignKeys.erase(table_meta->foreignKeys.begin() + i);
            table_meta->foreign_key_num--;
            // set
            bool result = setTableMeta(table_meta, statement->table_name);
            if(!result){
                cout<<"Error: cannot set table meta"<<endl;
                delete table_meta;
                return false;
            }
            break;
        }
    }
    if(!found){
        cout<<"Error: foreign key does not exist"<<endl;
        delete table_meta;
        return false;
    }

    delete table_meta;

    cout<<"drop foreign done";
    return true;
}

bool SystemManager::showIndexes(){
    if(this->current_db == ""){
        cout<<"Error: no database selected"<<endl;
        return false;
    }
    IndexMap index_map;
    if(!getIndexMap(&index_map)){
        cout<<"Error: cannot get index map"<<endl;
        return false;
    }
    if(index_map.index_map.size() == 0){
        cout<<"No index found"<<endl;
        return true;
    }

    // table index
    TableIndex* table_index = new TableIndex();
    if(!getTableIndex(table_index)){
        cout<<"Error: cannot get table index"<<endl;
        delete table_index;
        return false;
    }
    
    cout<<setfill('-')<<setw(50)<<" "<<endl;
    for(int i=0; i<index_map.index_map.size(); i++){
        IndexMeta index_meta = index_map.index_map[i];
        unsigned int table_id = index_meta.table_idx;
        // get the table name
        string table_name = table_index->indexToTable[table_id];

        // get the table meta
        TableMeta* table_meta = new TableMeta();
        if(!getTableMeta(table_meta, table_name)){
            cout<<"Error: cannot get table meta"<<endl;
            delete table_meta;
            return false;
        }

        cout<<"INDEX "<<index_meta.index_name<<" ON "<<table_name<<"(";
        for(int j=0; j<index_meta.column_idx.size(); j++){
            cout<<table_meta->columns[index_meta.column_idx[j]]->name;
            if(j != index_meta.column_idx.size() - 1){
                cout<<", ";
            }
        }
        cout<<")"<<endl;
        delete table_meta;
    }
    cout<<setfill('-')<<setw(50)<<" "<<endl;
    cout<<index_map.index_map.size()<<" index(es) in set";
    delete table_index;
    
    return true;
}

string SystemManager::getCurrentDB(){
    return this->current_db;
}