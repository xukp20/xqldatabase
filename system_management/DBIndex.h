#ifndef DB_INDEX_H
#define DB_INDEX_H

// store the mapping from db index to db name
#include <map>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
#define MAX_DB_NUM 8


class DBIndex{
public:
    // map from index to table name
    map<unsigned char, string> indexToDB;
    bool db_exist[MAX_DB_NUM];
    unsigned char db_num;

    DBIndex(): db_num(0){
        for(int i = 0; i < MAX_DB_NUM; i++){
            db_exist[i] = false;
        }
    }
    void init(){
        indexToDB.clear();
        for(int i = 0; i < MAX_DB_NUM; i++){
            db_exist[i] = false;
        }
        db_num = 0;
    }
    void print(){
        cout<<"index to database: "<<endl;
        for(auto it = indexToDB.begin(); it != indexToDB.end(); it++){
            cout<<(unsigned int)it->first<<" "<<it->second<<endl;
        }
    }
    unsigned char nextIndex(){
        for(int i = 0; i < MAX_DB_NUM; i++){
            if(!db_exist[i]){
                return i + 1;   // from 1 - 8
            }
        }
        return 0;
    }
    bool append(unsigned char index, string dbName){
        if(this->indexToDB.find(index) != indexToDB.end()){
            cout<<"index already exists"<<endl;
            return false;
        }
        // check duplicate db name
        for(auto it = indexToDB.begin(); it != indexToDB.end(); it++){
            if(it->second == dbName){
                cout<<"db name already exists"<<endl;
                return false;
            }
        }

        if(db_num >= MAX_DB_NUM){
            cout<<"too many databases"<<endl;
            return false;
        }
        indexToDB[index] = dbName;
        db_num++;
        db_exist[index - 1] = true;
        return true;
    }
    bool deleteIndex(unsigned char index){
        if(this->indexToDB.find(index) == indexToDB.end()){
            cout<<"index not exists"<<endl;
            return false;
        }
        indexToDB.erase(index);
        db_num--;
        db_exist[index - 1] = false;
        return true;
    }
    unsigned char getIndex(string dbName){
        for(auto it = indexToDB.begin(); it != indexToDB.end(); it++){
            if(it->second == dbName){
                return it->first;
            }
        }
        return 0;
    }
    bool hasDB(string dbName){
        for(auto it = indexToDB.begin(); it != indexToDB.end(); it++){
            if(it->second == dbName){
                return true;
            }
        }
        return false;
    }
};


#endif