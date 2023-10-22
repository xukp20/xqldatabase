/*
    high level class for insert, select, delete and update
*/

#ifndef QUERY_MANAGER_H
#define QUERY_MANAGER_H

#include <vector>
#include <utility>
#include "../statement_parse/SqlStatement.h"
#include "../record_management/RecordManager.h"
#include "../system_management/SystemManager.h"

class QueryManager {
public:
    RecordManager* record_manager;
    SystemManager* system_manager;
    FileManager* index_fm;
    BufPageManager* index_bpm;

    QueryManager(RecordManager* record_manager, SystemManager* system_manager, FileManager* index_fm, BufPageManager* index_bpm);
    ~QueryManager();
    bool select(SelectStatement* statement);
    bool insert(InsertStatement* statement);
    bool update(UpdateStatement* statement);
    bool delete_(DeleteStatement* statement);

    // for file input and output
    // no needs to check primary key or other constraints
    bool safe_insert(std::string file_name, std::string table_name);
    bool safe_select(std::string file_name, std::string table_name);

private:
    // tools 
    bool assembleRecord(const TableMeta* table_meta, const vector<Value*>& values, Record* record);
    // check if the primary keys are unique and not null
    bool checkPrimary(const TableMeta* table_meta, const Record* record);
    // check if the foreign keys are valid
    bool checkForeign(const TableMeta* table_meta, const Record* record);
    
    // check if the record matches the condition
    bool checkSingleRecord(const TableMeta* table_meta, Record* record, vector<Condition*>& conditions);
    // assemble the conditions according to the where clause
    bool assembleSingleConditions(const TableMeta* table_meta, vector<WhereClause*>& where_clauses, vector<Condition*>& conditions);
    // assemble the conditions according to the join clause
    bool assembleJoinConditions(const TableMeta* table_meta1, const TableMeta* table_meta2, vector<WhereClause*>& where_clauses, vector<Condition*>& conditions1, vector<Condition*>& conditions2, vector<Condition*>& cross_condition);
    // get the indexmeta according to the conditions
    // only for one table condition
    // return the selected index_id if found, else -1
    // also pass the condition_id1 and condition_id2
    int fetchSelectIndex(unsigned int table_id, IndexMap* index_map, vector<Condition*>& conditions, int& condition_id1, int& condition_id2);

    // for delete_
    // check if any foreign key is referencing the record
    bool checkReferenced(unsigned int table_id, vector<Record*> records);


    // find the column_id in the table
    // -1 for not found
    int findColumnId(const TableMeta* table_meta, const string& column_name);
    // get the condition according to operator
    // or opposite condition
    Condition::cond getTwoCondition(Operator::OperatorType op, bool opposite, bool is_str);
    // join the selected records according to the cross condition
    vector<pair<Record*, Record*>> joinRecords(vector<Record*>& records1, vector<Record*>& records2, vector<Condition*>& cross_condition);
};

#endif 
