/*
    mapping of all the indexes in a database
*/

#ifndef INDEX_MAP_H
#define INDEX_MAP_H

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
using namespace std;

#define NORMAL_INDEX 0
#define PRIMARY_INDEX 1
#define UNIQUE_INDEX 2

class IndexMeta{
public:
    string index_name;
    unsigned int table_idx;
    vector<unsigned int> column_idx;
    unsigned int index_type;
    IndexMeta(string index_name, unsigned int table_idx, vector<unsigned int> column_idx, unsigned int index_type):
        index_name(index_name), table_idx(table_idx), column_idx(column_idx), index_type(index_type){
            if(column_idx.size() > 2){
                cout<<"Error: too many columns in index"<<endl;
            }
        }
    
    IndexMeta(){}
    void serialize(char* buf){
        int offset = 0;
        // index_name
        int len = index_name.length();
        memset(buf + offset, 0, 128);
        memcpy(buf + offset, index_name.c_str(), len);
        offset += 128;
        // table_idx
        memcpy(buf + offset, &table_idx, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        // index_type
        memcpy(buf + offset, &index_type, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        // column_num
        int column_num = column_idx.size();
        memcpy(buf + offset, &column_num, sizeof(int));
        offset += sizeof(int);
        // column ids
        for(int i = 0; i < column_num; i++){
            memcpy(buf + offset, &column_idx[i], sizeof(unsigned int));
            offset += sizeof(unsigned int);
        }
    }
    bool deserialize(char* buf){
        int offset = 0;
        // index_name
        this->index_name = buf + offset;
        offset += 128;
        // table_idx
        memcpy(&this->table_idx, buf + offset, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        // index_type
        memcpy(&this->index_type, buf + offset, sizeof(unsigned int));
        offset += sizeof(unsigned int);
        // column_num
        int column_num;
        memcpy(&column_num, buf + offset, sizeof(int));
        offset += sizeof(int);
        if(column_num > 2){
            cout<<"Error: too many columns in index"<<endl;
            return false;
        }
        // column ids
        column_idx = vector<unsigned int>();
        for(int i = 0; i < column_num; i++){
            unsigned int id;
            memcpy(&id, buf + offset, sizeof(unsigned int));
            offset += sizeof(unsigned int);
            column_idx.push_back(id);
        }
        return true;
    }
};

class IndexMap{
public:
    vector<IndexMeta> index_map;
    IndexMap(){}
    IndexMeta getIndexMeta(string index_name){
        for(int i = 0; i < index_map.size(); i++){
            if(index_map[i].index_name == index_name){
                return index_map[i];
            }
        }
        cout<<"Error: index not found"<<endl;
        return IndexMeta();
    }
    int getIndexId(string index_name){
        for(int i = 0; i < index_map.size(); i++){
            if(index_map[i].index_name == index_name){
                return i;
            }
        }
        cout<<"Error: index not found"<<endl;
        return -1;
    }
    IndexMeta getIndexMeta(unsigned int id){
        if(id >= index_map.size()){
            cout<<"Error: index not found"<<endl;
            return IndexMeta();
        }
        return index_map[id];
    }

    // get the index with the given columns in the same table
    // return -1 if not found
    int getSameColumns(vector<unsigned int> columns, unsigned int table_idx){
        for(int i = 0; i < index_map.size(); i++){
            if(index_map[i].table_idx == table_idx && index_map[i].column_idx.size() == columns.size()){
                bool same = true;
                for(int j = 0; j < columns.size(); j++){
                    if(index_map[i].column_idx[j] != columns[j]){
                        same = false;
                        break;
                    }
                }
                if(same){
                    return i;
                }
            }
        }
        return -1;
    }

    bool hasIndexMeta(string index_name){
        for(int i = 0; i < index_map.size(); i++){
            if(index_map[i].index_name == index_name){
                return true;
            }
        }
        return false;
    }
    bool hasIndexMeta(unsigned int id){
        if(id >= index_map.size()){
            return false;
        }
        return true;
    }
    vector<string> deleteIndexes(unsigned int table_idx){
        int index_num = index_map.size();
        vector<string> delete_names;
        // cout<<"delete "<<table_idx<<endl;
        for(int i = 0; i < index_num; i++){
            // cout<<index_map[i].index_name<<endl;
            // cout<<index_map[i].table_idx<<endl;
            if(index_map[i].table_idx == table_idx){
                delete_names.push_back(index_map[i].index_name);
                index_map.erase(index_map.begin() + i);
                i--;
                index_num--;
            }
        }
        return delete_names;
    }
    vector<string> getIndexNames(unsigned int table_idx){
        vector<string> names;
        for(int i = 0; i < index_map.size(); i++){
            if(index_map[i].table_idx == table_idx){
                names.push_back(index_map[i].index_name);
            }
        }
        return names;
    }
    vector<string> getIndexFiles(unsigned int table_idx){
        vector<string> names;
        for(int i = 0; i < index_map.size(); i++){
            if(index_map[i].table_idx == table_idx){
                names.push_back(index_map[i].index_name + "_idx");
            }
        }
        return names;
    }
    vector<IndexMeta> getIndexMetas(unsigned int table_idx){
        vector<IndexMeta> metas;
        for(int i = 0; i < index_map.size(); i++){
            if(index_map[i].table_idx == table_idx){
                metas.push_back(index_map[i]);
            }
        }
        return metas;
    }

    bool deleteIndex(string index_name){
        int index_num = index_map.size();
        for(int i = 0; i < index_num; i++){
            if(index_map[i].index_name == index_name){
                index_map.erase(index_map.begin() + i);
                return true;
            }
        }
        cout<<"Error: index not found"<<endl;
        return false;
    }
    bool deleteIndex(unsigned int id){
        if(id >= index_map.size()){
            cout<<"Error: index not found"<<endl;
            return false;
        }
        index_map.erase(index_map.begin() + id);
        return true;
    }
    bool addIndex(IndexMeta index){
        index_map.push_back(index);
        return true;
    }
    bool addIndex(string index_name, unsigned int table_idx, vector<unsigned int> column_idx, unsigned int index_type){
        index_map.push_back(IndexMeta(index_name, table_idx, column_idx, index_type));
        return true;
    }
    string getNewDefaultName(){
        string defaultName = "index" + to_string(index_map.size());
        while(hasIndexMeta(defaultName)){
            defaultName = defaultName + "_1";
        }
        return defaultName;
    }
    void serialize(char* buf){
        unsigned int INDEX_LEN = 148;
        int offset = 0;
        int index_num = index_map.size();
        memcpy(buf + offset, &index_num, sizeof(int));
        offset += sizeof(int);
        for(int i = 0; i < index_num; i++){
            index_map[i].serialize(buf + offset);
            offset += INDEX_LEN;
        }
    }
    bool deserialize(char* buf){
        unsigned int INDEX_LEN = 148;
        int offset = 0;
        int index_num;
        memcpy(&index_num, buf + offset, sizeof(int));
        offset += sizeof(int);
        if(index_num > 32){
            cout<<"Error: too many indexes in this database"<<endl;
            return false;
        }
        index_map = vector<IndexMeta>();
        for(int i = 0; i < index_num; i++){
            IndexMeta index;
            if(!index.deserialize(buf + offset)){
                return false;
            }
            index_map.push_back(index);
            offset += INDEX_LEN;
        }
        return true;
    }
};
#endif