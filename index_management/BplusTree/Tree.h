/*
    B plus tree class based on file system
    non-leaf node stores the biggest key and the page id of the child
    created: 2022/12/9
*/

#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <string>
#include <vector>
#include "Node.h"
#include "../../filesystem/fileio/FileManager.h"
#include "../../filesystem/bufmanager/BufPageManager.h"

using namespace std;

class Tree
{
public:
    unsigned int rootId; // pageId for the root page
    unsigned int m;      // max number of indexes in one page
    string fileName;     // file name for the tree
    vector<bool> bitmap; // bitmap for the not empty pages

    FileManager *fm;     // file manager
    BufPageManager *bpm; // buffer manager

    // attributes related to the file
    int fileId; // file id for the tree in the fm

    // assemble a tree from an existing file
    Tree(string fileName, FileManager *fm, BufPageManager *bpm); // empty tree for deserializing
    // update meta data and close the file
    ~Tree();

    // initialize the tree from the file
    bool initTree();

    // save the tree into the file
    // do the save as the destructor
    // will close the file after saving
    bool saveTree();

    // initialize a new tree into a new file
    // use page 2 as the root
    // root node is a leaf node, with no index
    // return true if success
    static bool createTree(string fileName, FileManager *fm, BufPageManager *bpm, unsigned int m=MAX_M);

    // insert a new index into the tree
    // if the node is full, split it
    // return true if success
    bool insertIndex(Index index);

    // tool for insertIndex
    // insert to the last leaf node if the key is larger than all
    // split if needed
    bool insertToLastLeaf(Index index);
    // tool for insert Index
    // insert to the given node, must be a leaf node
    // split if needed
    bool insertToNode(Index index, Node* node);

    // delete one index with the key
    // if not found, return false
    // if the node is too small, merge it
    bool deleteIndex(Key key);

    // delete all the indexes with the same key
    // if not found, return false
    // call deleteIndex
    bool deleteIndexes(Key key);

    // delete the first index with the key and the rid
    // if not found, return false
    bool deleteIndex(Key key, RID& rid);

    // search for the index with the key
    // if not found, return the first index that is larger than the key
    // if all smaller, assemble a index with pageIdx=0 slotIdx=0
    Index searchIndex(Key key);

    // searchIndexes: search for all the indexes with the key
    // only get the equal ones
    list<Index> searchIndexes(Key key);

    // searchRange: search for all the indexes in the range [key1, key2)
    list<Index> searchRange(Key key1, Key key2);

    // serialize the tree into a file
    // only write the meta pages
    // let the buffer manager write the dirty pages
    void serialize();

    // deserialize the tree from a file
    // use fm and bpm to read the pages
    // needs to open the file before this function
    bool deserialize();

    // get a existing node from a page according to the pageId
    // created by new, needs to be deleted after using
    Node* readNode(unsigned int pageId);

    // write the node into a page according to the pageId
    void writeNode(Node* node);

    // splitNode: split the node after insert if needed
    // also call updateKey from the highest no split node to above
    bool splitNode(Node* node);

    // called after insert into a leaf node and need not split
    // or after spliting till a node that does not split
    // update the key in the ancestors
    // check if the biggest key is changed
    bool updateKey(Node* node);

    // check if this node is the first child of its parent
    // so that it doesn't has a left sibling
    // won't free the node
    bool isFirstChild(Node* node, Node* parent);

    // check if this node is the last child of its parent
    // so that it doesn't has a right sibling
    // won't free the node
    bool isLastChild(Node* node, Node* parent);

    // mergeNode: merge the node after delete if needed
    // first try to borrow from the left sibling, then the right sibling
    // if both fail, merge with the left sibling, pass to the parent
    // check if the root change
    bool mergeNode(Node* node);

    // get a new page for a new node
    // return the pageId
    // will not read the page, simply get the pageId
    // will set the bitmap
    unsigned int allocNode();

    // free a page for a node
    // will not write the page, simply free the pageId
    // will set the bitmap
    bool freeNode(unsigned int pageId);

    // print the structure of the tree
    void print();

    // check the order of the leaves
    bool checkOrder();
};

#endif