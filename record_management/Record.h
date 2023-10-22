//
// Created by Xu kangping on 2022/11/19.
//

#ifndef PROJECT_RECORD_H
#define PROJECT_RECORD_H

#include <vector>
#include <string>
#include "Record_Item.h"
#include "Rid.h"

class Record {
private:
    vector<bool> bitmap;            // to save if column is not NULL
    vector<Record_Item*> items;     // columns' values
    RID rid;                        // record id, to locate the record in the file + page
public:
    Record(){}
    ~Record();

    // interfaces
    void append(bool not_null, int integer);
    void append(bool not_null, float floating);
    void append(bool not_null, const char* str, unsigned int len);
    void setRID(const unsigned int pID, const unsigned int sID);
    RID getRID() {
        return this->rid;
    }
    friend class Serializer;
    friend class RecordManager;
    friend class QueryManager;
    friend class SystemManager;

    // tools
    void print();
};


#endif //PROJECT_RECORD_H
