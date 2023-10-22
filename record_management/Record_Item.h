//
// Created by Xu kangping on 2022/11/19.
//

#ifndef PROJECT_RECORD_ITEM_H
#define PROJECT_RECORD_ITEM_H


#include <cstring>
#include <iostream>
using namespace std;

class Record_Item {
public:
    unsigned short kind;
    void print(){
        cout<<"not implemented error";
    }
    ~Record_Item(){
        // cout<<"free record item"<<endl;
    }
};

class Int_Item : public Record_Item {
public:
    int integer;

    Int_Item(int integer=0) {
        this->kind = 0;
        this->integer = integer;
    }
    void print(){
        cout<<"int:   "<<this->integer;
    }
};

class Float_Item : public Record_Item {
public:
    float floating;

    Float_Item(float floating=0) {
        this->kind = 1;
        this->floating = floating;
    }

    void print(){
        cout<<"float: "<<this->floating;
    }
};

class Char_Item : public Record_Item {
public:
    char* str;
    unsigned int len;

    Char_Item(const char* str, unsigned int len) {
        this->kind = 2;
        this->len = len;
        this->str = new char[len];
        memset(this->str, 0, len);      // reset
        memcpy(this->str, str, strlen(str) < len ? strlen(str):len);
    }

    void print(){
        cout<<"char:  "<<string(this->str, this->len);
        cout<<"  len: "<<this->len;
    }

    ~Char_Item(){
        if(this->str){
            // cout<<"free item: "<<string(this->str, this->len)<<endl;
            delete[] this->str;
        }
    }
};
#endif //PROJECT_RECORD_ITEM_H
