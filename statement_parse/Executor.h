/*
    Executor that calls the interfaces of query parser and system manager
*/

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <iostream>
#include <string>
#include <ctime>
#include "SqlStatement.h"
#include "../system_management/SystemManager.h"
#include "../record_management/RecordManager.h"
#include "../index_management/BplusTree/Tree.h"
#include "../filesystem/fileio/FileManager.h"
#include "../filesystem/bufmanager/BufPageManager.h"
#include "../query_management/query_manager.h"

using namespace std;

class Executor
{
public:
    SystemManager* system_manager;
    RecordManager* record_manager;
    QueryManager* query_manager;
    FileManager* index_fm;
    BufPageManager* index_bpm;

    // timer
    clock_t start_time;
    clock_t end_time;

    Executor(string base_path, bool init = false);
    ~Executor();
    bool init();
    bool run();
    bool run(DBStatement* statement);
    bool run(IOStatement* statement);
    bool run(TableStatement* statement);
    bool run(AlterStatement* statement);

    // for timer
    void startTimer();
    void endTimer();
    void printTime();

    // show next prefix
    void showNextPrefix();

    void printEnd();

    // convert parser node to the meta data for managers, and also pass the foreign keys
    // needs to delete the table meta outside
    bool convertTableMeta(CreateTableStatement* statement, TableMeta* table_meta, vector<ForeignKeyField*>& foreign_keys);
};

#endif