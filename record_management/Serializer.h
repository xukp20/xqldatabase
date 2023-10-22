// 
// xukp: map Record to Byte or opposite
//

#ifndef SERIALIZER_H
#define SERIALIZER_H
#include "Record.h"
#include "../system_management/TableMeta.h"
#include "../system_management/TableIndex.h"
#include "../system_management/DBIndex.h"

#define DB_NAME_WIDTH 128

class Serializer {
public:
    // serialize a record into bytes
    // returns the length of the bytes
    static unsigned int serialize_record(Record* record, char* bytes);

    // fills in the blank contents of the target
    static void deserialize_record(char* bytes, Record* target);

    // serialize meta data of a table into bytes
    static bool serialize_table_meta(TableMeta* table_meta, char* bytes);

    // deserialize meta data of a table from bytes
    static void deserialize_table_meta(char* bytes, TableMeta* target);

    // serialize table indexes of a database into bytes
    static bool serialize_table_indexes(TableIndex* table_indexes, char* bytes);

    // deserialize table indexes of a database from bytes
    static void deserialize_table_indexes(char* bytes, TableIndex* target);

    // serialize db indexes into bytes
    static bool serialize_db_indexes(DBIndex* db_indexes, char* bytes);

    // deserialize db indexes from bytes
    static void deserialize_db_indexes(char* bytes, DBIndex* target);
};


#endif