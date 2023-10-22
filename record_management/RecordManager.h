//
// xukp: 2022/11/19 RecordManager
//
#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include <map>
#include <string>
#include <vector>
#include "Record.h"
#include "../utils/Condition.h"
#include "../filesystem/fileio/FileManager.h"
#include "../filesystem/bufmanager/BufPageManager.h"

struct meta_data{
    string table_name;
    unsigned int column_num;
    unsigned int record_bytes;
    unsigned int page_count;
    unsigned int next_spare;
    unsigned int first_slot_offset;
    unsigned int slot_count;

    void print(){
        cout<<"meta_data for table: "<<table_name<<endl
        <<" column_num: "<<column_num<<endl
        <<" record_bytes: "<<record_bytes<<endl
        <<" page_count: "<<page_count<<endl
        <<" next_spare: "<<next_spare<<endl
        <<" first_slot_offset: "<<first_slot_offset<<endl
        <<" slot_count: "<<slot_count<<endl<<endl;
    }
};


class RecordManager {
private:
    FileManager* fileManager;
    BufPageManager* bufPageManager;

    // store <fileID, meta_data> pairs
    map<int, meta_data> fileIDMap;
    // store <fileID, indexes> pairs
    map<int, vector<int>> fileIDIndexMap;

    // utils
    // allocate a new page for the table if all is full
    // return pageID, -1 for error
    int allocNewPage(const unsigned int fileID);

    // set the newly inserted page to be full
    void setFullPage(const unsigned int fileID, const unsigned int pageID);

    // meta data
    // get, update, remove
    // all 0s for not existing
    meta_data getMetaData(const int fileID);
    bool updateMetaData(const int fileID, meta_data meta);
    bool removeMetaData(const int fileID);

    // selectors
    // create a empty copy of the example record
    Record* emptyCopy(const Record* example);

    // batch
    std::vector<Record*> selectRecordsAluCompare(const unsigned int fileID, const CompareCondition* condition, Record* example);
    // single
    Record* selectRecordAluCompare(const unsigned int fileID, const CompareCondition* condition, Record* example, const RID& rid);

    friend class QueryManager;
public:
    RecordManager();
    ~RecordManager();

    // table manage
    // create a file for a new (NOT EXISTING) table, according to the name given
    // needs a template to define the data in the table
    bool createTableFile(const char* name, Record* example);
    
    // remove a EXISTING table file
    bool removeTableFile(const char* name);

    // open a EXISTING table file
    // returns the file id of the file in the FileManager
    // -1 for error
    int openTableFile(const char* name);

    // close an OPENED table file, of course it must be EXISTING
    bool closeTableFile(const char* name);
    bool closeTableFile(const int fileID);

    // close all the opened table files
    bool closeAll();

    // get fileID according to name
    // -1 for not existing
    unsigned int getFileID(const char* name);

    // record manage
    // insert a record into the opened table file
    RID insertRecord(const int fileID, Record* record);

    // delete a record from the opened table file
    // needs the RID of the record to locate it
    bool deleteRecord(const int fileID, const RID& rid);

    // update a record in the opened table file
    // needs the RID of the record to locate it
    // give a new record to replace the old one
    bool updateRecord(const int fileID, const RID& rid, Record* record);

    // select a series of record from the opened table file
    // should set the RID after getting the data
    std::vector<Record*> selectRecords(const int fileID, const Condition* cond, Record* example);

    // tool
    // match record with conditions
    bool matchRecord(const Record* record, const vector<Condition*>& conds);
    
    // select record according to conditions
    std::vector<Record*> selectRecords(const int fileID, const vector<Condition*>& conds, Record* example);

    // select record block according to the condition
    // given block size
    std::vector<Record*> selectFirstRecordsBlock(const int fileID, const vector<Condition*>& conds, Record* example, unsigned int block_size);
    // select record block after the rid position
    std::vector<Record*> selectNextRecordsBlock(const int fileID, const vector<Condition*>& conds, Record* example, unsigned int block_size, RID& rid);
    // select record block of the limit size from offset
    // -1 for no limit
    // 0 for no offset
    std::vector<Record*> selectRecordsBlock(const int fileID, const vector<Condition*>& conds, Record* example, unsigned int limit, unsigned int offset);

    // select record of the given rids
    std::vector<Record*> selectRecords(const int fileID, const vector<Condition*>& conds, const vector<RID>& rids, Record* example);

    // free record list
    void freeRecordList(vector<Record*>& recordList);
    
    // select the first record after the given location according to condition
    // return nullptr if no record is found
    Record* selectRecord(const int fileID, const Condition* cond, Record* example, const RID& rid);

    // select first/next record
    // return nullptr if no record is found
    Record* selectFirstRecord(const int fileID, Record* example);
    Record* selectNextRecord(const int fileID, Record* example, const RID& rid);
    
    // debug tools
    void printFile(const int fileID, Record* example, bool detail=true);
    void printPage(const int fileID, const int PageID, Record* example, bool detail=true);
}; 


#endif 