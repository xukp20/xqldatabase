//
// Created by Xu kangping on 2022/11/19.
//

#include "Record.h"

void Record::append(bool not_null, int integer) {
    this->bitmap.push_back(not_null);
    this->items.push_back(new Int_Item(integer));
}

void Record::append(bool not_null, float floating) {\
    this->bitmap.push_back(not_null);
    this->items.push_back(new Float_Item(floating));
}

void Record::append(bool not_null, const char* str, unsigned int len) {
    this->bitmap.push_back(not_null);
    this->items.push_back(new Char_Item(str, len));
}

Record::~Record(){
    // cout<<"free record"<<endl;
    for(int i=0; i<this->items.size(); i++)
    {
        switch (this->items[i]->kind)
        {
            case 0:
                delete (Int_Item*)this->items[i];
                break;
            case 1:
                delete (Float_Item*)this->items[i];
                break;
            case 2:
                delete (Char_Item*)this->items[i];
                break;
            default:
                break;
        }
    }
}

void Record::setRID(const unsigned int pID, const unsigned int sID) {
    this->rid.pageIdx = pID;
    this->rid.slotIdx = sID;
}

#include<iostream>
void Record::print(){
    cout<<"columns: "<<endl;
    for (int i = 0; i < this->bitmap.size(); i++){
        cout<<(this->bitmap[i]?"    not null     ":"    null     ");
        switch (this->items[i]->kind)
        {
            case 0:
                ((Int_Item*)(this->items[i]))->print();
                break;

            case 1:
                ((Float_Item*)(this->items[i]))->print();
                break;
            
            case 2:
                ((Char_Item*)(this->items[i]))->print();
                break;
            
            default:
                break;
        }
        cout<<endl;
    }
    cout<<"RID: "<<this->rid.pageIdx<<" "<<this->rid.slotIdx<<endl;
}