#ifndef INDEX_H
#define INDEX_H

#include <vector>
#include "Key.h"
#include "../../record_management/Rid.h"

using namespace std;

// One index item in the index page

class Index {
public:
    Key key;                        // key of the index
    RID rid;                        // if not leaf, slotIdx is 0, pageIdx point to the child; 
                                    // or pageIdx, slotIdx point to the record in record file
    Index(Key key, RID rid):key(key), rid(rid){}
    static Index getLeafIndex(Key key, RID rid){
        return Index(key, rid);
    }
    static Index getInnerIndex(Key key, int pageIdx){
        return Index(key, RID(pageIdx, 0));
    }

    void print(){
        cout << "key: ";
        key.print();
        cout << " rid: ";
        rid.print();
        cout<<endl;
    }
};

#endif