/*
    Tree node of B+ tree based on file system
*/

#ifndef NODE_H
#define NODE_H
#include <list>
#include <cstring>
#include "Index.h"
using namespace std;

#define HEADER_SIZE 32      // space for page header
#define PAGE_SIZE 8192      // page size: 8KB
#define INDEX_SIZE 16       // size of one index
#define MAX_M 500   // max number of indexes in one page

class Node {
public:
    list<Index> indexes;            // indexes in the node
    unsigned int parentId;          // parent node page id, 0 for non exist
    unsigned int nodeId;            // node page id
    unsigned int m;                 // highest dimension of the B+ tree
    bool isLeaf;                    // if leaf node
    bool isRoot;
    unsigned int succId;        // succ leaf node page id
    unsigned int prevId;        // previous leaf node page id

    Node(unsigned int nodeId, bool isLeaf, bool isRoot, unsigned int parentId, unsigned int succId, unsigned int prevId, list<Index> indexes=list<Index>(), unsigned int m=MAX_M):
        nodeId(nodeId), isLeaf(isLeaf), isRoot(isRoot), parentId(parentId), succId(succId), prevId(prevId), indexes(indexes), m(m){}

    Node(unsigned int m){
        this->m = m;
    }    // used for deserialize
    
    // insert a index into the right place,
    // if equal, insert to the first place
    bool insertIndex(Index index){
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(index.key<=it->key){
                indexes.insert(it, index);
                return true;
            }
        }
        indexes.push_back(index);
        return true;
    }

    // search for the first index not smaller than the key
    // if not found, return the first index that is larger than the key
    // if all smaller, assemble a index with pageIdx=0, slotIdx=0
    Index searchIndex(Key key){
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(key<=it->key){
                return *it;
            }
        }
        return Index(key, RID(0, 0));
    }

    // search all the keys that are equal to the key
    // return a list of indexes
    list<Index> searchIndexes(Key key){
        list<Index> res;
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(key==it->key){
                res.push_back(*it);
            }
        }
        return res;
    }

    // search for the indexes in the range [key1, key2)
    // return a list of indexes
    list<Index> searchRange(Key key1, Key key2){
        list<Index> res;
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(key1<=it->key && it->key<key2){
                res.push_back(*it);
            } else if (!(it->key < key2)) {
                break;
            }
        }
        return res;
    }

    // search for the index pointing to the page
    // if not found, return -1
    int searchPointer(unsigned int pageIdx){
        // only used by non leaf node
        if (isLeaf) return -1;

        int i=0;
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(pageIdx==it->rid.pageIdx){
                return i;
            }
            i++;
        }
        return -1;
    }

    // delete one index with the key
    // if not found, return false
    bool deleteIndex(Key key){
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(key==it->key){
                indexes.erase(it);
                return true;
            }
        }
        return false;
    }

    // delete the index of the given position
    // if not found, return false
    bool deleteIndex(int pos){
        int i=0;
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(i==pos){
                indexes.erase(it);
                return true;
            }
            i++;
        }
        return false;
    }

    // delete a index that is the same as the given one
    // if not found, return false
    // has is used to indicate if the key is found
    bool deleteIndex(Index index, bool& has){
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(it->key == index.key){
                has = true;
                if(it->rid.pageIdx == index.rid.pageIdx && it->rid.slotIdx == index.rid.slotIdx){
                    indexes.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

    // delete all the indexes with the key
    bool deleteIndexes(Key key){
        bool flag = false;
        for(auto it=indexes.begin();it!=indexes.end();){
            if(key==it->key){
                it = indexes.erase(it);
                flag = true;
            }
            else it++;
        }
        return flag;
    }

    // get the pointer to the child with pageIdx
    Index getPointer(unsigned int pageIdx){
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(pageIdx==it->rid.pageIdx){
                return *it;
            }
        }
        return indexes.front(); 
    }

    // get the position of the pointer to the child with pageIdx
    // if not found, return -1
    int getPointerPos(unsigned int pageIdx){
        int i=0;
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(pageIdx==it->rid.pageIdx){
                return i;
            }
            i++;
        }
        return -1;
    }

    // delete the pointer to the child page
    // if not found, return false
    bool deletePointer(unsigned int pageIdx){
        // can only be used by non leaf node
        if (isLeaf) return false;

        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(pageIdx==it->rid.pageIdx){
                indexes.erase(it);
                return true;
            }
        }
        return false;
    }

    // update the pointer with the pageIdx
    // if not found, return false
    bool updatePointer(unsigned int pageIdx, Key key){
        // can only be used by non leaf node
        if (isLeaf) return false;

        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(pageIdx==it->rid.pageIdx){
                it->key = key;
                return true;
            }
        }
        return false;
    }

    // divide the pointer: replace the pointer with the pageIdx with the two given pointers
    bool dividePointer(unsigned int pageIdx, Index index1, Index index2){
        // can only be used by non leaf node
        // cout<<"call divide pointer"<<endl;
        // cout<<"pageIdx: "<<pageIdx<<endl;
        // index1.print();
        // index2.print();
        // cout<<"old indexes: "<<endl;
        // for(auto it=indexes.begin();it!=indexes.end();it++){
        //     it->print();
        // }

        if (isLeaf) return false;

        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(pageIdx==it->rid.pageIdx){
                // return the next position
                it = indexes.erase(it);
                // insert the two indexes
                indexes.insert(it, index1);
                indexes.insert(it, index2);
                // cout<<"new indexes: "<<endl;
                // for(auto it=indexes.begin();it!=indexes.end();it++){
                //     it->print();
                // }
                return true;
            }
        }
        return false;
    }

    // get the index with the key
    // if not found, return false
    bool getIndex(Key key, Index& index){
        for(auto it=indexes.begin();it!=indexes.end();it++){
            if(key==it->key){
                index = *it;
                return true;
            }
        }
        return false;
    }

    // return the last index and delete it
    Index popLastIndex(){
        Index index = indexes.back();
        indexes.pop_back();
        return index;
    }

    // return the first index and delete it
    Index popFirstIndex(){
        Index index = indexes.front();
        indexes.pop_front();
        return index;
    }

    // merge the given node to the back
    // the given node should be the right node
    void mergeBack(Node* node){
        // indexes
        indexes.insert(indexes.end(), node->indexes.begin(), node->indexes.end());

        // share the same parentId, no update
        // update succId
        succId = node->succId;
    }

    // merge the given node to the front
    // the given node should be the left node
    void mergeFront(Node* node){
        // indexes
        indexes.insert(indexes.begin(), node->indexes.begin(), node->indexes.end());

        // share the same parentId, no update
        // update prevId
        prevId = node->prevId;
    }

    // getters
    unsigned int getParentId(){
        return parentId;
    }
    unsigned int getNodeId(){
        return nodeId;
    }
    bool getIsLeaf(){
        return isLeaf;
    }
    unsigned int getSuccLeafId(){
        return succId;
    }
    unsigned int getPrevLeafId(){
        return prevId;
    }
    Index getFirstIndex(){
        return indexes.front();
    }
    Index getLastIndex(){
        return indexes.back();
    }

    bool hasParent(){
        return parentId!=0;
    }
    bool hasSuccLeaf(){
        return succId!=0;
    }
    bool hasPrevLeaf(){
        return prevId!=0;
    }

    // check overflow or underflow
    // >=m , divide
    bool isOverflow(){
        // return indexes.size() >= m;
        return indexes.size() > m;
    }
    // < ceil(m/2)-1, merge
    bool isUnderflow(){
        // return indexes.size() < (m+1)/2-1;
        return indexes.size() < (m-1)/2;
    }
    // > ceil(m/2)-1, borrow
    bool isBorrowable(){
        // return indexes.size() > (m+1)/2-1;
        return indexes.size() > (m-1)/2;
    }

    // serialize
    void serialize(char* page){
        /*
            format:
            isLeaf (1 byte) | isRoot (1 byte) |
            parentId (4 bytes) | nodeId (4 bytes) | succId (4 bytes) | prevId (4 bytes) |
            key size (4 bytes ) |
            num of indexes (4 bytes) |
            indexes (each index is 16 bytes)        
        */

        // write header
        page[0] = isLeaf;
        page[1] = isRoot;
        memcpy(page+2, &parentId, 4);
        memcpy(page+6, &nodeId, 4);
        memcpy(page+10, &succId, 4);
        memcpy(page+14, &prevId, 4);
        unsigned int keySize = indexes.front().key.size();
        memcpy(page+18, &keySize, 4);
        unsigned int num = indexes.size();
        memcpy(page+22, &num, 4);

        // write indexes
        int offset = HEADER_SIZE;
        for(auto it=indexes.begin();it!=indexes.end();it++){
            memcpy(page + offset, &(it->rid.pageIdx), 4);
            memcpy(page + offset + 4, &(it->rid.slotIdx), 4);
            for(int i=0; i<it->key.size(); i++)
                memcpy(page + offset + 8 + 4 * i, &(it->key.vals[i]), 4);
            offset += INDEX_SIZE;
        }
    }

    // diserialize
    bool deserialize(char* page){
        /*
            format:
            isLeaf (1 byte) | isRoot (1 byte) |
            parentId (4 bytes) | nodeId (4 bytes) | succId (4 bytes) | prevId (4 bytes) |
            key size (4 bytes ) |
            num of indexes (4 bytes) |
            indexes (each index is 16 bytes)   
            index: rid (8 bytes) | key (4 bytes)     
        */

        // read header
        isLeaf = page[0];
        isRoot = page[1];
        memcpy(&parentId, page+2, 4);
        memcpy(&nodeId, page+6, 4);
        memcpy(&succId, page+10, 4);
        memcpy(&prevId, page+14, 4);
        unsigned int keySize;
        memcpy(&keySize, page+18, 4);
        unsigned int num;
        memcpy(&num, page+22, 4);

        // read indexes
        int offset = HEADER_SIZE;
        for(int i=0; i<num; i++){
            RID rid;
            memcpy(&(rid.pageIdx), page + offset, 4);
            memcpy(&(rid.slotIdx), page + offset + 4, 4);
            vector<int> vals;
            for(int j=0; j<keySize; j++)
            {
                int val;
                memcpy(&val, page + offset + 8 + 4 * j, 4);
                vals.push_back(val);
            }
            Key key(vals);
            indexes.push_back(Index(key, rid));
            offset += INDEX_SIZE;
        }
        return true;
    }

    // print the node
    void print(){
        cout<<endl<<"Node "<<nodeId<<": ";
        if(isLeaf) cout<<"leaf, ";
        else cout<<"non-leaf, ";
        if(isRoot) cout<<"root, ";
        else cout<<"non-root, ";

        cout<<endl<<"parent: "<<parentId<<", ";
        cout<<"succ: "<<succId<<", ";
        cout<<"prev: "<<prevId<<", ";

        cout<<" m: "<<m;
        cout<<endl<<"indexes: "<<endl;
        for(auto it=indexes.begin();it!=indexes.end();it++){
            it->print();
        }
        cout<<endl;
    }
};

#endif