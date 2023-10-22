// testing for record_manager.cpp
#ifndef RECORDMANAGERTESTER_H
#define RECORDMANAGERTESTER_H

#include "RecordManager.h"

class RecordManagerTester {
public:
    static void test(){
        {
            RecordManager recorder = RecordManager();

            // test record 
            Record* example = new Record();
            example->append(true, 1);
            example->append(true, float(2.0));
            example->append(true, "hello", 5);

            example->print();

            // test create table
            bool result = recorder.createTableFile("test", example);
            cout<<"create: "<<result<<endl;

            // test open table
            int fileID = recorder.openTableFile("test");
            cout<<"open: "<<fileID<<endl;

            // test insert record
            Record* record = new Record();
            record->append(true, 5);
            record->append(true, float(6.0));
            record->append(true, "world", 5);
            record->print();

            recorder.insertRecord(fileID, record);

            // test select record
            // Condition* cond = new CompareCondition(EQ, 0, 5);
            // vector<Record*> recordList = recorder.selectRecords(fileID, cond, example);
            // cout<<"list select"<<endl;
            // for (int i = 0; i < recordList.size(); i++){
            //     recordList[i]->print();
            // }
            // const RID first_rid = recordList[0]->getRID();
            // cout<<"first rid: "<<first_rid.pageIdx<<" "<<first_rid.slotIdx<<endl;
            // recorder.freeRecordList(recordList);

            // result = recorder.insertRecord(fileID, record);
            // cout<<"insert: "<<result<<endl;

            // // select after the first RID
            // Record* record2 = recorder.selectRecord(fileID, cond, example, first_rid);
            // cout<<"single select"<<endl;
            // record2->print();
            // delete record2;

            // // test delete record
            // result = recorder.deleteRecord(fileID, first_rid);
            // cout<<"delete: "<<result<<endl;

            // // test update record
            // result = recorder.updateRecord(fileID, first_rid, record);
            // cout<<"update: "<<result<<endl;

            // RID second_rid = RID(first_rid.pageIdx, first_rid.slotIdx + 1);
            // Record* record3 = new Record();
            // record3->append(true, 5);
            // record3->append(true, float(2.0));
            // record3->append(true, "hoho", 5);
            // record3->print();
            // result = recorder.updateRecord(fileID, second_rid, record3);
            // cout<<"update record2: "<<result<<endl;

            // // insert back the first
            // result = recorder.insertRecord(fileID, record);
            // cout<<"insert back: "<<result<<endl;

            // // select the first
            // recordList = recorder.selectRecords(fileID, cond, example);
            // for(int i = 0; i < recordList.size(); i++){
            //     recordList[i]->print();
            //     cout<<"rid: "<<recordList[i]->getRID().pageIdx<<" "<<recordList[i]->getRID().slotIdx<<endl;
            //     cout<<endl;
            // }


            // close table
            result = recorder.closeTableFile(fileID);
            cout<<"close: "<<result<<endl;
        }
        
        // open with a new recoder
        {
            RecordManager recorder = RecordManager();
            int fileID = recorder.openTableFile("test");
            cout<<"open: "<<fileID<<endl;

            // test record 
            Record* example = new Record();
            example->append(true, 1);
            example->append(true, float(2.0));
            example->append(true, "hello", 5);

            example->print();

            // test select record
            Condition* cond = new CompareCondition(EQ, 0, 5);
            vector<Record*> recordList = recorder.selectRecords(fileID, cond, example);
            cout<<"list select"<<endl;
            for (int i = 0; i < recordList.size(); i++){
                recordList[i]->print();
            }
            // RID first_rid = recordList[0]->getRID();
            // cout<<"first rid: "<<first_rid.pageIdx<<" "<<first_rid.slotIdx<<endl;
            // recorder.freeRecordList(recordList);

            // close table
            bool result = recorder.closeTableFile(fileID);
            cout<<"close: "<<result<<endl;
        }
        return ;
    }
    static void test_create(){
        RecordManager recorder = RecordManager();

        Record* example = new Record();
        example->append(true, 1);
        example->append(true, float(2.0));
        example->append(true, "hello", 5);

        // test create table
        bool result = recorder.createTableFile("test", example);
        cout<<"create: "<<result<<endl;

        // test open table
        int fileID = recorder.openTableFile("test");
        cout<<"open: "<<fileID<<endl;

        // test close table
        result = recorder.closeTableFile(fileID);
        cout<<"close: "<<result<<endl;
        return;
    }
    static void test_change(){
        RecordManager recorder = RecordManager();

        // test open table
        int fileID = recorder.openTableFile("test");
        cout<<"openID: "<<fileID<<endl;

        // test record 
        Record* example = new Record();
        example->append(true, 1);
        example->append(true, float(2.0));
        example->append(true, "hello", 5);

        // print old record
        recorder.printFile(fileID, example);


        Record* record1 = new Record();
        record1->append(true, 1);
        record1->append(true, float(5.0));
        record1->append(true, "world", 5);

        Record* record2 = new Record();
        record2->append(true, 2);
        record2->append(false, float(6.0));
        record2->append(true, "haha", 5);

        Record* record3 = new Record();
        record3->append(true, 3);
        record3->append(true, float(5.0));
        record3->append(true, "abcdef", 5);

        Record* record4 = new Record();
        record4->append(true, 4);
        record4->append(true, float(4.0));
        record4->append(true, "张", 5);

        Record* record5 = new Record();
        record5->append(true, 5);
        record5->append(true, float(5.0));
        record5->append(true, "张三", 5);
        
        // test insert record
        bool result;
        recorder.insertRecord(fileID, record1);
        // cout<<"insert: "<<result<<endl;

        recorder.insertRecord(fileID, record2);
        // cout<<"insert: "<<result<<endl;

        recorder.insertRecord(fileID, record3);
        // cout<<"insert: "<<result<<endl;

        recorder.printFile(fileID, example);

        // test select record
        Condition* cond_float5 = new CompareCondition(EQ, 1, 5);
        vector<Record*> recordList = recorder.selectRecords(fileID, cond_float5, example);
        cout<<endl<<"list select: "<<endl;
        for (int i = 0; i < recordList.size(); i++){
            recordList[i]->print();
        }
        RID first_rid = recordList[0]->getRID();
        cout<<"first rid: "<<first_rid.pageIdx<<" "<<first_rid.slotIdx<<endl;
        recorder.freeRecordList(recordList);

        Record* record = recorder.selectRecord(fileID, cond_float5, example, first_rid);
        cout<<endl<<"single select: "<<endl;
        record->print();
        delete record;

        // test delete record
        result = recorder.deleteRecord(fileID, first_rid);
        cout<<"delete: "<<result<<endl;
        recorder.printFile(fileID, example);

        // test update record
        recorder.insertRecord(fileID, record4);
        // cout<<"insert: "<<result<<endl;
        recorder.printFile(fileID, example);
        
        result = recorder.updateRecord(fileID, first_rid, record5);
        cout<<"update: "<<result<<endl;
        recorder.printFile(fileID, example);

        // test close table
        result = recorder.closeTableFile(fileID);
        cout<<"close: "<<result<<endl;

        return;
    }
    static void test_multi_file(){
        RecordManager recorder = RecordManager();

        // create table
        Record* example = new Record();
        example->append(true, 1);
        example->append(true, float(2.0));
        example->append(true, "hello", 10);

        bool result = recorder.createTableFile("test1", example);
        result = recorder.createTableFile("test2", example);

        // test open table
        int fileID1 = recorder.openTableFile("test1");
        cout<<"openID: "<<fileID1<<endl;

        int fileID2 = recorder.openTableFile("test2");
        cout<<"openID: "<<fileID2<<endl;

        // print old record
        recorder.printFile(fileID1, example);
        recorder.printFile(fileID2, example);

        Record* record1 = new Record();
        record1->append(true, 1);
        record1->append(true, float(5.0));
        record1->append(true, "world", 10);

        Record* record2 = new Record();
        record2->append(true, 2);
        record2->append(false, float(6.0));
        record2->append(true, "龙傲天", 10);

        // insert
        recorder.insertRecord(fileID1, record1);
        recorder.insertRecord(fileID2, record2);
        recorder.printFile(fileID1, example);
        recorder.printFile(fileID2, example);

        // select
        Condition* cond_float5 = new CompareCondition(EQ, 1, 5);
        vector<Record*> recordList = recorder.selectRecords(fileID1, cond_float5, example);
        cout<<endl<<"list select: "<<endl;
        for (int i = 0; i < recordList.size(); i++){
            recordList[i]->print();
        }
        vector<Record*> recordList2 = recorder.selectRecords(fileID2, cond_float5, example);
        cout<<endl<<"list select: "<<endl;
        for (int i = 0; i < recordList2.size(); i++){
            recordList2[i]->print();
        }
        recorder.freeRecordList(recordList);
        recorder.freeRecordList(recordList2);

        for(int i=0; i<10;i++)
            recorder.insertRecord(fileID1, record1);
        recorder.printFile(fileID1, example);

        // close table 1
        result = recorder.closeTableFile(fileID1);
        cout<<"close: "<<result<<endl;

        // insert
        recorder.insertRecord(fileID2, record2);
        recorder.printFile(fileID2, example);

        // open table 1
        fileID1 = recorder.openTableFile("test1");
        cout<<"openID: "<<fileID1<<endl;
        recorder.printFile(fileID1, example);

        // close table 2
        result = recorder.closeTableFile(fileID2);
        cout<<"close: "<<result<<endl;
        result = recorder.closeTableFile(fileID1);
        cout<<"close: "<<result<<endl;
    }
    static void test_batch_insert(){
        // test for large scale insert, delete update and select
        RecordManager recorder = RecordManager();

        // test open table
        int fileID = recorder.openTableFile("test");
        cout<<"openID: "<<fileID<<endl;

        // test record 
        Record* example = new Record();
        example->append(true, 1);
        example->append(true, float(2.0));
        example->append(true, "hello", 5);

        // print old record
        cout<<"old"<<endl;
        recorder.printFile(fileID, example);


        Record* record1 = new Record();
        record1->append(true, 1);
        record1->append(true, float(5.0));
        record1->append(true, "world", 5);

        Record* record2 = new Record();
        record2->append(true, 2);
        record2->append(false, float(6.0));
        record2->append(true, "haha", 5);

        Record* record3 = new Record();
        record3->append(true, 3);
        record3->append(true, float(5.0));
        record3->append(true, "abcdef", 5);

        Record* record4 = new Record();
        record4->append(true, 4);
        record4->append(true, float(4.0));
        record4->append(true, "张", 5);

        bool result;
        for(int i=0; i<1000; i++){
            recorder.insertRecord(fileID, record1);
        }

        for(int i=0; i<1000; i++){
            recorder.insertRecord(fileID, record2);
        }

        cout<<"insert"<<endl;
        recorder.printFile(fileID, example, false);

        for(int i=0; i<400; i++){
            result = recorder.deleteRecord(fileID, RID(1, i));
            result = recorder.deleteRecord(fileID, RID(2, i));
        }

        cout<<"delete"<<endl;
        recorder.printFile(fileID, example, false);

        for(int i=0; i<500; i++){
            recorder.insertRecord(fileID, record3);
        }

        cout<<"insert"<<endl;
        recorder.printFile(fileID, example, false);

        for(int i=10; i<500; i++){
            result = recorder.deleteRecord(fileID, RID(2, i));
        }

        cout<<"delete"<<endl;
        recorder.printPage(fileID, 2, example, true);

        for(int i=0; i<15; i++){
            result = recorder.updateRecord(fileID, RID(2, i), record4);
        }

        cout<<"update"<<endl;
        recorder.printPage(fileID, 2, example, true);

        // close table 1
        result = recorder.closeTableFile(fileID);
        return;
    }
};

#endif