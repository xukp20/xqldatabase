#include <iostream>
// #include "record_management/RecordManagerTester.h"
// #include "utils/ConditionTester.h"
// #include "system_management/SystemTester.h"
// #include "system_management/TableIndex.h"
// #include "index_management/IndexTester.h"
#include "query_parse/QueryParseTester.h"

int main(){
    // RecordManagerTester::test_create();
    // RecordManagerTester::test_batch_insert();

    // cout<<"test num"<<endl;
    // ConditionTester::test_num();
    // cout<<"test str"<<endl;
    // ConditionTester::test_str();
    // ConditionTester::test_list();
    // ConditionTester::test_null();
    // SystemTester::testTableMetaSerialize();
    // SystemTester::testTableIndexSerialize();
    // SystemTester::testDBIndexSerialize();
    // SystemTester::testInitSystem();
    // SystemTester::testCreateDB();
    // SystemTester::testcreateRawTable();
    // SystemTester::testSetTableIndex();
    // SystemTester::testReadWritePage();
    // IndexTester::testNode();
    // IndexTester::testCreateTree();
    // IndexTester::testInsert();
    // IndexTester::testDelete();
    // IndexTester::buildModelTree();
    // IndexTester::testSearch();
    // IndexTester::testSearchRange();
    // IndexTester::testDeleteAll();
    // IndexTester::testSearchRange();
    // IndexTester::testBigScale();
    // SystemTester::testCreateIndex();
    // SystemTester::testConflictIndex();
    QueryParseTester::testEnv();
    return 0;
}