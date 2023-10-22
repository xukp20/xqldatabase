#ifndef SYSTEM_TESTER_H
#define SYSTEM_TESTER_H

#include "../record_management/Serializer.h"
#include "../system_management/SystemManager.h"
#include <fstream>
#include <filesystem>
using namespace std;
using namespace std::filesystem;

class SystemTester{
public:
    static void testTableMetaSerialize(){
        TableMeta* table_meta = new TableMeta("Hello", 3, 1);
        char* bytes = new char[8196];   // a page

        // add columns
        // Column1: int with default value
        DefaultValue* default_value = new DefaultValue();
        default_value->kind = 0;
        default_value->integer = 10;
        table_meta->addColumn(
            "Column1", 0, 4, default_value, true
        );
        // Column2: float without default
        table_meta->addColumn(
            "Column2", 1, 4, nullptr, false
        );
        // Column3: char with default value
        default_value = new DefaultValue();
        default_value->kind = 2;
        default_value->str = (char*)"Hello世界";
        table_meta->addColumn(
            "Column3", 2, 20, default_value, false
        );

        // add foreign key
        unsigned int target_table = 2;
        vector<unsigned int> target_columns;
        target_columns.push_back(0);
        target_columns.push_back(1);
        vector<unsigned int> source_columns;
        source_columns.push_back(0);
        source_columns.push_back(2);
        ForeignKeyConstraint* foreign_key = new ForeignKeyConstraint(
            "FK1", target_table, target_columns, source_columns
        );
        table_meta->addForeignKey(foreign_key);

        // add Primary Key
        vector<unsigned int> primary_key_columns;
        primary_key_columns.push_back(0);
        primary_key_columns.push_back(1);
        PrimaryKeyConstraint* primary_key = new PrimaryKeyConstraint(
            primary_key_columns
        );
        table_meta->addPrimaryKey(primary_key);

        table_meta->print();


        Serializer::serialize_table_meta(table_meta, bytes);


        TableMeta* out = new TableMeta("", 1, 0);
        Serializer::deserialize_table_meta(bytes, out);
        out->print();
    }

    static void testTableIndexSerialize(){
        TableIndex* table_index = new TableIndex();
        char* bytes = new char[8196];   // a page

        table_index->append(1, "Table1");
        table_index->append(2, "Table2");
        table_index->append(3, "Table3");
        table_index->append(4, "Table4");
        table_index->append(5, "Table5");
        table_index->print();

        table_index->deleteIndex(3);
        table_index->print();
        table_index->deleteIndex(4);
        table_index->print();
        table_index->append(table_index->nextIndex(), "Table6");
        table_index->print();

        Serializer::serialize_table_indexes(table_index, bytes);

        TableIndex* out = new TableIndex();
        Serializer::deserialize_table_indexes(bytes, out);
        out->print();
        return;
    }

    static void testDBIndexSerialize(){
        DBIndex* db_index = new DBIndex();
        char* bytes = new char[8196];   // a page

        db_index->append(1, "DB1");
        db_index->append(2, "DB2");
        db_index->append(3, "DB3");
        db_index->append(4, "DB4");
        db_index->append(5, "DB5");
        db_index->print();

        db_index->deleteIndex(3);
        db_index->print();
        db_index->deleteIndex(5);
        db_index->print();
        db_index->append(db_index->nextIndex(), "DB6");
        db_index->print();


        Serializer::serialize_db_indexes(db_index, bytes);

        DBIndex* out = new DBIndex();
        Serializer::deserialize_db_indexes(bytes, out);
        out->print();
        return;
    }

    static void testInitSystem(){
        SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
        cout<<system_manager->initSys();
    }

    static void testCreateDB(){
        SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
        system_manager->initPath();
        cout<<system_manager->createDatabase("DB1");
        cout<<system_manager->useDatabase("DB1");
        cout<<system_manager->createDatabase("DB2");
        cout<<system_manager->useDatabase("DB2");
        cout<<system_manager->useDatabase("DB3");
        cout<<system_manager->showDatabases();
        delete system_manager;
        return;
    }

    static void testDropDB(){
        testInitSystem();
        testCreateDB();
        SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
        cout<<system_manager->initPath();
        cout<<system_manager->dropDatabase("DB2");
        cout<<system_manager->showDatabases();
        cout<<system_manager->useDatabase("DB2");
        cout<<system_manager->createDatabase("DB4");
        cout<<system_manager->showDatabases();
        cout<<system_manager->useDatabase("DB5");
        cout<<system_manager->showDatabases();
        cout<<system_manager->useDatabase("DB1");
        cout<<system_manager->showTables();
        delete system_manager;
        return;
    }

    static void testcreateRawTable(){
        testInitSystem();
        SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
        system_manager->initPath();
        cout<<system_manager->createDatabase("DB1");
        cout<<"test create table"<<endl;
        cout<<system_manager->useDatabase("DB1");
        // assemble table meta
        TableMeta* table_meta = new TableMeta("Table1", 3, 1);
        // add columns
        // Column1: int with default value
        DefaultValue* default_value = new DefaultValue();
        default_value->kind = 0;
        default_value->integer = 10;
        table_meta->addColumn(
            "Column1", 0, 4, default_value, true
        );
        delete default_value;
        // Column2: float without default
        table_meta->addColumn(
            "Column2", 1, 4, nullptr, false
        );
        // Column3: char with default value
        default_value = new DefaultValue();
        default_value->kind = 2;
        default_value->str = (char*)"Hello World";
        table_meta->addColumn(
            "Column3", 2, 20, default_value, false
        );
        // add foreign key
        unsigned int target_table = 2;
        vector<unsigned int> target_columns;
        target_columns.push_back(0);
        target_columns.push_back(1);
        vector<unsigned int> source_columns;
        source_columns.push_back(0);
        source_columns.push_back(2);
        ForeignKeyConstraint* foreign_key = new ForeignKeyConstraint(
            "FK1", target_table, target_columns, source_columns
        );
        table_meta->addForeignKey(foreign_key);
        // add primary key
        vector<unsigned int> primary_key_columns;
        primary_key_columns.push_back(0);
        primary_key_columns.push_back(1);
        PrimaryKeyConstraint* primary_key = new PrimaryKeyConstraint(
            primary_key_columns
        );
        table_meta->addPrimaryKey(primary_key);

        // table_meta->print();
        cout<<"create table"<<endl;
        // cout<<system_manager->showTables();
        cout<<system_manager->createRawTable(table_meta);
        cout<<system_manager->showTables();
        cout<<"desc table"<<endl;
        cout<<system_manager->descTable("Table1");
        // cout<<"drop table"<<endl;
        // cout<<system_manager->dropTable("Table1");
        // cout<<system_manager->showTables();
        cout<<system_manager->showDatabases();

        delete system_manager;
        return;
    }

    static void testSetTableIndex(){
        testInitSystem();
        SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
        system_manager->initPath();
        cout<<system_manager->createDatabase("DB1");
        cout<<"test create table"<<endl;
        cout<<system_manager->useDatabase("DB1");
        // assemble table meta
        TableMeta* table_meta = new TableMeta("Table1", 0, 0);

        TableIndex* table_index = new TableIndex();
        system_manager->getTableIndex(table_index);
        table_index->print();
        table_index->append(1, "Table1");
        table_index->print();
        system_manager->setTableIndex(table_index);
        TableIndex* out = new TableIndex();
        system_manager->getTableIndex(out);
        out->print();
        delete out;
        delete table_index;
        return;
    }

    static void testReadWritePage(){
        testInitSystem();
        SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
        system_manager->initPath();
        path file_path("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql/test0");
        ofstream file("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql/test0", ios::binary);
        char* buffer = new char[PAGE_SIZE];
        for(int i = 0; i < 16; i++){
            buffer[i] = i;
        }
        file.seekp(1 * PAGE_SIZE, ios::beg);    // write to page 1
        file.write(buffer, PAGE_SIZE);
        file.close();

        // readPage
        memset(buffer, 0, PAGE_SIZE);
        system_manager->readPage(file_path, buffer, 1);
        // print the page in hex
        for(int i = 0; i < 16; i++){
            printf("%02x ", (unsigned char)buffer[i]);
        }
        cout<<endl;

        // writePage
        for(int i = 0; i < 16; i++){
            buffer[i] = i + 1;
        }
        system_manager->writePage(buffer, file_path, 2);
        // readPage
        memset(buffer, 0, PAGE_SIZE);
        system_manager->readPage(file_path, buffer, 2);
        // print the page in hex
        for(int i = 0; i < 16; i++){
            printf("%02x ", (unsigned char)buffer[i]);
        }
        cout<<endl;
        // read 0
        memset(buffer, 0, PAGE_SIZE);
        system_manager->readPage(file_path, buffer, 0);
        // print the page in hex
        for(int i = 0; i < 16; i++){
            printf("%02x ", (unsigned char)buffer[i]);
        }
        cout<<endl;
        // read 1
        memset(buffer, 0, PAGE_SIZE);
        system_manager->readPage(file_path, buffer, 1);
        // print the page in hex
        for(int i = 0; i < 16; i++){
            printf("%02x ", (unsigned char)buffer[i]);
        }
        cout<<endl;
    }

    static void testCreateIndex();
    static void testConflictIndex();
};


void SystemTester::testCreateIndex(){
    testcreateRawTable();
    SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
    system_manager->initPath();
    system_manager->useDatabase("DB1");

    // create index
    vector<unsigned int> index_columns;
    index_columns.push_back(0);
    system_manager->createIndex("Table1", "Index1", index_columns, 0);
    system_manager->showIndexes();

    // create bad index
    index_columns.push_back(4);
    system_manager->createIndex("Table1", "Index2", index_columns, 0);
    system_manager->showIndexes();

    // create index
    index_columns.clear();
    index_columns.push_back(0);
    index_columns.push_back(1);
    system_manager->createIndex("Table1", "Index3", index_columns, 0);
    system_manager->showIndexes();

    // drop index
    system_manager->dropIndex("Index1", 0);
    system_manager->showIndexes();

    // drop index
    system_manager->dropIndex("Index3", 0);
    system_manager->showIndexes();

    // create new table
    TableMeta* table_meta = new TableMeta("Table2", 3, 0);
    table_meta->addColumn(
        "id", 0, 4, 0, 0
    );
    table_meta->addColumn(
        "len", 0, 4, 0, 0
    );
    table_meta->addColumn(
        "val", 0, 4, 0, 0
    );

    cout<<"create table"<<endl;
    // cout<<system_manager->showTables();
    // cout<<"table2 has primary key: "<<(table_meta->primaryKey != nullptr)<<endl;
    cout<<system_manager->createRawTable(table_meta);
    cout<<system_manager->showTables();
    cout<<"desc table"<<endl;
    cout<<system_manager->descTable("Table2");

    // create index
    index_columns.clear();
    index_columns.push_back(0);
    cout<<system_manager->createIndex("Table2", "Index4", index_columns, 0);

    // drop
    system_manager->dropIndex("Index4", 0);

    // create index with two columns
    index_columns.clear();
    index_columns.push_back(0);
    index_columns.push_back(1);
    cout<<system_manager->createIndex("Table2", "Index5", index_columns, 0);

    // show indexes
    system_manager->showIndexes();

    // drop
    system_manager->dropIndex("Index4", 0);
    system_manager->dropIndex("Index5", 0);

    // show indexes
    system_manager->showIndexes();

    delete system_manager;
    return;
}

void SystemTester::testConflictIndex(){
    testcreateRawTable();
    SystemManager* system_manager = new SystemManager("/mnt/e/2022autumn/数据库系统概论/xqldatabase/build/xql");
    system_manager->initPath();
    system_manager->useDatabase("DB1");

    // in table 1, column 0
    // create index
    vector<unsigned int> index_columns;
    index_columns.push_back(0);
    system_manager->createIndex("Table1", "Index1", index_columns, 0);
    system_manager->showIndexes();   

    // create again
    // create index
    system_manager->createIndex("Table1", "Index1", index_columns, 0);
    system_manager->showIndexes(); 

    // create new table
    TableMeta* table_meta = new TableMeta("Table2", 3, 0);
    table_meta->addColumn(
        "id", 0, 4, 0, 0
    );
    table_meta->addColumn(
        "len", 0, 4, 0, 0
    );
    table_meta->addColumn(
        "val", 0, 4, 0, 0
    );

    cout<<"create table"<<endl;
    // cout<<system_manager->showTables();
    cout<<system_manager->createRawTable(table_meta);
    cout<<system_manager->showTables();
    cout<<"desc table"<<endl;
    cout<<system_manager->descTable("Table2");

    // create on table 2, column 0
    // create index
    index_columns.clear();
    index_columns.push_back(0);
    cout<<system_manager->createIndex("Table2", "Index2", index_columns, 0);

    // create again
    // create index
    cout<<system_manager->createIndex("Table2", "Index3", index_columns, 0);

    // create on table 2, column 0, 1
    // create index
    index_columns.clear();
    index_columns.push_back(0);
    index_columns.push_back(1);
    cout<<system_manager->createIndex("Table2", "Index4", index_columns, 0);

    // create 1,0
    // create index
    index_columns.clear();
    index_columns.push_back(1);
    index_columns.push_back(0);
    cout<<system_manager->createIndex("Table2", "Index5", index_columns, 0);

    // show
    system_manager->showIndexes();

    // upgrade 0, 1 to primary
    index_columns.clear();
    index_columns.push_back(0);
    index_columns.push_back(1);
    cout<<system_manager->createIndex("Table2", "Index6", index_columns, 1);

    // show
    system_manager->showIndexes();

    // again
    // upgrade 0, 1 to primary
    index_columns.clear();
    index_columns.push_back(0);
    index_columns.push_back(1);
    cout<<system_manager->createIndex("Table2", "Index6", index_columns, 1);

    // drop with normal
    system_manager->dropIndex("Index4", 0);
    system_manager->showIndexes();

    // drop with primary
    system_manager->dropIndex("Index4", 1);
    system_manager->showIndexes();

    // again
    // upgrade 0, 1 to primary
    index_columns.clear();
    index_columns.push_back(0);
    index_columns.push_back(1);
    cout<<system_manager->createIndex("Table2", "Index6", index_columns, 1);

    // upgrade to primary + unique
    index_columns.clear();
    index_columns.push_back(0);
    index_columns.push_back(1);
    cout<<system_manager->createIndex("Table2", "Index7", index_columns, 2);

    // show 
    system_manager->showIndexes();
}
#endif