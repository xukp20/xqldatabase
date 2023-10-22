#include "Tree.h"
#define DEBUG1 1
#define BITMAP_SIZE PAGE_SIZE * 8
// #define DEBUG true

Tree::Tree(string fileName, FileManager* fm, BufPageManager* bpm){
    rootId = 0;
    fileId = -1;
    m = MAX_M;
    this->fileName = fileName;
    this->fm = fm;
    this->bpm = bpm;
    this->bitmap = vector<bool>();

    if (fm == nullptr || bpm == nullptr){
        cout << "Error: file manager or buffer manager is null" << endl;
        return;
    }
} 

Tree::~Tree(){
    // write the new meta data to the meta pages
    serialize();

    // write back the dirty pages
    bpm->close();

    // close the file
    fm->closeFile(fileId);
}

bool Tree::initTree(){
    // open the file
    bool result = fm->openFile(fileName.c_str(), this->fileId);
    if(!result){
        cout << "Error: cannot open file " << fileName << endl;
        return false;
    }

    // diserialize the tree from the file
    return deserialize();
}

bool Tree::saveTree(){
    // cout<<"before save"<<endl;
    print();
    // write the new meta data to the meta pages
    serialize();

    // write back the dirty pages
    bpm->close();

    // close the file
    fm->closeFile(fileId);
    return true;
}

bool Tree::createTree(string fileName, FileManager* fm, BufPageManager* bpm, unsigned int m){
    // check if the file exists
    struct stat buffer;
    if (stat (fileName.c_str(), &buffer) == 0){
        cout << "Error: file " << fileName << " already exists" << endl;
        return false;
    }
    
    // create a new file
    if(!fm->createFile(fileName.c_str())){
        cout << "Error: cannot create file " << fileName << endl;
        return false;
    }

    // init meta data
    int fileID = 0;
    bool result = fm->openFile(fileName.c_str(), fileID);
    if(!result){
        cout << "Error: cannot open file " << fileName << endl;
        return false;
    }

    int index = 0;
    BufType meta_page = bpm->allocPage(fileID, 0, index, false);
    meta_page[0] = 2;       // page 2 is the root
    meta_page[1] = m;
    meta_page[2] = BITMAP_SIZE;
    bpm->markDirty(index);

    // init the bitmap
    BufType bitmap_page = bpm->allocPage(fileID, 1, index, false);
    memset(bitmap_page, 0, PAGE_SIZE);
    // set the first 3 bits to 1
    char* byte_page = (char*)bitmap_page;
    byte_page[0] = 0xE0;
    // cout<<"byte_page[0] = hex "<<hex<<(int)byte_page[0]<<endl;
    bpm->markDirty(index);

    // init the root
    BufType root_page = bpm->allocPage(fileID, 2, index, false);
    Node* root = new Node(2, true, true, 0, 0, 0, list<Index>(), MAX_M);
    root->serialize((char*)root_page);
    bpm->markDirty(index);

    // write back the dirty pages
    bpm->close();

    // close the file
    fm->closeFile(fileID);

    return true;
}

void Tree::serialize(){
    // write the meta data to the meta pages
    int index = 0;
    BufType meta_page = bpm->getPage(fileId, 0, index);
    meta_page[0] = rootId;
    meta_page[1] = m;
    meta_page[2] = bitmap.size();
    bpm->markDirty(index);
    bpm->writeBack(index);

    // write the bitmap to the second page
    // one bit for one node in the bitmap
    BufType bitmap_page = bpm->getPage(fileId, 1, index);
    char* byte_page = (char*)bitmap_page;
    for (int i = 0; i < bitmap.size(); i++){
        if (bitmap[i]){
            byte_page[i / 8] |= (1 << (7 - i % 8));
        }
        else{
            byte_page[i / 8] &= ~(1 << (7- i % 8));
        }
    }
    // cout<<"serialize byte_page"<<endl;
    // for(int i=0;i<8;i++){
    //     cout<<hex<<(int)byte_page[i]<<" ";
    // }
    // cout<<endl;
    // cout<<"bitmap"<<endl;
    // for(int i=0;i<8;i++){
    //     cout<<bitmap[i]<<" ";
    // }
    bpm->markDirty(index);
    bpm->writeBack(index);
}

bool Tree::deserialize(){
    if(fileId == -1){
        cout << "Error: file is not opened" << endl;
        return false;
    }

    // read the meta data from the meta pages
    int index = 0;
    BufType meta_page = bpm->getPage(fileId, 0, index);
    rootId = meta_page[0];
    m = meta_page[1];
    int bitmap_size = meta_page[2];
    // cout<<dec<<"rootId = "<<rootId<<endl;
    // cout<<"m = "<<m<<endl;
    // cout<<"bitmap_size = "<<bitmap_size<<endl;
    if (bitmap_size != BITMAP_SIZE){
        cout << "Error: bitmap size is not correct" << endl;
        return false;
    }
    bpm->access(index);

    // read the bitmap from the second page
    // one bit for one node in the bitmap
    BufType bitmap_page = bpm->getPage(fileId, 1, index);
    char* byte_page = (char*)bitmap_page;
    // cout<<"deserialize byte_page"<<endl;
    // for (int i = 0; i < 8; i++){
    //     cout<<hex<<(int)byte_page[i]<<" ";
    // }
    for (int i = 0; i < bitmap_size; i++){
        if (byte_page[i / 8] & (1 << (7 - i % 8))){
            bitmap.push_back(true);
        }
        else{
            bitmap.push_back(false);
        }
    }

    // cout<<"bitmap"<<endl;
    // for (int i = 0; i < 8; i++){
    //     cout<<bitmap[i]<<" ";
    // }
    bpm->access(index);

    return true;
}


/* tree interfaces */ 
// insert

bool Tree::insertIndex(Index index){
    // find the position 
    Node* node = readNode(rootId);
    // node->print();
    Key key = index.key;

    // search through the non-leaf nodes
    while(!node->isLeaf){
        // search for the first child with its biggest key >= key
        // node->print();
        Index mid_index = node->searchIndex(key);
        if(mid_index.rid.pageIdx == 0 && mid_index.rid.slotIdx == 0){
            // the key is bigger than all the keys in the tree
            delete node;
            return insertToLastLeaf(index);
        } else {
            // get the child node
            delete node;
            node = readNode(mid_index.rid.pageIdx);
        }
    }

    // insert into the leaf node
#ifdef DEBUG
    if(index.rid.pageIdx == 11 && index.rid.slotIdx == 6){
        cout<<"insert into the leaf node"<<endl;
        cout<<node->nodeId<<endl;
    }
#endif
    return insertToNode(index, node);
}

bool Tree::insertToLastLeaf(Index index){
    // called when needs to insert a index bigger than all the keys in the tree
    // get the last leaf node
#ifdef DEBUG
    cout<<"insertToLastLeaf"<<endl;
#endif
    Node* node = readNode(rootId);
    while(!node->isLeaf){
#ifdef DEBUG
        cout<<"node->isLeaf = "<<node->isLeaf<<endl;
        cout<<"goto "<<node->getLastIndex().rid.pageIdx<<endl;
#endif
        unsigned int pageIdx = node->getLastIndex().rid.pageIdx;
        delete node;
#ifdef DEBUG
        cout<<"delete node"<<endl;
#endif
        node = readNode(pageIdx); // the largest key in the node
    }

    // insert into the last leaf node
#ifdef DEBUG
    cout<<"insert to the leaf node"<<endl;
#endif
    bool result = node->insertIndex(index);
    if(!result){
        delete node;
        return false;
    }

    // check if the node is full and split if necessary
    if(node->isOverflow()){
        return splitNode(node);
    } else {
        writeNode(node);
        return updateKey(node);
    }
}

bool Tree::insertToNode(Index index, Node* node){
    // insert the index into the node
    if(!node->isLeaf){
        delete node;
        return false;
    } else {
        // insert into the leaf node
        bool result = node->insertIndex(index);
        if(!result){
            delete node;
            return false;
        }
    }

    // check if the node is full and split if necessary
    if(node->isOverflow()){
        return splitNode(node);
    } else {
        writeNode(node);
        return updateKey(node);
    }
}

bool Tree::splitNode(Node* node){
    // this function must start in a overflowed leaf node
#ifdef DEBUG
    cout<<"splitNode"<<endl;
#endif
    if(!node->isLeaf){
        delete node;
        return false;
    }

    while(node->isOverflow()){
        unsigned int left_num = m/2 + 1;
        unsigned int right_num = m - left_num + 1;

        // get meta
        unsigned int parentId = node->parentId;
        unsigned int prevId = node->prevId;
        unsigned int succId = node->succId;
        bool isRoot = node->isRoot;
        bool isLeaf = node->isLeaf;
#ifdef DEBUG
        if(node->nodeId == 11){
            cout<<"split node 11"<<endl;
            cout<<"parent "<<parentId<<endl;
            cout<<"prev "<<prevId<<endl;
            cout<<"succ "<<succId<<endl;
        }
        if(node->nodeId == 9){
            cout<<"split node 9"<<endl;
            cout<<"parent "<<parentId<<endl;
            cout<<"prev "<<prevId<<endl;
            cout<<"succ "<<succId<<endl;
        }
#endif

        // init indexes of two nodes
        list<Index> left_indexes = list<Index>();
        list<Index> right_indexes = list<Index>();
        for(unsigned int i = 0; i < left_num; i++){
            left_indexes.push_back(node->indexes.front());
            node->indexes.pop_front();
        }
        for(unsigned int i = 0; i < right_num; i++){
            right_indexes.push_back(node->indexes.front());
            node->indexes.pop_front();
        }
#ifdef DEBUG
        if(node->nodeId == 11){
            cout<<"left indexes"<<endl;
            for(list<Index>::iterator it = left_indexes.begin(); it != left_indexes.end(); it++){
                it->print();
            }
            cout<<"right indexes"<<endl;
            for(list<Index>::iterator it = right_indexes.begin(); it != right_indexes.end(); it++){
                it->print();
            }
        }
        if(node->nodeId == 9){
            cout<<"left indexes"<<endl;
            for(list<Index>::iterator it = left_indexes.begin(); it != left_indexes.end(); it++){
                it->print();
            }
            cout<<"right indexes"<<endl;
            for(list<Index>::iterator it = right_indexes.begin(); it != right_indexes.end(); it++){
                it->print();
            }
        }
#endif

        // if this node is the root, create a new root
        Node* root, *left_node, *right_node;
        unsigned int right_nodeId = allocNode();
        if(isRoot){
            // create a new root
            unsigned int new_rootId = allocNode();
            root = new Node(new_rootId, false, true, 0, 0, 0, list<Index>(), m);

            // assemble the left and right node
            // left is in the original page
            left_node = new Node(node->nodeId, isLeaf, false, new_rootId, right_nodeId, prevId, left_indexes, m);
            // right is in the new page
            right_node = new Node(right_nodeId, isLeaf, false, new_rootId, succId, node->nodeId, right_indexes, m);
            // if the right is not a leaf, update the parent id of the children
            if(!isLeaf){
                for(list<Index>::iterator it = right_indexes.begin(); it != right_indexes.end(); it++){
                    Node* child = readNode(it->rid.pageIdx);
                    child->parentId = right_nodeId;
                    writeNode(child);
                    delete child;
                }
            }

            // assemble the root
            Key left_key = left_node->getLastIndex().key;
            Key right_key = right_node->getLastIndex().key;
            // root->insertIndex(Index(left_key, RID(left_node->nodeId, 0)));
            // root->insertIndex(Index(right_key, RID(right_node->nodeId, 0)));
            root->indexes = list<Index>();
            root->indexes.push_back(Index(left_key, RID(left_node->nodeId, 0)));
            root->indexes.push_back(Index(right_key, RID(right_node->nodeId, 0)));

            // write the nodes to the page
            writeNode(left_node);
            writeNode(right_node);
            writeNode(root);

            // finish spliting
            delete root;
            delete left_node;
            delete right_node;
            delete node;

            // update the rootId
            rootId = new_rootId;
            return true;
        } else {
            // this node is not the root
            Node* parent = readNode(parentId);

            // assemble the left and right node
            // left is in the original page
            left_node = new Node(node->nodeId, isLeaf, false, parentId, right_nodeId, prevId, left_indexes, m);
            // right is in the new page
            right_node = new Node(right_nodeId, isLeaf, false, parentId, succId, node->nodeId, right_indexes, m);
            // also update the prev and succ in the neighbors
            if(prevId != 0){
                Node* prev = readNode(prevId);
                prev->succId = left_node->nodeId;
                writeNode(prev);
                delete prev;
            }
            if(succId != 0){
                Node* succ = readNode(succId);
                succ->prevId = right_node->nodeId;
                writeNode(succ);
                delete succ;
            }
            // if the right is not a leaf, update the parent id of the children
            if(!isLeaf){
                for(list<Index>::iterator it = right_indexes.begin(); it != right_indexes.end(); it++){
                    Node* child = readNode(it->rid.pageIdx);
                    child->parentId = right_nodeId;
                    writeNode(child);
                    delete child;
                }
            }

            // delete the original pointer to the node in the parent node
            // parent->deletePointer(node->nodeId);
            // add the two new pointers to the parent node
            Key left_key = left_node->getLastIndex().key;
            Key right_key = right_node->getLastIndex().key;
            // parent->insertIndex(Index(left_key, RID(left_node->nodeId, 0)));
            // parent->insertIndex(Index(right_key, RID(right_node->nodeId, 0)));
            parent->dividePointer(node->nodeId, Index(left_key, RID(left_node->nodeId, 0)), Index(right_key, RID(right_node->nodeId, 0)));

            // write the nodes to the page
            writeNode(left_node);
            writeNode(right_node);
            writeNode(parent);

            // continue checking parent node
            delete left_node;
            delete right_node;
            delete node;
            node = parent;
        } 
    }

    return updateKey(node);
}

bool Tree::updateKey(Node* node){
    Node* parent;
    while(!node->isRoot){
        // get the parent node
        unsigned int parentId = node->parentId;
        parent = readNode(parentId);

        // update the pointer in the parent node
        Key key = node->getLastIndex().key;
        Index old_pointer = parent->getPointer(node->nodeId);
        if(old_pointer.rid.pageIdx == node->nodeId && old_pointer.key == key){
            // need not update
            delete node;
            delete parent;
            return true;
        }
        parent->updatePointer(node->nodeId, key);

        // write the parent node to the page
        writeNode(parent);

        // continue checking parent node
        delete node;
        node = parent;
    }
    delete node;
    return true;
}

bool Tree::isFirstChild(Node* node, Node* parent){
    if(parent->isLeaf){
        return false;
    } else {
        if(parent->getPointerPos(node->nodeId) == 0){
            return true;
        } else {
            return false;
        }
    }
}

bool Tree::isLastChild(Node* node, Node* parent){
    if(parent->isLeaf){
        return false;
    } else {
        if(parent->getPointerPos(node->nodeId) == parent->indexes.size()-1){
            return true;
        } else {
            return false;
        }
    }
}


// delete
bool Tree::deleteIndex(Key key){
    // search with the key in the tree
    Node* node = readNode(rootId);
    while(!node->isLeaf){
        // search for the first child with its biggest key >= key
        Index index = node->searchIndex(key);
        if(index.rid.pageIdx == 0 && index.rid.slotIdx == 0){
            // the key is bigger than all the keys in the node
            delete node;
            return false;   //no result
        } else {
            // get the child node
            delete node;
            node = readNode(index.rid.pageIdx);
        }
    }

    // search the index in the leaf node
    bool result = node->deleteIndex(key);
    if(!result){
        delete node;
        return false;
    } else if(node->isUnderflow()){
        // if the node is underflow, merge it with its sibling
        return mergeNode(node);
    } else {
        // write the node to the page
        writeNode(node);
        // update the key in the parent nodes
        return updateKey(node);
    }
}

bool Tree::mergeNode(Node* node){
    // must start from a leaf node
    if(!node->isLeaf){
        delete node;
        return false;
    }

    unsigned int parentId, nodeId, prevId, succId;
    Node* parent, *prev_node, *succ_node;
    while(node->isUnderflow() && !node->isRoot){
        parentId = node->parentId;
        nodeId = node->nodeId;
        prevId = node->prevId;
        succId = node->succId;
        parent = readNode(parentId);

        // 1. check if any neighbor node can lend an index
        if(!isFirstChild(node, parent)){
            prev_node = readNode(prevId);
            if(prev_node->isBorrowable()){
                // borrow from the previous node
                Index borrowed = prev_node->popLastIndex();
                node->insertIndex(borrowed);

                // if is not leaf, update the parent of the moved node
                if(!node->isLeaf){
                    Node* moved_child = readNode(borrowed.rid.pageIdx);
                    moved_child->parentId = nodeId;
                    writeNode(moved_child);
                    delete moved_child;
                }

                // update the parent node
                parent->updatePointer(nodeId, node->getLastIndex().key);
                parent->updatePointer(prevId, prev_node->getLastIndex().key);

                // write the nodes to the page
                writeNode(node);
                writeNode(prev_node);
                writeNode(parent);

                // finish merging
                delete node;
                delete prev_node;
                return updateKey(parent);
            }
        } else if (!isLastChild(node, parent)){
            succ_node = readNode(succId);
            if(succ_node->isBorrowable()){
                // borrow from the next node
                Index borrowed = succ_node->popFirstIndex();
                node->insertIndex(borrowed);

                // if is not leaf, update the parent of the moved node
                if(!node->isLeaf){
                    Node* moved_child = readNode(borrowed.rid.pageIdx);
                    moved_child->parentId = nodeId;
                    writeNode(moved_child);
                    delete moved_child;
                }

                // update the parent node
                parent->updatePointer(nodeId, node->getLastIndex().key);
                parent->updatePointer(succId, succ_node->getLastIndex().key);

                // write the nodes to the page
                writeNode(node);
                writeNode(succ_node);
                writeNode(parent);

                // finish merging
                delete node;
                delete succ_node;
                return updateKey(parent);
            }
        }

        // 2. merge with the neighbor node if exists
        if(!isFirstChild(node, parent)){
            // merge with the previous node
            prev_node = readNode(prevId);
            prev_node->mergeBack(node);

            // if has the succ, change the prev
            if(node->hasSuccLeaf()){
                succ_node = readNode(succId);
                succ_node->prevId = prevId;
                writeNode(succ_node);
                delete succ_node;
            }

            // if is not leaf node, update the parent of the children
            if(!node->isLeaf){
                for(list<Index>::iterator iter = node->indexes.begin(); iter != node->indexes.end(); iter++){
                    Node* child = readNode(iter->rid.pageIdx);
                    child->parentId = prevId;
                    writeNode(child);
                    delete child;
                }
            }  

            // update the parent node
            parent->deletePointer(nodeId);
            parent->updatePointer(prevId, prev_node->getLastIndex().key);

            // write the nodes to the page
            writeNode(prev_node);

            // free this node in the bitmap
            freeNode(nodeId);

            // continue merging
            delete node;
            delete prev_node;
            node = parent;
        } else if (!isLastChild(node, parent)){
            // merge with the next node
            succ_node = readNode(succId);
            succ_node->mergeFront(node);

            // if has the prev, change the succ
            if(node->hasPrevLeaf()){
                prev_node = readNode(prevId);
                prev_node->succId = succId;
                writeNode(prev_node);
                delete prev_node;
            }

            // if is not leaf node, update the parent of the children
            if(!node->isLeaf){
                for(list<Index>::iterator iter = node->indexes.begin(); iter != node->indexes.end(); iter++){
                    Node* child = readNode(iter->rid.pageIdx);
                    child->parentId = succId;
                    writeNode(child);
                    delete child;
                }
            }   

            // update the parent node
            parent->deletePointer(nodeId);
            parent->updatePointer(succId, succ_node->getLastIndex().key);

            // write the nodes to the page
            writeNode(succ_node);

            // free this node in the bitmap
            freeNode(nodeId);

            // continue merging
            delete node;
            delete succ_node;
            node = parent;
        } else {
            // the node is the only leaf node
            // not possible
        }
    }

    if(!node->isRoot){
        writeNode(node);
        return updateKey(node);
    } else {
#ifdef DEBUG
        cout << "merge the root node" << endl;
        node->print();
#endif  
        // if the root node is a leaf, do nothing
        if(node->isLeaf){
            writeNode(node);
            delete node;
            return true;
        }

        // stop at the root node, so the node's indexes is reduced
        // check if there is only one index left
        if(node->indexes.size() == 1){
            // set the child to be the new root
            rootId = node->indexes.front().rid.pageIdx;
            Node* child = readNode(rootId);
            child->isRoot = true;
            child->parentId = 0;
            writeNode(child);
            delete child;
            // free the old root node
            freeNode(node->nodeId);
            delete node;
            return true;
        } else {
            writeNode(node);
            delete node;
            return true;
        }
    }
}

// delete all the indexes with the key
bool Tree::deleteIndexes(Key key){
    // search for the index
    list<Index> indexes = searchIndexes(key);
    if(indexes.size() == 0){
        return false;
    } else {
        bool result = true;
        for(int i = 0; i < indexes.size(); i++){
            result = deleteIndex(key);
            if(!result){
                return false;
            }
        }
    }
    return true;
}

bool Tree::deleteIndex(Key key, RID& rid){
    // cout<<"delete index ";
    // key.print();
    // cout<<" ";
    // rid.print();
    // cout<<endl;

    // get the root node
    Node* node = readNode(rootId);

    // search through the non-leaf nodes to find the leaf node
    while(!node->isLeaf){
        // search for the first child with its biggest key >= key
        Index index = node->searchIndex(key);
        if(index.rid.pageIdx == 0 && index.rid.slotIdx == 0){
            // the key is bigger than all the keys in the node
            delete node;
            return false;   //no result
        } else {
            // get the child node
            delete node;
            node = readNode(index.rid.pageIdx);
        }
    }

    // start from the leaf node, search to the right until return empty list
    while(true){
        // delete from the node
        bool has;
        bool result = node->deleteIndex(Index(key, rid), has);
// cout<<"delete result: "<<result<<endl;
// cout<<"has: "<<has<<endl;
        if(!result){
            // find no result in this node
            if(!has || !node->hasSuccLeaf()){    // if no results in this leaf or the last leaf
                // no more result
                delete node;
                break;
            } else {
                // get the next leaf node
                delete node;
                node = readNode(node->succId);
            }
        } else if(node->isUnderflow()){
            // if the node is underflow, merge it with its sibling
            return mergeNode(node);
        } else {
            // write the node to the page
            writeNode(node);
// check the root node
// Node* root = readNode(rootId);
// root->print();
// delete root;
            // update the key in the parent nodes
            return updateKey(node);
        }
    }

    return false;
}

// search
Index Tree::searchIndex(Key key){
    // get the root node
    Node* node = readNode(rootId);

    // search through the non-leaf nodes
    while(!node->isLeaf){
        // search for the first child with its biggest key >= key
        Index index = node->searchIndex(key);
        if(index.rid.pageIdx == 0 && index.rid.slotIdx == 0){
            // the key is bigger than all the keys in the node
            delete node;
            return index;   //no result
        } else {
            // get the child node
            delete node;
            node = readNode(index.rid.pageIdx);
        }
    }
    
    // search through the leaf node
    Index index = node->searchIndex(key);
    delete node;
    return index;
}


list<Index> Tree::searchIndexes(Key key){
    // get the root node
    Node* node = readNode(rootId);
    list<Index> res;

    // search through the non-leaf nodes to find the leaf node
    while(!node->isLeaf){
        // search for the first child with its biggest key >= key
        Index index = node->searchIndex(key);
        if(index.rid.pageIdx == 0 && index.rid.slotIdx == 0){
            // the key is bigger than all the keys in the node
            delete node;
            return res;   //no result
        } else {
            // get the child node
            delete node;
            node = readNode(index.rid.pageIdx);
        }
    }

    // start from the leaf node, search to the right until return empty list
    while(true){
        list<Index> indexes = node->searchIndexes(key);
        // add the result to the list
        res.insert(res.end(), indexes.begin(), indexes.end());
        if(indexes.empty() || !node->hasSuccLeaf()){    // if no results in this leaf or the last leaf
            // no more result
            delete node;
            break;
        } else {
            // get the next leaf node
            delete node;
            node = readNode(node->succId);
        }
    }

    return res;
}

list<Index> Tree::searchRange(Key key1, Key key2){
    if(!(key1 < key2)){
        return list<Index>();
    }
    // get all the indexes [key1, key2)
    list<Index> res;

    // get the root node
    Node* node = readNode(rootId);

    // search through the non-leaf nodes to find the leaf node
    while(!node->isLeaf){
        // search for the first child with its biggest key >= key
        Index index = node->searchIndex(key1);
        if(index.rid.pageIdx == 0 && index.rid.slotIdx == 0){
            // the key is bigger than all the keys in the node
            delete node;
            return res;   //no result
        } else {
            // get the child node
            delete node;
            node = readNode(index.rid.pageIdx);
        }
    }

    // start from the leaf node, search to the right until find the first key >= key2
    while(true){
        list<Index> indexes = node->searchRange(key1, key2);
        // add the result to the list
        res.insert(res.end(), indexes.begin(), indexes.end());
        if(indexes.empty() || !node->hasSuccLeaf()){    // if no results in this leaf or the last leaf
            // no more result
            delete node;
            break;
        } else {
            // get the next leaf node
            delete node;
            node = readNode(node->succId);
        }
    }

    return res;
}


// tools
Node* Tree::readNode(unsigned int pageId){
    // needs to promise that the pageId is in the bitmap
#ifdef DEBUG
    cout<<"readNode: "<<pageId<<endl;
    cout.flush();
#endif
    int index = 0;
    BufType page = bpm->getPage(fileId, pageId, index);
    Node* node = new Node(m);
    node->deserialize((char*)page);
#ifdef DEBUG
    cout<<"after readNode: "<<node->nodeId<<endl;
#endif
    return node;
}

void Tree::writeNode(Node* node){
    // needs to promise that the pageId is in the bitmap
#ifdef DEBUG
    cout<<"writeNode: "<<node->nodeId<<endl;
#endif
    int index = 0;
    BufType page = bpm->getPage(fileId, node->nodeId, index);
    node->serialize((char*)page);
    bpm->markDirty(index);
    bpm->writeBack(index);
}

unsigned int Tree::allocNode(){
    // find the first 0 in the bitmap
    for (int i = 0; i < bitmap.size(); i++){
        if (!bitmap[i]){
            bitmap[i] = true;
#ifdef DEBUG
            cout<<"allocNode: "<<i<<endl;
#endif
            return i;
        }
    }
    cout<<"Error: indexes number exceeds the limit"<<endl;
    return -1;
}

bool Tree::freeNode(unsigned int pageId){
    if(!bitmap[pageId]) {
        cout<<"Error: free a node that is not allocated"<<endl;
        return false;
    }
#ifdef DEBUG
    cout<<"freeNode: "<<pageId<<endl;
#endif
    bitmap[pageId] = false;
    return true;
}


void Tree::print(){
    cout<<"--------------"<<endl;
    cout<<"Tree: "<<endl;
    cout<<"Root: "<<rootId<<endl;
    cout<<"dimension: "<<m<<endl;
    cout<<"Bitmap: "<<endl;
    for(int i = 0; i < 128; i++){
        cout<<bitmap[i]<<" ";
    }
    cout<<endl;
    cout<<"Nodes: "<<endl;
    for(int i = 2; i < bitmap.size(); i++){
        if(bitmap[i]){
            Node* node = readNode(i);
            node->print();
            delete node;
        }
    }
    cout<<"--------------"<<endl;
    return ;
}


bool Tree::checkOrder(){
    // get the first leaf node
    Node* node = readNode(rootId);
    while(!node->isLeaf){
        Node* next = readNode(node->getFirstIndex().rid.pageIdx);
        delete node;
        node = next;
    }

    Node* first_parent = readNode(node->parentId);

    // check the order of the leaf nodes
    while(true){
        for(list<Index>::iterator it = node->indexes.begin(); it != node->indexes.end(); it++){
            if(it != node->indexes.begin()){
                list<Index>::iterator prev = it;
                prev--;
                if(!(prev->key <= it->key)){
                    cout<<"Error: the order is wrong"<<endl;
                    cout<<"Position: "<<node->nodeId<<endl;
                    return false;
                }
            }
        }

        if(!node->hasSuccLeaf()){
            delete node;
            break;
        } else {
            Node* next = readNode(node->succId);
            Key last = node->getLastIndex().key;
            Key first = next->getFirstIndex().key;
            if(!(last <= first)){
                cout<<"Error: the order is wrong"<<endl;
                cout<<"Between: "<<node->nodeId<<" and "<<next->nodeId<<endl;
                return false;
            }
            delete node;
            node = next;
        }
    }

    // check if the leafnodes are linked correctly
    node = first_parent;
    while(node->hasSuccLeaf()){
        // go through the children of the node
        Node * child, *prev_child;
        for(list<Index>::iterator it = node->indexes.begin(); it != node->indexes.end(); it++){
            child = readNode(it->rid.pageIdx);
            if(it != node->indexes.begin()){
                list<Index>::iterator prev = it;
                prev--;
                prev_child = readNode(prev->rid.pageIdx);
                if(!(prev_child->succId == child->nodeId) || !(child->prevId == prev_child->nodeId)){
                    cout<<"false child link"<<endl;
                    cout<<"Between: "<<prev_child->nodeId<<" and "<<child->nodeId<<endl;
                    cout<<"Parent: "<<node->nodeId<<endl;
                    delete child;
                    delete prev_child;
                    return false;
                }
            } else {
                if(!(child->prevId == 0)){
                    cout<<"false child link"<<endl;
                    cout<<"Between: "<<0<<" and "<<child->nodeId<<endl;
                    cout<<"Parent: "<<node->nodeId<<endl;
                    delete child;
                    return false;
                }
            }
            delete child;
        }
        unsigned int next = node->succId;
        delete node;
        node = readNode(next);
    }
    return true;
}