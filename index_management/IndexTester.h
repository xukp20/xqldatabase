// testing for index management

#ifndef INDEX_TESTER_H
#define INDEX_TESTER_H

#include <iostream>
#include <string>
#include "BplusTree/Tree.h"

class IndexTester {
public:
    static void testCreateTree();
    static void testNode();
    static void buildModelTree();
    static void testInsert(unsigned int slotIdx = 0, bool print=false);
    static void testDelete();
    static void testSearch();
    static void testSearchRange();
    static void testDeleteAll();
    static void testBigScale();
};

void IndexTester::testNode(){
    vector<int> vals;
    vals.push_back(1);
    vals.push_back(2);
    Key key = Key(vals);
    Index index = Index(key, RID(1, 2));
    Index index2 = Index(key, RID(3, 4));
    list<Index> indices;
    indices.push_back(index);
    indices.push_back(index2);

    Node* node = new Node(2, true, true, 0, 1, 3, indices, 5);
    node->print();

    char* buffer = new char[PAGE_SIZE];

    node->serialize(buffer);
    Node* node2 = new Node(5);
    node2->deserialize(buffer);
    node2->print();

    delete node;
    delete node2;
    delete[] buffer;

    return;
}

void IndexTester::testCreateTree(){
    FileManager* fm = new FileManager();
    BufPageManager* bm = new BufPageManager(fm);

    bool result = Tree::createTree("test_tree", fm, bm, 5);
    if(!result){
        cout<<"create tree failed"<<endl;
        return;
    }

    Tree* tree = new Tree("test_tree", fm, bm);
    tree->initTree();
    // tree->print();
    return;
}

void IndexTester::buildModelTree(){
    testCreateTree();
    for(int i = 0; i < 4; i++){
        testInsert(i);
    }
}

void IndexTester::testInsert(unsigned int sloxIdx, bool print){
    FileManager* fm = new FileManager();
    BufPageManager* bm = new BufPageManager(fm);

    Tree* tree = new Tree("test_tree", fm, bm);
    tree->initTree();
    if (print)
        tree->print();

    // 1
    Key key = Key(1);
    RID rid = RID(1, sloxIdx);
    Index index = Index(key, rid);
    tree->insertIndex(index);
    if (print)
        tree->print();

    // 2
    key = Key(2);
    rid.pageIdx = 2;
    index = Index(key, rid);
    tree->insertIndex(index);
    if (print)
        tree->print();

    // 3
    key = Key(3);
    rid.pageIdx = 3;
    index = Index(key, rid);
    tree->insertIndex(index);
    if (print)
        tree->print();

    // 4 
    key = Key(4);
    rid.pageIdx = 4;
    index = Index(key, rid);
    tree->insertIndex(index);
    if (print)
        tree->print();

    // 5
    key = Key(5);
    rid.pageIdx = 5;
    index = Index(key, rid);
    tree->insertIndex(index);
    if (print)
        tree->print();

    // 6
    key = Key(6);
    rid.pageIdx = 6;
    index = Index(key, rid);
    tree->insertIndex(index);
    if (print)
        tree->print();

    delete tree;
    delete fm;
    delete bm;
    return;
}

void IndexTester::testDelete(){
    // use this after 4 of the insertions in testInsert()
    // original tree:
    /*
                            Node 10: 3 6    (Key)
                                     4 9    (Node)
            
                Node 4: 1 2 3               Node 9: 4 5 6
                        2 7 5                       3 8 6
        
    Node 2: 1 1 1                       Node 3: 4 4 4 
            Node 7: 1 2 2 2                     Node 8: 4 5 5 5
                    Node 5: 2 3 3 3 3                   Node 6: 5 6 6 6 6
    */
    
    cout<<setw(30)<<setfill('=')<<""<<endl;
    cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl;

    FileManager* fm = new FileManager();
    BufPageManager* bm = new BufPageManager(fm);

    Tree* tree = new Tree("test_tree", fm, bm);
    tree->initTree();
    tree->print();

    // delete 1s
    cout<<"delete 1s"<<endl;
    // four 1
    Key key = Key(1);
    for (int i = 0; i < 4; i++){
        tree->deleteIndex(key);
        tree->print();
    }

    /* after delete four 1s
                                Node 10: 3 6    (Key)
                                     4 9    (Node)
            
                Node 4: 2 3               Node 9: 4 5 6
                        7 5                       3 8 6
        
                                        Node 3: 4 4 4 
            Node 7: 2 2 2                       Node 8: 4 5 5 5
                    Node 5: 2 3 3 3 3                   Node 6: 5 6 6 6 6
    */
    // delete 2s
    cout<<"delete 2s"<<endl;
    // four 2
    key = Key(2);
    for (int i = 0; i < 4; i++){
        tree->deleteIndex(key);
        tree->print();
    }

    /* after delete four 2s
                                Node 10: 3 6    (Key)
                                     4 9    (Node)
            
                Node 4: 3 3              Node 9: 4 5 6
                        7 5                       3 8 6
        
                                        Node 3: 4 4 4 
            Node 7: 3 3                       Node 8: 4 5 5 5
                    Node 5: 3 3                        Node 6: 5 6 6 6 6
    */

    // delete 3s
    cout<<"delete 3s"<<endl;
    // four 3
    key = Key(3);
    for (int i = 0; i < 4; i++){
        tree->deleteIndex(key);
        tree->print();
    }

    /* after delete four 3s
                    Node 9: 4 5 6
                            3 8 6
        
                Node 3: 4 4 4 
                        Node 8: 4 5 5 5
                                Node 6: 5 6 6 6 6
    */

    delete tree;
    delete fm;
    delete bm;
    return;
}

void IndexTester::testSearch(){
    // original tree:
    /*
                            Node 10: 3 6    (Key)
                                     4 9    (Node)
            
                Node 4: 1 2 3               Node 9: 4 5 6
                        2 7 5                       3 8 6
        
    Node 2: 1 1 1                       Node 3: 4 4 4 
            Node 7: 1 2 2 2                     Node 8: 4 5 5 5
                    Node 5: 2 3 3 3 3                   Node 6: 5 6 6 6 6
    */
    
    FileManager* fm = new FileManager();
    BufPageManager* bm = new BufPageManager(fm);

    Tree* tree = new Tree("test_tree", fm, bm);
    tree->initTree();
    // tree->print();

    // search one index 
    Key key = Key(1);
    Index result = tree->searchIndex(key);
    result.print();
    key = Key(5);
    result = tree->searchIndex(key);
    result.print();

    // search a range of indexes
    key = Key(3);
    list<Index> results = tree->searchIndexes(key);
    for (list<Index>::iterator it = results.begin(); it != results.end(); it++){
        it->print();
    }

    key = Key(-1);
    result = tree->searchIndex(key);
    result.print();
    results = tree->searchIndexes(key);
    for (list<Index>::iterator it = results.begin(); it != results.end(); it++){
        it->print();
    }

    key = Key(7);
    result = tree->searchIndex(key);
    result.print();
    results = tree->searchIndexes(key);
    for (list<Index>::iterator it = results.begin(); it != results.end(); it++){
        it->print();
    }

    delete tree;
    delete fm;
    delete bm;
    return;
}

void IndexTester::testSearchRange(){
    cout<<"test SearchRange"<<endl;
    FileManager* fm = new FileManager();
    BufPageManager* bm = new BufPageManager(fm);

    Tree* tree = new Tree("test_tree", fm, bm);
    tree->initTree();

    // search a range of indexes
    Key key1 = Key(3);
    Key key2 = Key(5);
    list<Index> results = tree->searchRange(key1, key2);
    for (list<Index>::iterator it = results.begin(); it != results.end(); it++){
        it->print();
    }

    key1 = Key(1);
    key2 = Key(8);
    results = tree->searchRange(key1, key2);
    for (list<Index>::iterator it = results.begin(); it != results.end(); it++){
        it->print();
    }
}

void IndexTester::testDeleteAll(){
    cout<<"test DeleteAll"<<endl;
    FileManager* fm = new FileManager();
    BufPageManager* bm = new BufPageManager(fm);

    Tree* tree = new Tree("test_tree", fm, bm);
    tree->initTree();
    tree->print();

    // delete all
    cout<<"delete all"<<endl;
    Key key = Key(4);
    tree->deleteIndexes(key);
    tree->print();

    delete tree;
    delete fm;
    delete bm;
}

void IndexTester::testBigScale(){
    cout<<"test BigScale"<<endl;
    FileManager* fm = new FileManager();
    BufPageManager* bm = new BufPageManager(fm);

    Tree* tree = new Tree("test_tree", fm, bm);
    tree->initTree();
    tree->print();

    // insert 100 indexes
    cout<<"insert 100 different keys"<<endl;
    for (int i = 11; i < 111; i++){
        for(int j = 0; j < i; j++){
            cout<<"insert "<<i<<" "<<j<<endl;
            tree->insertIndex(Index(Key(i), RID(i, j)));
        }
    }

    // search
    cout<<"search 1"<<endl;
    Key key = Key(31);
    Index result = tree->searchIndex(key);
    result.print();

    // delete 1 
    cout<<"delete 1"<<endl;
    tree->deleteIndex(key);
    result = tree->searchIndex(key);
    result.print();

    // search range
    cout<<"search range"<<endl;
    key = Key(31);
    Key key2 = Key(33);
    list<Index> indexes = tree->searchRange(key, key2);
    for (list<Index>::iterator it = indexes.begin(); it != indexes.end(); it++){
        it->print();
    }

    // delete indexes with the same key
    cout<<"delete range"<<endl;
    tree->deleteIndexes(key);
    indexes = tree->searchRange(key, key2);
    for (list<Index>::iterator it = indexes.begin(); it != indexes.end(); it++){
        it->print();
    }

    // search range 2
    cout<<"search range 2"<<endl;
    key = Key(1);
    key2 = Key(112);
    indexes = tree->searchRange(key, key2);
    cout<<"indexes: "<<endl;
    cout<<"length: "<<indexes.size()<<endl;
    for (list<Index>::iterator it = indexes.begin(); it != indexes.end(); it++){
        it->print();
    }

    delete tree;
    delete fm;
    delete bm;
}
#endif