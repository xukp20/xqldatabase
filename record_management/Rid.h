// RID of a record

#ifndef RID_H
#define RID_H

class RID {
public:
    unsigned int pageIdx;
    unsigned int slotIdx;

    RID(unsigned int pId=0, unsigned sId=0): pageIdx(pId), slotIdx(sId){}

    void print(){
        cout << "(" << pageIdx << ", " << slotIdx << ")";
    }
};

#endif