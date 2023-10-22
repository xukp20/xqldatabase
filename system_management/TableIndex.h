#ifndef TABLE_INDEX_H
#define TABLE_INDEX_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
#define MAX_TABLE_NUM 32


class TableIndex{
public:
    // map from index to table name
    map<unsigned int, string> indexToTable;
    bool table_exist[MAX_TABLE_NUM];
    unsigned int table_num;

    TableIndex(): table_num(0){
        for(int i = 0; i < MAX_TABLE_NUM; i++){
            table_exist[i] = false;
        }
    }
    void init(){
        indexToTable.clear();
        for(int i = 0; i < MAX_TABLE_NUM; i++){
            table_exist[i] = false;
        }
        table_num = 0;
    }
    void print(){
        cout<<"index to table: "<<endl;
        for(auto it = indexToTable.begin(); it != indexToTable.end(); it++){
            cout<<(unsigned int)it->first<<" "<<it->second<<endl;
        }
    }
    unsigned int nextIndex(){
        for(int i = 0; i < MAX_TABLE_NUM; i++){
            if(!table_exist[i]){
                return i + 1;   // from 1 - 32
            }
        }
        return 0;
    }
    bool append(unsigned int index, string tableName){
        if(indexToTable.find(index) != indexToTable.end()){
            cout<<"index already exists"<<endl;
            return false;
        }
        // check duplicate table name
        for(auto it = indexToTable.begin(); it != indexToTable.end(); it++){
            if(it->second == tableName){
                cout<<"table name already exists"<<endl;
                return false;
            }
        }
        if(table_num >= MAX_TABLE_NUM){
            cout<<"too many tables in this database"<<endl;
            return false;
        }
        indexToTable[index] = tableName;
        table_num++;
        table_exist[index - 1] = true;
        return true;
    }
    bool deleteIndex(unsigned int index){
        if(indexToTable.find(index) == indexToTable.end()){
            cout<<"index does not exist"<<endl;
            return false;
        }
        indexToTable.erase(index);
        table_num--;
        table_exist[index - 1] = false;
        return true;
    }
    unsigned int getIndex(string tableName){
        for(auto it = indexToTable.begin(); it != indexToTable.end(); it++){
            if(it->second == tableName){
                return it->first;
            }
        }
        return 0;
    }
    bool hasTable(string tableName){
        for(auto it = indexToTable.begin(); it != indexToTable.end(); it++){
            if(it->second == tableName){
                return true;
            }
        }
        return false;
    }
};

#endif