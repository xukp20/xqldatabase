#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <string>
#include <cstring>
#include <vector>
#include <filesystem>
#include "DBIndex.h"
#include "../record_management/Serializer.h"
#include "IndexMap.h"
#include "../index_management/BplusTree/Tree.h"
#include "../record_management/RecordManager.h"
#include "../statement_parse/SqlStatement.h"

using namespace std;
using namespace std::filesystem;

#define NORMAL_INDEX 0
#define PRIMARY_INDEX 1
#define UNIQUE_INDEX 2
#define UNIQUE_PRIMARY_INDEX 3

class SystemManager {
private:
    // base path of all the data bases
    string base_path;

    // current database
    string current_db;

    // tools 
    bool readPage(path file_path, char* page, unsigned int page_id);
    bool writePage(char* page, path file_path, unsigned int page_id);
    // get DBIndex and TableIndex from the file
    bool getDBIndex(DBIndex* db_index);
    bool setDBIndex(DBIndex* db_index);
    bool getTableIndex(TableIndex* table_index);
    bool setTableIndex(TableIndex* table_index);
    // get IndexMap from the index file
    bool getIndexMap(IndexMap* index_map);
    bool setIndexMap(IndexMap* index_map);
    // get TableMeta from the index file
    bool getTableMeta(TableMeta* table_meta, string table_name);
    bool setTableMeta(TableMeta* table_meta, string table_name);
    // only clear the page of that table
    bool clearTableMeta(string table_name);
    friend class SystemTester;
    friend class QueryManager;
public:
    SystemManager(string base_path);
    ~SystemManager();

    // initialize system
    bool initSys();

    // change to the base path of the system
    // called everytime when the system starts
    bool initPath();

    // create a new database
    bool createDatabase(string db_name);

    // drop a database
    bool dropDatabase(string db_name);

    // use a database
    bool useDatabase(string db_name);

    // show all databases
    bool showDatabases();

    // show all tables in the current database
    bool showTables();

    // tools for creating table
    // create the table file after checking the meta data
    bool createRawTable(TableMeta* table_meta, RecordManager* record_manager);

    // create a table
    // 1. check the meta data
    // 2. check foreign key references
    // 3. create the table file
    // 4. create the primary index file
    bool createTable(TableMeta* table_meta, vector<ForeignKeyField*>& foreign_keys, RecordManager* record_manager);

    // drop a table
    bool dropTable(string table_name);

    // show the meta data of a table
    bool descTable(string table_name);

    // create an index on the table and the given columns
    bool createIndex(string table_name, string index_name, vector<unsigned int> columns, unsigned int kind);

    // called by the executor
    // use record manager to get all the records and insert them into the index
    bool createIndex(AlterStatement* statement, RecordManager* record_manager, FileManager* index_fm, BufPageManager* index_bpm);

    // drop an index
    bool dropIndex(string index_name, unsigned int kind);

    // called by the executor
    bool dropIndex(AlterStatement* statement);

    // add primary key
    bool addPrimaryKey(AlterStatement* statement, RecordManager* record_manager, FileManager* index_fm, BufPageManager* index_bpm);

    // drop primary key
    bool dropPrimaryKey(AlterStatement* statement);

    // add foreign key
    bool addForeignKey(AlterForeignKeyStatement* statement, RecordManager* record_manager, FileManager* index_fm, BufPageManager* index_bpm);

    // drop foreign key
    bool dropForeignKey(AlterForeignKeyStatement* statement);

    // show all indexes
    bool showIndexes();

    // tools for executor
    string getCurrentDB();
};

#endif