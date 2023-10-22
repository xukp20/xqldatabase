#include <sys/stat.h>
#include "RecordManager.h"
#include "Serializer.h"

#define PAGE_HEADER_SIZE 4          // reserved for page header meta data
#define PAGE_SLOT_BITMAP_SIZE 16    // reserved for slot bitmap
#define NULL_PAGE 0                 // symbol for null page

RecordManager::RecordManager() {
    this->fileManager = new FileManager();
    this->bufPageManager = new BufPageManager(this->fileManager);

    // init here
    MyBitMap::initConst();
}

RecordManager::~RecordManager() {
    // free here
    this->bufPageManager->close();

    if(this->fileManager != nullptr){
        delete this->fileManager;
        this->fileManager = nullptr;
    }
    if(this->bufPageManager != nullptr){
        delete this->bufPageManager;
        this->bufPageManager = nullptr;
    }
}


bool RecordManager::createTableFile(const char* name, Record* example) {
    // check if the file exists or create fails
    // cout<<"create table file: "<<name<<endl;
    struct stat buffer;
    if (stat(name, &buffer) == 0) {     // exists
        return false;
    }

    if(!this->fileManager->createFile(name)){   // create fails
        return false;
    }

    // first create a page for meta data
    int fileID = 0;
    bool result = this->fileManager->openFile(name, fileID);
    if(!result){
        return false;
    }

    int index;   // index of the page in the buffer
    // this new page is not in the buffer, use allocPage to get it
    BufType meta_page = this->bufPageManager->allocPage(fileID, 0, index, false);
    // put meta data inside
    unsigned int count = example->bitmap.size();
    meta_page[0] = count;           // column number
    int bytes = (count + 7) / 8;    // bytes for bitmap of null info
    for (int i = 0; i < count; i++) {
        switch (example->items[i]->kind)
        {
            case 0:     // int
                bytes += 4;
                break;
            case 1:     // float
                bytes += 4;
                break;
            case 2:     // char
                bytes += ((Char_Item*)(example->items[i]))->len;
                break;
            default:
                break;
        }
    }
    meta_page[1] = bytes;       // bytes of each record
    meta_page[2] = 1;           // total page number
    meta_page[3] = NULL_PAGE;   // the next page with spare space, 0 for none
    meta_page[4] = PAGE_HEADER_SIZE * 4 + PAGE_SLOT_BITMAP_SIZE * 4;        // start offset of the first slot
    unsigned int slot_count = (PAGE_SIZE - meta_page[4]) / bytes;  
    meta_page[5] = slot_count < PAGE_SLOT_BITMAP_SIZE * 32 ? slot_count : PAGE_SLOT_BITMAP_SIZE * 32;  // slot count of each page   
    // TODO: add more meta data here 
    this->bufPageManager->markDirty(index);
    this->bufPageManager->writeBack(index);
    
    return true;
}


// utils for slot bitmap
void initSlotBitmap(char* bitmap, int count) {
    for (int i = 0; i < (count + 7) / 8; i++) {
        bitmap[i] = 0b00000000;
    }
}

void setSlotBitMap(char* bitmap, unsigned index, bool value) {
    if (value) {
        bitmap[index / 8] |= (1 << (7 - index % 8));
    } else {
        bitmap[index / 8] &= ~(1 << (7 - index % 8));
    }
}

bool getSlotBitMap(char* bitmap, unsigned index) {
    return bitmap[index / 8] & (1 << (7 - index % 8));
}

void printSlotBitMap(char* bitmap, int count) {
    for (int i = 0; i < count; i++) {
        if (getSlotBitMap(bitmap, i)) {
            std::cout << "1 ";
        } else {
            std::cout << "0 ";
        }
    }
    std::cout << std::endl;
}


int RecordManager::allocNewPage(const unsigned int fileID){
    // read meta data from the meta page
    // cout<<"allocNewPage"<<endl;
    int meta_index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, meta_index);
    int new_pageID = meta_page[2];      // this page's pageID
    int index;
    BufType next_page = this->bufPageManager->allocPage(fileID, new_pageID, index, false);
    this->fileIDIndexMap[fileID].push_back(index);  // add new index of this file

    // set the header of the new page
    next_page[0] = meta_page[3];        // link to the old spare page
    meta_page[3] = new_pageID;          // update the spare page list header
    meta_page[2] += 1;                  // new page

    // init the slot bitmap
    next_page[1] = meta_page[5];        // slot count
    initSlotBitmap((char*)(next_page + PAGE_HEADER_SIZE), meta_page[5]);
    next_page[2] = 0;                   // used slot count
    
    // store record len
    next_page[3] = meta_page[1];

    // new meta data
    meta_data old_meta = getMetaData(fileID);
    meta_data new_meta = meta_data{
        old_meta.table_name, 
        meta_page[0], meta_page[1], meta_page[2], meta_page[3], meta_page[4], meta_page[5]
    };
    updateMetaData(fileID, new_meta);
    this->bufPageManager->markDirty(meta_index);

    return new_pageID;
} 


bool RecordManager::removeTableFile(const char* name) {
    // check if opened
    for (map<int, meta_data>::iterator it = this->fileIDMap.begin(); it != this->fileIDMap.end(); it++) {
        if (it->second.table_name == string(name)) {
            return false;
        }
    }

    // check if the file exists or remove fails
    struct stat buffer;
    if (stat(name, &buffer) != 0) {     // not exists
        return false;
    }

    if(remove(name) != 0){   // remove fails
        return false;
    }

    return true;
}


int RecordManager::openTableFile(const char* name) {
    int fileID = 0;
    bool result = this->fileManager->openFile(name, fileID);
    if(!result){
        return -1;
    }

    // load the meta page 
    int index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, index);
    // save meta data
    meta_data meta = {string(name), meta_page[0], meta_page[1], meta_page[2], meta_page[3], meta_page[4], meta_page[5]};
    // meta.print();
    this->fileIDMap.insert(std::pair<int, meta_data>(fileID, meta));
    // save buf info
    this->fileIDIndexMap.insert(std::pair<int, vector<int>>(fileID, vector<int>(1, index)));
    // cout<<"openTableFile: "<<name<<" "<<fileID<<endl;

    // // now fileIDMap
    // for(map<int, meta_data>::iterator it = this->fileIDMap.begin(); it != this->fileIDMap.end(); it++){
    //     cout<<it->first<<" ";
    //     it->second.print();
    // }
    // // now fileIDIndexMap
    // for(map<int, vector<int>>::iterator it = this->fileIDIndexMap.begin(); it != this->fileIDIndexMap.end(); it++){
    //     cout<<it->first<<" ";
    //     for(int i = 0; i < it->second.size(); i++){
    //         cout<<it->second[i]<<" ";
    //     }
    //     cout<<endl;
    // }
    return fileID;
}


bool RecordManager::closeTableFile(const char* name) {
    map<int, meta_data>::iterator it;
    unsigned int fileID;
    for (it = this->fileIDMap.begin(); it != this->fileIDMap.end(); it++) {
        if (it->second.table_name == string(name)) {
            fileID = it->first;
            break;
        }
    }

    if (it == this->fileIDMap.end()) {
        return false;
    } else {
        return this->closeTableFile(fileID);
    }
}

bool RecordManager::closeTableFile(const int fileID) {
    // save buf back to the disk
    // cout<<"closeTableFile: "<<fileID<<endl;
    vector<int> index_list = this->fileIDIndexMap[fileID];
    // int buf_fileID, buf_pageID;
    // for(int i = 0; i < index_list.size(); i++){
    //     this->bufPageManager->getKey(index_list[i], buf_fileID, buf_pageID);
    //     // if the page of index is for this file, save it
    //     if(buf_fileID == fileID){
    //         this->bufPageManager->writeBack(index_list[i]);
    //     }
    // }
    this->bufPageManager->close();
    // remove this file's buf info and meta_data
    this->fileIDMap.erase(fileID);
    this->fileIDIndexMap.erase(fileID);

    // // now fileIDMap
    // for(map<int, meta_data>::iterator it = this->fileIDMap.begin(); it != this->fileIDMap.end(); it++){
    //     cout<<it->first<<" ";
    //     it->second.print();
    // }
    // // now fileIDIndexMap
    // for(map<int, vector<int>>::iterator it = this->fileIDIndexMap.begin(); it != this->fileIDIndexMap.end(); it++){
    //     cout<<it->first<<" ";
    //     for(int i = 0; i < it->second.size(); i++){
    //         cout<<it->second[i]<<" ";
    //     }
    //     cout<<endl;
    // }

    // close file
    int result = this->fileManager->closeFile(fileID);
    // cout<<"result "<<result<<endl;
    return !result;
}

bool RecordManager::closeAll(){
    // write back all the buf
    this->bufPageManager->close();

    // close all the files
    for(map<int, meta_data>::iterator it = this->fileIDMap.begin(); it != this->fileIDMap.end(); it++){
        this->fileManager->closeFile(it->first);
        // cout<<"close file "<<it->first<<endl;
    }

    // clear all the info
    this->fileIDMap.clear();
    this->fileIDIndexMap.clear();

    delete this->fileManager;
    delete this->bufPageManager;

    this->fileManager = new FileManager();
    this->bufPageManager = new BufPageManager(this->fileManager);

    return true;
}


meta_data RecordManager::getMetaData(const int fileID) {
    map<int, meta_data>::iterator it;
    it = this->fileIDMap.find(fileID);
    if (it == this->fileIDMap.end()) {
        return meta_data{"", 0, 0, 0, 0, 0, 0};
    } else {
        return it->second;
    }
}

bool RecordManager::updateMetaData(const int fileID, meta_data meta) {
    map<int, meta_data>::iterator it;
    it = this->fileIDMap.find(fileID);
    if (it == this->fileIDMap.end()) {
        return false;
    } else {
        it->second = meta;
        return true;
    }
}

bool RecordManager::removeMetaData(const int fileID){
    map<int, meta_data>::iterator it;
    it = this->fileIDMap.find(fileID);
    if (it == this->fileIDMap.end()) {
        return false;
    } else {
        this->fileIDMap.erase(it);
        return true;
    }
}

unsigned int RecordManager::getFileID(const char* name){
    map<int, meta_data>::iterator it;
    for (it = this->fileIDMap.begin(); it != this->fileIDMap.end(); it++) {
        if (it->second.table_name == string(name)) {
            break;
        }
    }

    if (it == this->fileIDMap.end()) {
        return -1;
    } else {
        return it->first;
    }
}


// record management
RID RecordManager::insertRecord(const int fileID, Record* record) {
    // read meta data from the meta page

    meta_data meta = getMetaData(fileID);
    // meta.print();
    // cout<<"1";

    int pageID = meta.next_spare;      // the spare page list header
    if (pageID == 0) {             // no spare page
        pageID = allocNewPage(fileID);
        meta = getMetaData(fileID);
        // meta.print();
    }
    // cout<<"1.5";
    int slot_count = meta.slot_count;
    int bytes = meta.record_bytes;
    int slots_offset = meta.first_slot_offset;

    // cout<<"slot_count: "<<meta.slot_count;
    // cout<<"bytes: "<<meta.record_bytes;
    // cout<<"slot_offset: "<<meta.first_slot_offset;

    // find the spare slot
    // cout<<"2";
    int index;
    int slot_index = -1;
    BufType page = this->bufPageManager->getPage(fileID, pageID, index);
    char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
    // printSlotBitMap(bitmap, slot_count);
    for (int i = 0; i < slot_count; i++) {
        if (!getSlotBitMap(bitmap, i)) {    // empty
            slot_index = i;
            // cout<<"find empty slot: "<<slot_index<<endl;
            break;
        }
    }
    if(slot_index == -1){
        return RID(0, 0);       // error if this occurs
    }

    // write the record to the slot
    char* start_offset = ((char*)page) + slots_offset + slot_index * bytes;
    // cout<<"write to page: "<<pageID<<endl;
    // cout<<"offset: "<<slots_offset + slot_index * bytes<<endl;
    unsigned int len = Serializer::serialize_record(record, start_offset);
    // cout<<"serialize record length: "<<len<<endl;
    if (len != bytes) {
        cout<<"serialize record length: "<<len<<endl;
        cout<<"record bytes: "<<bytes<<endl;
        return RID(0, 0);       // error if serialized bytes != bytes of each record
    }

    // check if the page is full
    setSlotBitMap(bitmap, slot_index, true);
    
    // printSlotBitMap(bitmap, slot_count);

    page[2] += 1;                   // used slot count
    if(page[2] == slot_count){      // full
        int meta_index; 
        BufType meta_page = this->bufPageManager->getPage(fileID, 0, meta_index);
        meta_page[3] = page[0];     // update the spare page list header
        page[0] = NULL_PAGE;        // set the next page to NULL_PAGE
        meta.next_spare = meta_page[3];
        updateMetaData(fileID, meta);
        this->bufPageManager->markDirty(meta_index);
    }

    // set dirty
    this->bufPageManager->markDirty(index);
    // this->bufPageManager->writeBack(index);

    return RID(pageID, slot_index);
}


bool RecordManager::deleteRecord(const int fileID, const RID& rid){
    unsigned int pageID = rid.pageIdx;
    unsigned int slotID = rid.slotIdx;
    int index;
    BufType page = this->bufPageManager->getPage(fileID, pageID, index);
    this->fileIDIndexMap[fileID].push_back(index);

    // clear the slot
    setSlotBitMap((char*)(page + PAGE_HEADER_SIZE), slotID, false);

    // if was full, now set to be a spare page
    page[2] -= 1;                   // used slot count
    if(page[2] == page[1] - 1){     // was full
        int meta_index;
        BufType meta_page = this->bufPageManager->getPage(fileID, 0, meta_index);   // open meta page
        page[0] = meta_page[3];
        meta_page[3] = pageID;      // update the spare page list header
        
        // update meta data
        meta_data old_meta = getMetaData(fileID);
        updateMetaData(fileID, {old_meta.table_name, meta_page[0], meta_page[1], meta_page[2], meta_page[3], meta_page[4], meta_page[5]});
        this->bufPageManager->markDirty(meta_index);
    }

    // set dirty
    this->bufPageManager->markDirty(index);
    return true;
}

bool RecordManager::updateRecord(const int fileID, const RID& rid, Record* record){
    unsigned int pageID = rid.pageIdx;
    unsigned int slotID = rid.slotIdx;
    int index;
    BufType page = this->bufPageManager->getPage(fileID, pageID, index);
    this->fileIDIndexMap[fileID].push_back(index);

    // check if the slot is empty
    if(!getSlotBitMap((char*)(page + PAGE_HEADER_SIZE), slotID)){
        return false;
    }

    // write the record to the slot
    int bytes = page[3];
    int slots_offset = PAGE_HEADER_SIZE * 4 + PAGE_SLOT_BITMAP_SIZE * 4;
    char* start_offset = ((char*)page) + slots_offset + slotID * bytes;
    unsigned int len = Serializer::serialize_record(record, start_offset);
    if (len != bytes) {
        return false;       // error if serialized bytes != bytes of each record
    }

    // set dirty
    this->bufPageManager->markDirty(index);

    return true;
}


/* record list select */ 
Record* RecordManager::emptyCopy(const Record* record){
    Record* new_record = new Record();
    new_record->bitmap = record->bitmap;
    for(int i=0; i < record->items.size(); i++){
        switch (record->items[i]->kind)
        {
            case 0:    // int
                new_record->items.push_back(new Int_Item());
                break;

            case 1:    // float
                new_record->items.push_back(new Float_Item());
                break;

            case 2:    // string
                new_record->items.push_back(new Char_Item((char*)" ", ((Char_Item*)record->items[i])->len));
                break;
            
            default:
                break;
        }
    } 
    return new_record;
}

void RecordManager::freeRecordList(vector<Record*>& record_list){
    for(int i=0; i < record_list.size(); i++){
        if(record_list[i])
            delete record_list[i];
    }
}

// compare condition
std::vector<Record*> RecordManager::selectRecordsAluCompare(const unsigned int fileID, const CompareCondition* condition, Record* example){
    std::vector<Record*> result;
    int index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, index);
    meta_data meta = getMetaData(fileID); 
    unsigned int page_count = meta.page_count;
    unsigned int slot_count = meta.slot_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;

    // iterate all pages
    for (int i = 1; i <= page_count; i++) {
        BufType page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                // set the RID
                record->rid = RID(i, j);
                
                // compare column 
                unsigned int column = condition->column;
                
                double value = 0;
                if(record->items[column]->kind == 0){
                    value = (double)((Int_Item*)record->items[column])->integer;
                } else if(record->items[column]->kind == 1){
                    value = (double)((Float_Item*)record->items[column])->floating;
                } else {
                    delete record;
                    return result;      // error if the column is not int or float
                }

                if (condition->compare(value)) {
                    result.push_back(record);
                } else {                // remove this record
                    delete record;
                }
            }
            start_offset += bytes;
        }
    }
    return result;
}


std::vector<Record*> RecordManager::selectRecords(const int fileID, const Condition* condition, Record* example){
    // TODO: condition == nullptr
    switch (condition->kind) {
        case Condition::EQ:
        case Condition::NE:
        case Condition::LT:
        case Condition::LE:
        case Condition::GT:
        case Condition::GE:
            return selectRecordsAluCompare(fileID, (CompareCondition*)condition, example);

        default:
            return std::vector<Record*>();
    }
}

std::vector<Record*> RecordManager::selectRecords(const int fileID, const vector<Condition*>& conds, const vector<RID>& rids, Record* example){
    std::vector<Record*> result;
    int index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, index);
    meta_data meta = getMetaData(fileID); 
    unsigned int page_count = meta.page_count;
    unsigned int slot_count = meta.slot_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;

    // iterate through all the pages, and get all the rids in the page
    for (int i = 1; i <= page_count; i++) {
        bool first = true;  // first in this page
        BufType page;
        char* bitmap;
        char* start_offset;
        for (int j = 0; j < rids.size(); j++){
            if (rids[j].pageIdx == i) {
                if(first){
                    page = this->bufPageManager->getPage(fileID, i, index);
                    bitmap = (char*)(page + PAGE_HEADER_SIZE);
                    start_offset = ((char*)page) + slots_offset;
                    first = false;
                }

                // get the slot
                unsigned int slotIdx = rids[j].slotIdx;
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset + slotIdx * bytes, record);
                // set the RID
                record->rid = rids[j];

                // check conditions
                bool match = matchRecord(record, conds);

                if (match) {
                    result.push_back(record);
                } else {                // remove this record
                    delete record;
                }
            }
        }
    }
    return result;
}


std::vector<Record*> RecordManager::selectRecords(const int fileID, const vector<Condition*>& conds, Record* example){
    std::vector<Record*> result;
    int index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, index);
    meta_data meta = getMetaData(fileID); 
    unsigned int page_count = meta.page_count;
    unsigned int slot_count = meta.slot_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;
    
    // cout<<"page_count"<<meta.page_count;
    // cout<<"slot_count"<<meta.slot_count;
    // cout<<"bytes"<<meta.record_bytes;
    // cout<<"first slots_offset"<<meta.first_slot_offset;

    // iterate all pages
    for (int i = 1; i <= page_count; i++) {
        BufType page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                // set the RID
                record->rid = RID(i, j);

                // cout<<"get record"<<endl;
                // record->print();
                
                // compare column 
                bool match = matchRecord(record, conds);

                if (match) {
                    result.push_back(record);
                } else {                // remove this record
                    delete record;
                }
            }
            start_offset += bytes;
        }
    }
    return result;
}

bool RecordManager::matchRecord(const Record* record, const vector<Condition*>& conds){
    // compare column 
    for(int k=0; k<conds.size(); k++){
        unsigned int column1 = conds[k]->column;
        
        switch (conds[k]->kind)
        {
            case Condition::EQ:
            case Condition::NE:
            case Condition::LT:
            case Condition::LE:
            case Condition::GT:
            case Condition::GE:
            {
                // test if null
                if(record->bitmap[column1] == 0){
                    return false;
                }

                double value = 0;
                if(record->items[column1]->kind == 0){
                    value = (double)((Int_Item*)record->items[column1])->integer;
                } else if(record->items[column1]->kind == 1){
                    value = (double)((Float_Item*)record->items[column1])->floating;
                }

                if(!((CompareCondition*)conds[k])->compare(value)){
                    return false;
                } else {
                    break;
                }
            }

            case Condition::LIKE:
            {
                // test if null
                if(record->bitmap[column1] == 0){
                    return false;
                }

                string value = ((Char_Item*)record->items[column1])->str;
                if(!((StringCondition*)conds[k])->compare(value)){
                    return false;
                } else {
                    break;
                }
            }

            case Condition::IN:
            {
                // test if null
                if(record->bitmap[column1] == 0){
                    return false;
                }

                if(record->items[column1]->kind == 2){
                    string value = ((Char_Item*)record->items[column1])->str;
                    if(!((StrListCondition*)conds[k])->compare(value)){
                        return false;
                    } else {
                        break;
                    }
                } else {
                    double value = 0;
                    if(record->items[column1]->kind == 0){
                        value = (double)((Int_Item*)record->items[column1])->integer;
                    } else if(record->items[column1]->kind == 1){
                        value = (double)((Float_Item*)record->items[column1])->floating;
                    }

                    if(!((NumListCondition*)conds[k])->compare(value)){
                        return false;
                    } else {
                        break;
                    }
                }
            }

            case Condition::IS_NULL:
            case Condition::IS_NOT_NULL:
            {
                if(!((NullCondition*)conds[k])->compare(record->bitmap)){
                    return false;
                } else {
                    break;
                }
            }

            case Condition::EQS:
            case Condition::NES:
            {
                // test if null
                if(record->bitmap[column1] == 0){
                    return false;
                }

                string value = ((Char_Item*)record->items[column1])->str;
                // cout<<"value: "<<value<<endl;
                // cout<<"bound: "<<((StringCompareCondition*)conds[k])->bound<<endl;
                // cout<<"equal: "<<(value == ((StringCompareCondition*)conds[k])->bound)<<endl; 
                if(!((StringCompareCondition*)conds[k])->compare(value)){
                    return false;
                } else {
                    break;
                }
            }

            case Condition::EQ2:
            case Condition::NE2:
            case Condition::LT2:
            case Condition::LE2:
            case Condition::GT2:
            case Condition::GE2:
            {
                // test if null
                if(record->bitmap[column1] == 0){
                    return false;
                }

                unsigned int column2 = ((CompareTwoColumnCondition*)conds[k])->column2;

                // test if null
                if(record->bitmap[column2] == 0){
                    return false;
                }
            
                double value1 = 0;
                double value2 = 0;
                if(record->items[column1]->kind == 0){
                    value1 = (double)((Int_Item*)record->items[column1])->integer;
                } else if(record->items[column1]->kind == 1){
                    value1 = (double)((Float_Item*)record->items[column1])->floating;
                }

                if(record->items[column2]->kind == 0){
                    value2 = (double)((Int_Item*)record->items[column2])->integer;
                } else if(record->items[column2]->kind == 1){
                    value2 = (double)((Float_Item*)record->items[column2])->floating;
                }

                if(!((CompareTwoColumnCondition*)conds[k])->compare(value1, value2)){
                    return false;
                } else {
                    break;
                }
            }

            case Condition::EQS2:
            case Condition::NES2:
            {
                // test if null
                if(record->bitmap[column1] == 0){
                    return false;
                }

                unsigned int column2 = ((CompareTwoStringCondition*)conds[k])->column2;

                // test if null
                if(record->bitmap[column2] == 0){
                    return false;
                }

                string value1 = ((Char_Item*)record->items[column1])->str;
                string value2 = ((Char_Item*)record->items[column2])->str;
                if(!((CompareTwoStringCondition*)conds[k])->compare(value1, value2)){
                    return false;
                } else {
                    break;
                }
            }

            default:
            {
                return false;
            }
        }
    }
    return true;
}

std::vector<Record*> RecordManager::selectFirstRecordsBlock(const int fileID, const vector<Condition*>& conds, Record* example, unsigned int block_size){
    std::vector<Record*> result;
    unsigned int selected_count = 0;

    int index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, index);
    meta_data meta = getMetaData(fileID); 
    unsigned int page_count = meta.page_count;
    unsigned int slot_count = meta.slot_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;

    // iterate all pages
    for (int i = 1; i <= page_count; i++) {
        BufType page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                // set the RID
                record->rid = RID(i, j);
                
                // compare column 
                bool match = matchRecord(record, conds);

                if (match) {
                    result.push_back(record);
                    selected_count++;
                    if(selected_count == block_size)
                        return result;
                } else {                // remove this record
                    delete record;
                }
            }
            start_offset += bytes;
        }
    }
    return result;
}

std::vector<Record*> RecordManager::selectNextRecordsBlock(const int fileID, const vector<Condition*>& conds, Record* example, unsigned int block_size, RID& rid){
    std::vector<Record*> result;
    unsigned int selected_count = 0;

    unsigned int pageID = rid.pageIdx;
    unsigned int slotID = rid.slotIdx;

    meta_data meta = getMetaData(fileID); 
    unsigned int page_count = meta.page_count;
    unsigned int slot_count = meta.slot_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;

    // iterate the other slots in this page
    unsigned int start;
    if(slotID >= slot_count - 1){
        if(pageID == page_count){ // no more page
            return result;
        } else {
            pageID++;
            start = 0;
        }
    } else {
        start = slotID + 1;
    }

    // this page
    int index;
    BufType page = this->bufPageManager->getPage(fileID, pageID, index);

    // iterate all the slots after the given one
    char* start_offset = ((char*)page) + slots_offset + start * bytes;
    char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
    for (int j = start; j < slot_count; j++) {
        if (getSlotBitMap(bitmap, j)) {     // not empty
            Record* record = emptyCopy(example);
            Serializer::deserialize_record(start_offset, record);
            record->rid = RID(pageID, j);
            
            // compare column 
            bool match = matchRecord(record, conds);

            if (match) {
                result.push_back(record);
                selected_count++;
                if(selected_count == block_size){
                    return result;
                }
            } else {                // remove this record
                delete record;
            }

        }
        start_offset += bytes;
    }

    // iterate all pages
    for (int i = pageID + 1; i <= page_count; i++) {
        BufType page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                // set the RID
                record->rid = RID(i, j);
                
                // compare column 
                bool match = matchRecord(record, conds);

                if (match) {
                    result.push_back(record);
                    selected_count++;
                    if(selected_count == block_size)
                        return result;
                } else {                // remove this record
                    delete record;
                }
            }
            start_offset += bytes;
        }
    }
    return result;
}

std::vector<Record*> RecordManager::selectRecordsBlock(const int fileID, const vector<Condition*>& conds, Record* example, unsigned int limit=0, unsigned int offset=0){
    std::vector<Record*> result;
    if(limit == 0)
        return result;
        
    int index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, index);
    meta_data meta = getMetaData(fileID); 
    unsigned int page_count = meta.page_count;
    unsigned int slot_count = meta.slot_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;

    // iterate all pages
    for (int i = 1; i <= page_count; i++) {
        BufType page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                // set the RID
                record->rid = RID(i, j);

                // cout<<"get record"<<endl;
                // record->print();
                
                // compare column 
                bool match = matchRecord(record, conds);

                if (match) {
                    if(offset > 0){
                        offset--;
                        delete record;
                    } else {
                        result.push_back(record);
                        if(limit != -1 && result.size() == limit)
                            return result;
                    }
                } else {                // remove this record
                    delete record;
                }
            }
            start_offset += bytes;
        }
    }
    return result;
}

/* single record select */ 
Record* RecordManager::selectRecordAluCompare(const unsigned int fileID, const CompareCondition* condition, Record* example, const RID& rid){
    unsigned int pageID = rid.pageIdx;
    unsigned int slotID = rid.slotIdx;
    int index;
    BufType page = this->bufPageManager->getPage(fileID, pageID, index);

    // read the record from the slot
    meta_data meta = getMetaData(fileID);
    unsigned int page_count = meta.page_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;
    unsigned int slot_count = meta.slot_count;

    unsigned int start;
    if(slotID >= slot_count - 1){
        if(pageID == page_count){ // no more page
            return nullptr;
        } else {
            pageID++;
            page = this->bufPageManager->getPage(fileID, pageID, index);
            start = 0;
        }
    } else {
        start = slotID + 1;
    }

    // iterate all the slots after the given one
    char* start_offset = ((char*)page) + slots_offset + start * bytes;
    char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
    for (int j = start; j < slot_count; j++) {
        if (getSlotBitMap(bitmap, j)) {     // not empty
            Record* record = emptyCopy(example);
            Serializer::deserialize_record(start_offset, record);
            record->rid = RID(pageID, j);

            // compare column 
            unsigned int column = condition->column;
            
            double value = 0;
            if(record->items[column]->kind == 0){
                value = (double)((Int_Item*)record->items[column])->integer;
            } else if(record->items[column]->kind == 1){
                value = (double)((Float_Item*)record->items[column])->floating;
            } else {
                return nullptr;      // error if the column is not int or float
            }

            if (condition->compare(value)) {
                return record;
            } else {                // remove this record
                delete record;
            }
        }
        start_offset += bytes;
    }

    // if this page has no more record, iterate pages after this one
    for (int i = pageID + 1; i <= page_count; i++) {
        page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                record->rid = RID(i, j);

                // compare column 
                unsigned int column = condition->column;
                
                double value = 0;
                if(record->items[column]->kind == 0){
                    value = (double)((Int_Item*)record->items[column])->integer;
                } else if(record->items[column]->kind == 1){
                    value = (double)((Float_Item*)record->items[column])->floating;
                } else {
                    return nullptr;      // error if the column is not int or float
                }

                if (condition->compare(value)) {
                    return record;
                } else {                // remove this record
                    delete record;
                }
            }
            start_offset += bytes;
        }
    }
    return nullptr;     // no record found
}


Record* RecordManager::selectRecord(const int fileID, const Condition* condition, Record* example, const RID& rid)
{
    switch (condition->kind)
    {
        case Condition::EQ:
        case Condition::NE:
        case Condition::LT:
        case Condition::LE:
        case Condition::GT:
        case Condition::GE:
            return selectRecordAluCompare(fileID, (CompareCondition*)condition, example, rid);
            break;
        
        default:
            return nullptr;
            break;
    }
}

Record* RecordManager::selectFirstRecord(const int fileID, Record* example){
    unsigned int pageID = 1;
    unsigned int slotID = 0;
    int index;
    BufType page = this->bufPageManager->getPage(fileID, pageID, index);

    // read the record from the slot
    meta_data meta = getMetaData(fileID);
    unsigned int page_count = meta.page_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;
    unsigned int slot_count = meta.slot_count;

    // iterate all the slots after the given one
    unsigned int start = 0;
    char* start_offset = ((char*)page) + slots_offset + start * bytes;
    char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
    for (int j = start; j < slot_count; j++) {
        if (getSlotBitMap(bitmap, j)) {     // not empty
            Record* record = emptyCopy(example);
            Serializer::deserialize_record(start_offset, record);
            record->rid = RID(pageID, j);
            return record;
        }
        start_offset += bytes;
    }

    // if this page has no more record, iterate pages after this one
    for (int i = pageID + 1; i <= page_count; i++) {
        page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                record->rid = RID(i, j);

                return record;
            }
            start_offset += bytes;
        }
    }
    return nullptr;     // no record found
}

Record* RecordManager::selectNextRecord(const int fileID, Record* example, const RID& rid){
    // cout<<"selectNextRecord"<<endl;
    unsigned int pageID = rid.pageIdx;
    unsigned int slotID = rid.slotIdx;
    // cout<<"pageID: "<<pageID<<endl;
    // cout<<"slotID: "<<slotID<<endl;
    int index;
    BufType page = this->bufPageManager->getPage(fileID, pageID, index);

    // read the record from the slot
    meta_data meta = getMetaData(fileID);
    unsigned int page_count = meta.page_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;
    unsigned int slot_count = meta.slot_count;

    unsigned int start;
    if(slotID >= slot_count - 1){
        if(pageID == page_count){ // no more page
            return nullptr;
        } else {
            pageID++;
            page = this->bufPageManager->getPage(fileID, pageID, index);
            start = 0;
        }
    } else {
        start = slotID + 1;
    }

    // iterate all the slots after the given one
    char* start_offset = ((char*)page) + slots_offset + start * bytes;
    char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
    for (int j = start; j < slot_count; j++) {
        if (getSlotBitMap(bitmap, j)) {     // not empty
            Record* record = emptyCopy(example);
            Serializer::deserialize_record(start_offset, record);
            record->rid = RID(pageID, j);
            // cout<<"get "<<record->rid.pageIdx<<" "<<record->rid.slotIdx<<endl;
            // cout<<"offset "<<slots_offset + j * bytes<<endl;
            return record;
        }
        start_offset += bytes;
    }

    // if this page has no more record, iterate pages after this one
    for (int i = pageID + 1; i <= page_count; i++) {
        page = this->bufPageManager->getPage(fileID, i, index);
        char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
        char* start_offset = ((char*)page) + slots_offset;
        for (int j = 0; j < slot_count; j++) {
            if (getSlotBitMap(bitmap, j)) {     // not empty
                Record* record = emptyCopy(example);
                Serializer::deserialize_record(start_offset, record);
                record->rid = RID(i, j);

                return record;
            }
            start_offset += bytes;
        }
    }
    return nullptr;     // no record found
}


// debug tools 
void RecordManager::printFile(const int fileID, Record* example, bool detail){
    cout<<endl<<"File ID: "<<fileID<<endl<<"-------------"<<endl;
    int meta_index;
    BufType meta_page = this->bufPageManager->getPage(fileID, 0, meta_index);
    meta_data meta = getMetaData(fileID); 
    meta.print();
    cout<<"Records: "<<endl<<"=================="<<endl;
    unsigned int page_count = meta.page_count;
    unsigned int slot_count = meta.slot_count;
    unsigned int bytes = meta.record_bytes;
    unsigned int slots_offset = meta.first_slot_offset;

    // iterate all pages
    int index;
    for (int i = 1; i < page_count; i++) {
        Record* record = emptyCopy(example);
        this->printPage(fileID, i, record, detail);
        delete record;
    }
}

void RecordManager::printPage(const int fileID, const int i, Record* example, bool detail){
    int index;
    BufType page = this->bufPageManager->getPage(fileID, i, index);
    cout<<"Page: "<<i<<endl<<"-------------"<<endl;
    char* bitmap = (char*)(page + PAGE_HEADER_SIZE);
    unsigned int slot_count = page[1];
    unsigned int slots_offset = PAGE_HEADER_SIZE * 4 + PAGE_SLOT_BITMAP_SIZE * 4;
    unsigned int bytes = page[3];
    printSlotBitMap(bitmap, slot_count);
    cout<<"-------------"<<endl;
    if(!detail)
        return;
    
    // detail of records
    char* start_offset = ((char*)page) + slots_offset;
    for (int j = 0; j < slot_count; j++) {
        if (getSlotBitMap(bitmap, j)) {     // not empty
            Serializer::deserialize_record(start_offset, example);
            // set the RID
            example->rid = RID(i, j);
            cout<<"Record at slot "<<j<<": "<<endl;
            example->print();
        }
        start_offset += bytes;
    }
}
    