/*
    class of the key of the B+ tree
    int
*/

#ifndef KEY_H
#define KEY_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

#define MAX_DIM 2

class Key{
public:
    unsigned int dims;
    std::vector<int> vals;
    Key(std::vector<int> vals):vals(vals){
        if(vals.size()>MAX_DIM){
            cout<<"Error: dimension of key is larger than MAX_DIM"<<endl;
            exit(1);
        }
        dims = vals.size();
    }
    Key(int val=0):vals(1,val){
        dims = 1;
    }
    unsigned int size() const {
        return dims;
    }
    bool operator<(const Key& key) const{
        for(int i=0;i<dims;i++){
            if(vals[i]<key.vals[i]) return true;
            else if(vals[i]>key.vals[i]) return false;
        }
        return false;
    }
    bool operator<=(const Key& key) const{
        for(int i=0;i<dims;i++){
            if(vals[i]<key.vals[i]) return true;
            else if(vals[i]>key.vals[i]) return false;
        }
        return true;
    }
    bool operator==(const Key& key) const{
        for(int i=0;i<dims;i++){
            if(vals[i]!=key.vals[i]) return false;
        }
        return true;
    }
    
    void print(){
        cout<<"(";
        for(int i=0;i<dims;i++){
            cout<<vals[i];
            if(i!=dims-1) cout<<",";
        }
        cout<<")";
    }
};

#endif