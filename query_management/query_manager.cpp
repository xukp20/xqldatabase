#include "query_manager.h"
#include "limits.h"
#include "iomanip"
#include <cstring>
#include <fstream>
#include <stdlib.h>

// #define DEBUG 1

QueryManager::QueryManager(RecordManager* record_manager, SystemManager* system_manager, FileManager* index_fm, BufPageManager* index_bpm) {
    this->record_manager = record_manager;
    this->system_manager = system_manager;
    this->index_fm = index_fm;
    this->index_bpm = index_bpm;
}

QueryManager::~QueryManager() {
}

// insert
bool QueryManager::insert(InsertStatement* statement) {
    string table_name = statement->table_name;
    vector<ValueList*> value_lists = statement->value_lists->value_lists;

    // get the table meta from the index file
    TableMeta* table_meta = new TableMeta();
    if (!system_manager->getTableMeta(table_meta, table_name)) {
        cout << "Error: cannot get the table meta of " << table_name << endl;
        delete table_meta;
        return false;
    }
#ifdef DEBUG
    table_meta->print();
#endif

    // get the table id
    TableIndex* table_index = new TableIndex();
    if (!system_manager->getTableIndex(table_index)) {
        cout << "Error: cannot get the table index" << endl;
        delete table_index;
        delete table_meta;
        return false;
    }
    unsigned int table_id = table_index->getIndex(table_name);
    if (table_id == 0) {
        cout << "Error: cannot find the table " << table_name << endl;
        delete table_index;
        delete table_meta;
        return false;
    }
    delete table_index;

    // open the table file
    int fileId = record_manager->openTableFile(table_name.c_str());
    if (fileId == -1) {
        cout << "Error: cannot open the table " << table_name << endl;
        delete table_meta;
        return false;
    }

    // get the index map
    IndexMap* index_map = new IndexMap();
    if (!system_manager->getIndexMap(index_map)) {
        cout << "Error: cannot get the index map" << endl;
        // close file
        if (!record_manager->closeTableFile(fileId)) {
            cout << "Error: cannot close the table " << table_name << endl;
        }
        delete index_map;
        delete table_meta;
        return false;
    }
    vector<IndexMeta> indexes = index_map->getIndexMetas(table_id);
    // open and save all the index trees
    vector<Tree*> index_trees;
    for (int i = 0; i < indexes.size(); i++) {
        Tree* tree = new Tree(indexes[i].index_name + "_idx", index_fm, index_bpm);
        tree->initTree();
        index_trees.push_back(tree);
    }
    delete index_map;

    // go through the records
    for(int i = 0; i < value_lists.size(); i++) {
        // assemble the record
        Record* record = new Record();
        if (!assembleRecord(table_meta, value_lists[i]->values, record)) {
            // close the table file
            if (!record_manager->closeTableFile(fileId)) {
                cout << "Error: cannot close the table " << table_name << endl;
            }
            // remove the index trees
            for (int j = 0; j < index_trees.size(); j++) {
                // auto save
                delete index_trees[j];
            }
            delete record;
            delete table_meta;
            return false;
        }

        // insert the record
        // record->print();
        RID rid = record_manager->insertRecord(fileId, record);
        if (rid.pageIdx == 0 && rid.slotIdx == 0) {
            // close the table file
            if (!record_manager->closeTableFile(fileId)) {
                cout << "Error: cannot close the table " << table_name << endl;
            }
            // remove the index trees
            for (int j = 0; j < index_trees.size(); j++) {
                // auto save
                delete index_trees[j];
            }
            delete record;
            delete table_meta;
            return false;
        }
        // cout<<"insert to page "<<rid.pageIdx<<" slot "<<rid.slotIdx<<endl;
        delete record;

        // insert the index if there is one
        for (int j = 0; j < index_trees.size(); j++) {
            // get the key
            vector<unsigned int> key_cols = indexes[j].column_idx;
            vector<int> vals;
            for (int k = 0; k < key_cols.size(); k++) {
                // cout<<"key col "<<key_cols[k]<<endl;
                vals.push_back(value_lists[i]->values[key_cols[k]]->int_value);
            }
            Key key(vals);
            // insert to the index tree
            bool result = index_trees[j]->insertIndex(Index(key, rid));
            if (!result) {
                cout << "Error: cannot insert the index" << endl;
                delete table_meta;
                // close the table file
                if (!record_manager->closeTableFile(fileId)) {
                    cout << "Error: cannot close the table " << table_name << endl;
                }
                // remove the index trees
                for (int j = 0; j < index_trees.size(); j++) {
                    // auto save
                    delete index_trees[j];
                }
                return false;
            }
        }    
    }

    // close the table file
    if (!record_manager->closeTableFile(fileId)) {
        cout << "Error: cannot close the table " << table_name << endl;
        delete table_meta;
        // remove the index trees
        for (int j = 0; j < index_trees.size(); j++) {
            // auto save
            delete index_trees[j];
        }
        return false;
    }

    // close and delete the index trees
    for (int i = 0; i < index_trees.size(); i++) {
        // auto save
        delete index_trees[i];
    }
    delete table_meta;

    // print insert count
    cout << "Query OK, " << value_lists.size() << " row(s) affected";

    // renew the fm and bpm
    this->record_manager->closeAll();
    delete this->record_manager->fileManager;
    delete this->record_manager->bufPageManager;
    this->record_manager->fileManager = new FileManager();
    this->record_manager->bufPageManager = new BufPageManager(this->record_manager->fileManager);
    delete this->index_fm;
    delete this->index_bpm;
    this->index_fm = new FileManager();
    this->index_bpm = new BufPageManager(this->index_fm);

    return true;
}

bool QueryManager::assembleRecord(const TableMeta* table_meta, const vector<Value*>& values, Record* record) {
    if (table_meta->column_num != values.size()) {
        cout << "Error: the number of values does not match the table" << endl;
        return false;
    }

    const vector<ColumnMeta*>& columns = table_meta->columns;
    for (int i = 0; i < values.size(); i++) {
        if (values[i]->value_type == columns[i]->kind) {
            switch (columns[i]->kind) {
                case 0:
                    record->append(true, values[i]->int_value);
                    break;
                case 1:
                    record->append(true, values[i]->float_value);
                    break;
                case 2:
                    {
                        int max_len = columns[i]->len;
                        int len = strlen(values[i]->string_value.c_str());
                        if (len > max_len) {
                            cout << "Error: the length of string is too long" << endl;
                            return false;
                        }
                        record->append(true, values[i]->string_value.c_str(), max_len);
                        break;
                    }
                default:
                    cout << "Error: unknown type" << endl;
                    return false;
            }
        } else if (values[i]->value_type == Value::NULLVAL) {   // insert a NULL
            if (columns[i]->notNull) {
                cout << "Error: the value cannot be null" << endl;
                return false;
            } else if (columns[i]->defaultValue != nullptr){
                switch (columns[i]->kind)
                {
                    case 0:
                        {
                            DefaultIntConstraint* defaultIntConstraint = (DefaultIntConstraint*)columns[i]->defaultValue;
                            record->append(true, defaultIntConstraint->integer);
                            break;
                        }
                    case 1:
                        {
                            DefaultFloatConstraint* defaultFloatConstraint = (DefaultFloatConstraint*)columns[i]->defaultValue;
                            record->append(true, defaultFloatConstraint->floating);
                            break;
                        }
                    case 2:
                        {
                            DefaultCharConstraint* defaultCharConstraint = (DefaultCharConstraint*)columns[i]->defaultValue;
                            record->append(true, defaultCharConstraint->str, columns[i]->len);
                            break;
                        }
                    default:
                        cout << "Error: unknown type" << endl;
                        return false;
                }
            } else {
                switch (columns[i]->kind) {
                    case 0:
                        record->append(false, 0);
                        break;
                    case 1:
                        record->append(false, float(0.0));
                        break;
                    case 2:
                        record->append(false, "", columns[i]->len);
                        break;
                    default:
                        cout << "Error: unknown type" << endl;
                        return false;
                }
            }
        } else {
            cout << "Error: the type of value does not match the table" << endl;
            return false;
        }
    }

    if(!checkPrimary(table_meta, record)){
        return false;
    }
    if(!checkForeign(table_meta, record)){
        return false;
    }
    return true;
}


bool QueryManager::checkPrimary(const TableMeta* table_meta, const Record* record){
    // search for the value in the primary index

    if(table_meta->primaryKey == nullptr){
        return true;
    }
    // get index map
    IndexMap* index_map = new IndexMap();
    if (!system_manager->getIndexMap(index_map)) {
        cout << "Error: cannot get the index map" << endl;
        delete index_map;
        return false;
    }
    // get the primary index
    IndexMeta primary_index = index_map->getIndexMeta(table_meta->name + "_pri");
    if(primary_index.index_name == ""){
        delete index_map;
        return false;
    }

    // get the index tree
    Tree* index_tree = new Tree(primary_index.index_name + "_idx", index_fm, index_bpm);
    bool result = index_tree->initTree();
    if (!result) {
        cout << "Error: cannot init the index tree" << endl;
        delete index_tree;
        delete index_map;
        return false;
    }

    // search for the value
    // get the columns in the index
    vector<unsigned int> column_idx = primary_index.column_idx;
    vector<int> values;
    for(int i = 0; i < column_idx.size(); i++){
        // check not null
        if(record->bitmap[column_idx[i]] == 0){
            cout << "Error: the primary key cannot be null" << endl;
            delete index_tree;
            delete index_map;
            return false;
        }
        int value = ((Int_Item*)record->items[column_idx[i]])->integer;
        values.push_back(value);
    }
    Key key = Key(values);
#ifdef DEBUG
    cout << "search for the key: " << key.vals[0] << key.vals[1] << endl;
#endif


    // search for the key
    Index index = index_tree->searchIndex(key);

    // compare the value
    if(index.key == key && !(index.rid.pageIdx == 0 && index.rid.slotIdx == 0)){
        cout << "Error: the primary key already exists" << endl;
        delete index_tree;
        delete index_map;
        return false;
    }

    return true;
}

bool QueryManager::checkForeign(const TableMeta* table_meta, const Record* record){
    // get the foreign keys
#ifdef DEBUG
    cout << "check foreign keys" << endl;
#endif
    vector<ForeignKeyConstraint*> foreign_keys = table_meta->foreignKeys;
    if(foreign_keys.size() == 0){
#ifdef DEBUG
    cout << "no foreign keys" << endl;
#endif
        return true;
    }

    // get index map
    IndexMap* index_map = new IndexMap();
    if (!system_manager->getIndexMap(index_map)) {
        cout << "Error: cannot get the index map" << endl;
        delete index_map;
        return false;
    }
    // get table index
    TableIndex* table_index = new TableIndex();
    if (!system_manager->getTableIndex(table_index)) {
        cout << "Error: cannot get the table index" << endl;
        delete table_index;
        delete index_map;
        return false;
    }

    // get the index tree of the primary keys of the target tables
    for(int i = 0; i < foreign_keys.size(); i++){
        unsigned int target_table_id = foreign_keys[i]->target_table;
        string target_table_name = table_index->indexToTable[target_table_id];
#ifdef DEBUG
        cout << "target table name: " << target_table_name << endl;
#endif
        
        // target must be a primary key
        IndexMeta target_index = index_map->getIndexMeta(target_table_name + "_pri");

        // get the index tree
        Tree* index_tree = new Tree(target_index.index_name + "_idx", index_fm, index_bpm);
        bool result = index_tree->initTree();
        if (!result) {
            cout << "Error: cannot init the index tree" << endl;
            delete index_tree;
            delete index_map;
            delete table_index;
            return false;
        }

        // assemble the key
        vector<unsigned int> column_idx = foreign_keys[i]->source_columns;
        vector<int> values;
        for(int j = 0; j < column_idx.size(); j++){
            // check not null
            if(record->bitmap[column_idx[j]] == 0){
                cout << "Error: the foreign key cannot be null" << endl;
                delete index_tree;
                delete index_map;
                delete table_index;
                return false;
            }
            int value = ((Int_Item*)record->items[column_idx[j]])->integer;
            values.push_back(value);
        }

        Key key = Key(values);

        // search for the key
        Index index = index_tree->searchIndex(key);
#ifdef DEBUG
    cout << "search for the key" << endl;
    cout << index.key.vals[0] << endl;
    cout << index.key.vals[1] << endl;
    cout << index.rid.pageIdx << endl;
    cout << index.rid.slotIdx << endl;
    cout << key.vals[0] << endl;
    cout << key.vals[1] << endl;
#endif

        // compare the value
        if(!(index.key == key) || (index.rid.pageIdx == 0 && index.rid.slotIdx == 0)){
            cout << "Error: the foreign key does not exist" << endl;
            delete index_tree;
            delete index_map;
            delete table_index;
            return false;
        }
    }
    return true;
}

// select 
bool QueryManager::select(SelectStatement* statement) {
    // cout<<"1"<<endl;
#ifdef DEBUG
    cout << "select" << endl;
#endif
    if(statement->identifiers->identifiers.size() > 2){
        cout << "Error: too many identifiers" << endl;
        return false;
    }
    // no group by
    if(statement->hasGroupBy){
        cout << "Error: no group by" << endl;
        return false;
    }

    // get the tables' name
    // cout<<"2"<<endl;
    vector<string> table_names;
    for(int i=0; i<statement->identifiers->identifiers.size(); i++){
        table_names.push_back(statement->identifiers->identifiers[i]);
    }
    // check table exist
    TableIndex* table_index = new TableIndex();
    if (!system_manager->getTableIndex(table_index)) {
        cout << "Error: cannot get the table index" << endl;
        delete table_index;
        return false;
    }
    // cout<<"b"<<endl;

    vector<unsigned int> table_ids;
    for(int i=0; i<table_names.size(); i++){
        unsigned int table_id = table_index->getIndex(table_names[i]);
        if(table_id == 0){
            cout << "Error: table " << table_names[i] << " does not exist" << endl;
            delete table_index;
            return false;
        }
        table_ids.push_back(table_id);
    }

#ifdef DEBUG
    cout << 1 << endl;
#endif

    vector<TableMeta*> table_metas;
    for(int i=0; i<table_ids.size(); i++){
        TableMeta* table_meta = new TableMeta();
        if (!system_manager->getTableMeta(table_meta, table_names[i])) {
            cout << "Error: cannot get the table meta" << endl;
            delete table_index;
            delete table_meta;
            return false;
        }
        table_metas.push_back(table_meta);
    }
#ifdef DEBUG
    cout << 2 << endl;
#endif
    // cout<<"2"<<endl;
    // check selectors
    bool all = statement->selectors->all;
    vector<pair<int, int>> target_columns;
    vector<pair<string, string>> target_columns_name;
    // no aggregate function
    if(all){
        for(int i=0; i<table_metas.size(); i++){
            for(int j=0; j<table_metas[i]->column_num; j++){
                target_columns.push_back(make_pair(i, j));
                target_columns_name.push_back(make_pair(table_metas[i]->name, table_metas[i]->columns[j]->name));
            }
        }
        // cout<<"3"<<endl;
    } else {
        vector<Selector*>& selectors = statement->selectors->selectors;
        for(int i=0; i<selectors.size(); i++){
            if(selectors[i]->selector_type == Selector::AGGREGATOR || selectors[i]->selector_type == Selector::COUNT){
                cout << "Error: no aggregate function implemented" << endl;
                delete table_index;
                for(int i=0; i<table_metas.size(); i++){
                    delete table_metas[i];
                }
                return false;
            } 
            // check column exist and not conflict
            else if(selectors[i]->selector_type == Selector::COLUMN){
                string table_name = ((ColumnSelector*)selectors[i])->column->table_name;
                string column_name = ((ColumnSelector*)selectors[i])->column->column_name;
                int table_id = -1;
                int column_id = -1;
                if(table_name == ""){ // look for the column in all tables
                    bool find = false;
                    bool conflict = false;
                    for(int j=0; j<table_metas.size(); j++){
                        for(int k=0; k<table_metas[j]->column_num; k++){
                            if(table_metas[j]->columns[k]->name == column_name){
                                if(find){
                                    conflict = true;
                                    break;
                                }
                                find = true;
                                table_id = j;
                                column_id = k;
                            }
                        }
                        if(conflict){
                            delete table_index;
                            for(int i=0; i<table_metas.size(); i++){
                                delete table_metas[i];
                            }
                            cout << "Error: column " << column_name << " is ambiguous" << endl;
                            return false;
                        }
                    } 
                    if(!find){
                        delete table_index;
                        for(int i=0; i<table_metas.size(); i++){
                            delete table_metas[i];
                        }
                        cout << "Error: column " << column_name << " does not exist" << endl;
                        return false;
                    }
                } else {
                    for(int j=0; j<table_metas.size(); j++){
                        if(table_metas[j]->name == table_name){
                            table_id = j;
                            break;
                        }
                    }
                    if(table_id == -1){
                        cout << "Error: table " << table_name << " does not exist" << endl;
                        delete table_index;
                        for(int i=0; i<table_metas.size(); i++){
                            delete table_metas[i];
                        }
                        return false;
                    }
                    for(int j=0; j<table_metas[table_id]->column_num; j++){
                        if(table_metas[table_id]->columns[j]->name == column_name){
                            column_id = j;
                            break;
                        }
                    }
                    if(column_id == -1){
                        cout << "Error: column " << column_name << " does not exist" << endl;
                        delete table_index;
                        for(int i=0; i<table_metas.size(); i++){
                            delete table_metas[i];
                        }
                        return false;
                    }
                }
                target_columns.push_back(make_pair(table_id, column_id));
                target_columns_name.push_back(make_pair(table_name, column_name));
            }
        }
    }
    

    // one table
    if(table_ids.size() == 1){
        // get table meta
        TableMeta* table_meta = table_metas[0];
        // open table
        int fileId = record_manager->openTableFile(table_meta->name.c_str());
        if(fileId == -1){
            delete table_index;
            for(int i=0; i<table_metas.size(); i++){
                delete table_metas[i];
            }
            return false;
        }
        // cout<<"5"<<endl;

        // assemble conditions according to the where clause
        vector<WhereClause*> where_clauses;
        vector<Condition*> conditions;
        if(statement->where_and_clause){
            where_clauses = statement->where_and_clause->where_clauses;
            if(!assembleSingleConditions(table_meta, where_clauses, conditions)){
                // close
                record_manager->closeTableFile(fileId);
                delete table_index;
                for(int i=0; i<table_metas.size(); i++){
                    delete table_metas[i];
                }
                return false;
            }
            // cout<<"6"<<endl;
        }

        // if the where clause can be indexes
        IndexMap* index_map = new IndexMap();
        if(!system_manager->getIndexMap(index_map)){
            // close
            record_manager->closeTableFile(fileId);
            delete table_index;
            for(int i=0; i<table_metas.size(); i++){
                delete table_metas[i];
            }
            delete index_map;
            return false;
        }

        int condition_id1, condition_id2;
        int index_id;
        unsigned int table_id = table_ids[0];
        index_id = fetchSelectIndex(table_id, index_map, conditions, condition_id1, condition_id2);
        
        // select result
        vector<Record*> records;

        // find a index and no limit or offset
        if(index_id != -1 && !statement->hasLimit && !statement->hasOffset){
#ifdef DEBUG
            cout << "find index" << endl;
#endif
            IndexMeta index_meta = index_map->index_map[index_id];
            unsigned int index_size = 0;
            if(condition_id2 == -1){
                index_size = 1;
            } else {
                index_size = 2;
            }

            // get the selected conditions
            CompareCondition* condition1 = (CompareCondition*)conditions[condition_id1];

            // get the upper and lower bound
            // index tree will get [lower, upper)
            int lower, upper;
            switch(condition1->kind){
                case Condition::EQ:
                    lower = condition1->bound;
                    upper = condition1->bound + 1;
                    break;
                case Condition::LT:
                    lower = INT_MIN;
                    upper = condition1->bound;
                    break;
                case Condition::LE:
                    lower = INT_MIN;
                    upper = condition1->bound + 1;
                    break;
                case Condition::GT:
                    lower = condition1->bound + 1;
                    upper = INT_MAX;
                    break;
                case Condition::GE:
                    lower = condition1->bound;
                    upper = INT_MAX;
                    break;
                default:
                    lower = 0;
                    upper = 0;
            }

            // assemble the key of the bounds
            Key key1;
            Key key2;
            if(index_size == 1){
                key1 = Key(lower);
                key2 = Key(upper);
            } else {
                // if two dimensions, needs to use the first to select index 
                // then use the second to select records from the index's
                vector<int> key1_vals;
                vector<int> key2_vals;
                key1_vals.push_back(lower);
                key1_vals.push_back(INT_MIN);
                key2_vals.push_back(upper);
                key2_vals.push_back(INT_MIN);
                key1 = Key(key1_vals);
                key2 = Key(key2_vals);
            }
#ifdef DEBUG
            key1.print();
            key2.print();
#endif

            // get the index tree
            Tree* tree = new Tree(index_meta.index_name + "_idx", index_fm, index_bpm);
            bool result = tree->initTree();
            if(!result){
                // close
                record_manager->closeTableFile(fileId);
                delete table_index;
                for(int i=0; i<table_metas.size(); i++){
                    delete table_metas[i];
                }
                delete index_map;
                delete tree;
                return false;
            }

            // get the indexes
            list<Index> positions = tree->searchRange(key1, key2);
            vector<RID> rids;

#ifdef DEBUG
            cout << "Search in tree: " << endl;
            tree->print();
            cout << "positions size: " << positions.size() << endl;
            for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
                it->print();
            }
#endif


            // if two demensions, need to select records from the index's
            if(index_size == 2){
                // get the second condition
                CompareCondition* condition2 = (CompareCondition*)conditions[condition_id2];
                
                for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
                    if(condition2->compare(it->key.vals[1])){
                        rids.push_back(it->rid);
                    }
                }
            } else {
                // all
                for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
                    rids.push_back(it->rid);
                }
            }

#ifdef DEBUG
            cout << "rids size: " << rids.size() << endl;
            for(int i=0; i<rids.size(); i++){
                rids[i].print();
            }
#endif

            // select from the record file
            Record example_record = table_meta->getExampleRecord();
            records = record_manager->selectRecords(fileId, conditions, rids, &example_record);

#ifdef DEBUG
            cout << "records size: " << records.size() << endl;
            for(int i=0; i<records.size(); i++){
                records[i]->print();
            }
#endif
            // delete
            delete tree;
#ifdef DEBUG
            cout<<"end"<<endl;
#endif
        
        } else {
            // no index can be used, or need to use limit or offset
            // select all the records according to the conditions;
            Record example_record = table_meta->getExampleRecord();
            int limit = -1;
            int offset = 0;
            if(statement->hasLimit){
                limit = statement->limit;
            } 
            if(statement->hasOffset){
                offset = statement->offset;
            }

            records = record_manager->selectRecordsBlock(fileId, conditions, &example_record, limit, offset);
        }

#ifdef DEBUG
        cout << "records size: " << records.size() << endl;
        for(int i=0; i<records.size(); i++){
            records[i]->print();
        }
#endif
        // print
        // get the len of each column
        vector<int> column_lens;
        for(int i=0; i<target_columns_name.size(); i++){
            int len = 0;    // get the larger one in the column name or the value
            switch (table_meta->columns[target_columns[i].second]->kind)
            {
                case 0:
                    len = 10 + 2;   // 10 for int
                    break;
                case 1:
                    len = 15 + 2;   // 15 for float
                    break;
                case 2:
                    len = table_meta->columns[target_columns[i].second]->len + 2;
                    break;
                default:
                    break;
            }
            int column_name_len = target_columns_name[i].second.length() + 2;
            if(len < column_name_len){
                len = column_name_len;
            }
            column_lens.push_back(len);
        }

        // print header
        cout<<"+";
        for(int i=0; i<target_columns_name.size(); i++){
            cout<<setw(column_lens[i])<<setfill('-')<<"";
            cout<<"+";
        }
        cout<<endl;

        cout<<"|";
        for(int i=0; i<target_columns_name.size(); i++){
            cout<<" "<<setw(column_lens[i] - 2)<<setfill(' ')<<left<<target_columns_name[i].second;
            cout<<" |";
        }
        cout<<endl;

        cout<<"+";
        for(int i=0; i<target_columns_name.size(); i++){
            cout<<setw(column_lens[i])<<setfill('-')<<"";
            cout<<"+";
        }
        cout<<endl;
        
        // print the records
        for(int i=0; i<records.size(); i++){
            Record* record = records[i];
            cout<<"|";
            for(int i=0; i<target_columns.size(); i++){
                int table_id = target_columns[i].first; // no use here
                int column_id = target_columns[i].second;
                // cout<<"column_id: "<<column_id<<endl;
                if(!record->bitmap[column_id]){
                    cout <<" NULL" <<setw(column_lens[i]-6)<<setfill(' ')<<""<<" |";
                } else {
                    switch(record->items[column_id]->kind)
                    {
                        case 0:{
                            string i_str = to_string(((Int_Item*)(record->items[column_id]))->integer);
                            cout <<" "<< i_str << setw(column_lens[i]- 2 - i_str.length())<<setfill(' ')<<""<<" |";
                            break;
                        }
                        case 1:{
                            string f_str = to_string(((Float_Item*)(record->items[column_id]))->floating);
                            cout <<" "<< f_str << setw(column_lens[i]- 2 - f_str.length())<<setfill(' ')<<""<<" |";
                            break;
                        }
                        case 2:
                        {   
                            int len = ((Char_Item*)record->items[column_id])->len;
                            cout<<" ";
                            for(int j=0; j<len; j++){
                                if(((Char_Item*)(record->items[column_id]))->str[j] == 0)
                                    cout<<" ";
                                else
                                    cout<<((Char_Item*)(record->items[column_id]))->str[j];
                            }
                            cout <<setw(column_lens[i]- 2 - len)<<setfill(' ')<<""<<" |";
                            break;
                        }
                        default:
                            cout << "Error: unknown type" << endl;
                            break;
                    }
                }
            }
            cout << endl;
        }
        cout<<"+";
        for(int i=0; i<target_columns_name.size(); i++){
            cout<<setw(column_lens[i])<<setfill('-')<<"";
            cout<<"+";
        }
        cout<<endl;
        // print count
        cout << records.size() << " rows in set";

        record_manager->freeRecordList(records);
        // done
        record_manager->closeTableFile(fileId);
        record_manager->closeAll();
        // free
        for(int i=0; i<conditions.size(); i++){
            delete conditions[i];
        }
        delete table_index;
        for(int i=0; i<table_metas.size(); i++){
            delete table_metas[i];
        }

        // renew the fm and bpm
        this->record_manager->closeAll();
        delete this->record_manager->fileManager;
        delete this->record_manager->bufPageManager;
        this->record_manager->fileManager = new FileManager();
        this->record_manager->bufPageManager = new BufPageManager(this->record_manager->fileManager);
        delete this->index_fm;
        delete this->index_bpm;
        this->index_fm = new FileManager();
        this->index_bpm = new BufPageManager(this->index_fm);

        return true;

    } else if (table_ids.size() == 2) {
        // two tables
        TableMeta* table_meta1 = table_metas[0];
        TableMeta* table_meta2 = table_metas[1];
        // open table
        int fileId1 = record_manager->openTableFile(table_meta1->name.c_str());
        if(fileId1 == -1){
            delete table_index;
            for(int i=0; i<table_metas.size(); i++){
                delete table_metas[i];
            }
            return false;
        }
        int fileId2 = record_manager->openTableFile(table_meta2->name.c_str());
        if(fileId2 == -1){
            // close 
            record_manager->closeTableFile(fileId1);
            delete table_index;
            for(int i=0; i<table_metas.size(); i++){
                delete table_metas[i];
            }
            return false;
        }
        // cout<<"5"<<endl;
#ifdef DEBUG
    cout << 3 << endl;
#endif

        vector<WhereClause*> where_clauses;
        vector<Condition*> conditions1, conditions2, cross_conditions;
        if(statement->where_and_clause){
            where_clauses = statement->where_and_clause->where_clauses;
            if(!assembleJoinConditions(table_meta1, table_meta2, where_clauses, conditions1, conditions2, cross_conditions)){
                record_manager->closeTableFile(fileId1);
                record_manager->closeTableFile(fileId2);
                delete table_index;
                for(int i=0; i<table_metas.size(); i++){
                    delete table_metas[i];
                }
                return false;
            }
        }

        // select all the records according to the conditions;
        Record example_record1 = table_meta1->getExampleRecord();
        Record example_record2 = table_meta2->getExampleRecord();
        vector<Record*> records1 = record_manager->selectRecords(fileId1, conditions1, &example_record1);
        vector<Record*> records2 = record_manager->selectRecords(fileId2, conditions2, &example_record2);

        // cross join to get the final result
        vector<pair<Record*, Record*>> results = joinRecords(records1, records2, cross_conditions);

        // print
        // get the len of each column
        vector<int> column_lens;
        for(int i=0; i<target_columns_name.size(); i++){
            int len = 0;    // get the larger one in the column name or the value
            TableMeta* table_meta = table_metas[target_columns[i].first];
            switch (table_meta->columns[target_columns[i].second]->kind)
            {
                case 0:
                    len = 10 + 2;   // 10 for int
                    break;
                case 1:
                    len = 15 + 2;   // 15 for float
                    break;
                case 2:
                    len = table_meta->columns[target_columns[i].second]->len + 2;
                    break;
                default:
                    break;
            }
            int column_name_len = target_columns_name[i].second.length() + target_columns_name[i].first.length() + 3;
            if(len < column_name_len){
                len = column_name_len;
            }
            column_lens.push_back(len);
        }

        // print header
        cout<<"+";
        for(int i=0; i<target_columns_name.size(); i++){
            cout<<setw(column_lens[i])<<setfill('-')<<"";
            cout<<"+";
        }
        cout<<endl;
        
        cout<<"|";
        for(int i=0; i<target_columns_name.size(); i++){
            string name = target_columns_name[i].first + "." + target_columns_name[i].second;
            cout<<" "<<setw(column_lens[i] - 2)<<setfill(' ')<<left<<name;
            cout<<" |";
        }
        cout<<endl;

        cout<<"+";
        for(int i=0; i<target_columns_name.size(); i++){
            cout<<setw(column_lens[i])<<setfill('-')<<"";
            cout<<"+";
        }
        cout<<endl;
        
        // print the result
        for(int i=0; i<results.size(); i++){
            Record* record1 = results[i].first;
            Record* record2 = results[i].second;
            cout<<"|";
            for(int i=0; i<target_columns.size(); i++){
                int table_id = target_columns[i].first;
                int column_id = target_columns[i].second;
                Record* record = table_id == 0 ? record1 : record2;
                
                if(!record->bitmap[column_id]){
                    cout <<" NULL" <<setw(column_lens[i]-6)<<setfill(' ')<<""<<" |";
                } else {
                    switch(record->items[column_id]->kind)
                    {
                        case 0:{
                            string i_str = to_string(((Int_Item*)(record->items[column_id]))->integer);
                            cout <<" "<< i_str << setw(column_lens[i]- 2 - i_str.length())<<setfill(' ')<<""<<" |";
                            break;
                        }
                        case 1:{
                            string f_str = to_string(((Float_Item*)(record->items[column_id]))->floating);
                            cout <<" "<< f_str << setw(column_lens[i]- 2 - f_str.length())<<setfill(' ')<<""<<" |";
                            break;
                        }
                        case 2:
                        {
                            int len = ((Char_Item*)(record->items[column_id]))->len;
                            cout<<" ";
                            for(int j=0; j<len; j++){
                                if(((Char_Item*)(record->items[column_id]))->str[j] == 0)
                                    cout<<" ";
                                else
                                    cout<<((Char_Item*)(record->items[column_id]))->str[j];
                            }
                            cout <<setw(column_lens[i]- 2 - len)<<setfill(' ')<<""<<" |";
                            break;
                        }
                        default:
                            cout << "Error: unknown type" << endl;
                            break;
                    }
                }
            }
            cout << endl;
        }
        // print count 
        cout<<"+";
        for(int i=0; i<target_columns_name.size(); i++){
            cout<<setw(column_lens[i])<<setfill('-')<<"";
            cout<<"+";
        }
        cout<<endl;
        cout<<results.size()<<" rows in set";

        // free
        record_manager->freeRecordList(records1);
        record_manager->freeRecordList(records2);
        for(int i=0; i<conditions1.size(); i++){
            delete conditions1[i];
        }
        for(int i=0; i<conditions2.size(); i++){
            delete conditions2[i];
        }
        for(int i=0; i<cross_conditions.size(); i++){
            delete cross_conditions[i];
        }
        record_manager->closeAll();
        delete table_index;
        for(int i=0; i<table_metas.size(); i++){
            delete table_metas[i];
        }

        // renew the fm and bpm
        this->record_manager->closeAll();
        delete this->record_manager->fileManager;
        delete this->record_manager->bufPageManager;
        this->record_manager->fileManager = new FileManager();
        this->record_manager->bufPageManager = new BufPageManager(this->record_manager->fileManager);
        delete this->index_fm;
        delete this->index_bpm;
        this->index_fm = new FileManager();
        this->index_bpm = new BufPageManager(this->index_fm);
        
        return true;
    } else {
        cout<<"Error: too many tables"<<endl;
        delete table_index;
        for(int i=0; i<table_metas.size(); i++){
            delete table_metas[i];
        }
        return false;
    }
    return false;
}

bool QueryManager::checkSingleRecord(const TableMeta* table_meta, Record* record, vector<Condition*>& conditions){
    return true;
}

int QueryManager::fetchSelectIndex(unsigned int table_id, IndexMap* index_map, vector<Condition*>& conditions, int& condition_id1, int& condition_id2){
    // go through the pair of conditions 
    // if two columns match a two column index, return the index id
    // or fetch a single column index
    // or return -1

#ifdef DEBUG
    cout << "fetchSelectIndex" << endl;
#endif

    // first select the conditions that is compare between single column and a value
    bool* type_right = new bool[conditions.size()];

    for(int i=0; i<conditions.size(); i++){
        if(conditions[i]->isSingleCompare()){
            type_right[i] = true;
        } else {
            type_right[i] = false;
        }
    }

    // get the indexes of this table from the index map
    vector<IndexMeta> indexes = index_map->getIndexMetas(table_id);

    for(int i=0; i<conditions.size(); i++){
        for(int j=0; j<conditions.size(); j++){
            if(i == j){
                continue;
            }
            if(type_right[i] && type_right[j]){
                // check if they are the same column
                if(conditions[i]->column == conditions[j]->column){
                    continue;
                }
                // not same column, check if they are in the same index
                unsigned int column_id1 = conditions[i]->column;
                unsigned int column_id2 = conditions[j]->column;
                for(int k=0; k<indexes.size(); k++){
                    if(indexes[k].column_idx.size() == 2){
                        if(indexes[k].column_idx[0] == column_id1 && indexes[k].column_idx[1] == column_id2){
                            condition_id1 = i;
                            condition_id2 = j;
                            delete[] type_right;
                            return index_map->getIndexId(indexes[k].index_name);
                        } else if(indexes[k].column_idx[0] == column_id2 && indexes[k].column_idx[1] == column_id1){
                            condition_id1 = j;
                            condition_id2 = i;
                            delete[] type_right;
                            return index_map->getIndexId(indexes[k].index_name);
                        }
                    }
                }
            }
        }
    }

    // check if there is a single column index
    for(int i=0; i<conditions.size(); i++){
        if(type_right[i]){
            unsigned int column_id = conditions[i]->column;
            for(int k=0; k<indexes.size(); k++){
                if(indexes[k].column_idx.size() == 1){
                    if(indexes[k].column_idx[0] == column_id){
                        condition_id1 = i;
                        condition_id2 = -1;
                        delete[] type_right;
                        return index_map->getIndexId(indexes[k].index_name);
                    }
                }
            }
        }
    }

    // not found
    delete[] type_right;
    return -1;
}

bool QueryManager::assembleSingleConditions(const TableMeta* table_meta, vector<WhereClause*>& where_clauses, vector<Condition*>& conditions){
    for(int i=0; i<where_clauses.size(); i++){
        WhereClause* where_clause = where_clauses[i];
        Condition* condition;

        // get the first column
        Column* column = where_clause->column;
        string table_name1 = column->table_name;

        // get it from the table_metas who has this column
        if(table_name1 == ""){
            table_name1 = table_meta->name;
        }
        string column_name1 = column->column_name;
        unsigned int column_id1 = -1;
        // check if exists
        bool check = true;
        if(table_meta->name != table_name1){
            cout<<"Error: table "<<table_name1<<" does not exist"<<endl;
            check = false;
        } else {
            bool find = false;
            for(int j=0; j<table_meta->column_num; j++){
                if(table_meta->columns[j]->name == column_name1){
                    find = true;
                    column_id1 = j;
                    break;
                }
            }
            if(!find){
                cout<<"Error: column "<<column_name1<<" does not exist"<<endl;
                check = false;
            }
        }
        if(!check){
            return false;
        }
        
        // condition
        switch (where_clause->type)
        {
            case WhereClause::OperatorExpr:
            {
                WhereOperatorExpr* where_operator_expr = (WhereOperatorExpr*)where_clause;
                Operator* op = where_operator_expr->op;
                Expression* expr = where_operator_expr->expression;

                // expr is the second column
                switch (expr->expression_type)
                {
                    case Expression::COLUMN:
                    {
                        Column* column = expr->column;
                        string table_name2 = column->table_name;
                        // get it from the table_metas who has this column
                        if(table_name2 == ""){
                            table_name2 = table_meta->name;
                        }
                        string column_name2 = column->column_name;

                        // check if the column exists
                        // only one table
                        unsigned int column_id2 = -1;
                        bool check = true;
                        if(table_meta->name != table_name2){
                            cout<<"Error: table "<<table_name2<<" does not exist"<<endl;
                            check = false;
                        } else {
                            bool find = false;
                            for(int j=0; j<table_meta->column_num; j++){
                                if(table_meta->columns[j]->name == column_name2){
                                    find = true;
                                    column_id2 = j;
                                    break;
                                }
                            }
                            if(!find){
                                cout<<"Error: column "<<column_name2<<" does not exist"<<endl;
                                check = false;
                            }
                        }
                        if(!check){
                            // clear conditions 
                            for(int i=0; i<conditions.size(); i++){
                                delete conditions[i];
                            }
                            return false;
                        }

                        // check if the two columns are the same type
                        if(table_meta->columns[column_id1]->kind != table_meta->columns[column_id2]->kind){
                            cout<<"Error: the two columns are not the same type"<<endl;
                            // clear conditions 
                            for(int i=0; i<conditions.size(); i++){
                                delete conditions[i];
                            }
                            return false;
                        }

                        // get the op
                        Condition::cond cond;
                        if(table_meta->columns[column_id1]->kind == 2){
                            switch (op->operator_type){
                                case Operator::EqualOrAssign:
                                    cond = Condition::EQS2;
                                    break;
                                case Operator::NotEqual:
                                    cond = Condition::NES2;
                                    break;
                                default:
                                    cout<<"Error: unknown operator"<<endl;
                                    // clear conditions 
                                    for(int i=0; i<conditions.size(); i++){
                                        delete conditions[i];
                                    }
                                    return false;
                            }

                            // assemble condition
                            condition = new CompareTwoStringCondition(cond, column_id1, column_id2);
                        } else {
                            switch (op->operator_type)
                            {
                                case Operator::EqualOrAssign:
                                    cond = Condition::EQ2;
                                    break;
                                case Operator::Less:
                                    cond = Condition::LT2;
                                    break;
                                case Operator::LessEqual:
                                    cond = Condition::LE2;
                                    break;
                                case Operator::Greater:
                                    cond = Condition::GT2;
                                    break;
                                case Operator::GreaterEqual:
                                    cond = Condition::GE2;
                                    break;
                                case Operator::NotEqual:
                                    cond = Condition::NE2;
                                    break;
                                default:
                                    cout<<"Error: unknown operator"<<endl;
                                    // clear conditions 
                                    for(int i=0; i<conditions.size(); i++){
                                        delete conditions[i];
                                    }
                                    return false;
                                    break;
                            }

                            // assemble condition
                            condition = new CompareTwoColumnCondition(cond, column_id1, column_id2);
                        }
                        

                        break;
                    }
                    case Expression::VALUE:
                    {
                        Value* value = expr->value;
                        // check if the value is the same type as the column
                        if(table_meta->columns[column_id1]->kind != value->value_type){
                            cout<<"Error: the value is not the same type as the column"<<endl;
                            // clear conditions 
                            for(int i=0; i<conditions.size(); i++){
                                delete conditions[i];
                            }
                            return false;
                        }

                        if(value->value_type == Value::STRING){
                            Condition::cond cond;
                            switch (op->operator_type)
                            {
                                case Operator::EqualOrAssign:
                                    cond = Condition::EQS;
                                    break;
                                case Operator::NotEqual:
                                    cond = Condition::NES;
                                    break;
                                default:
                                    cout<<"Error: unknown operator"<<endl;
                                    // clear conditions 
                                    for(int i=0; i<conditions.size(); i++){
                                        delete conditions[i];
                                    }
                                    return false;
                                    break;
                            }
                            string bound = value->string_value;
                            condition = new StringCompareCondition(cond, column_id1, bound);
                        } else {
                            // get the op
                            Condition::cond cond;
                            switch (op->operator_type)
                            {
                                case Operator::EqualOrAssign:
                                    cond = Condition::EQ;
                                    break;
                                case Operator::Less:
                                    cond = Condition::LT;
                                    break;
                                case Operator::LessEqual:
                                    cond = Condition::LE;
                                    break;
                                case Operator::Greater:
                                    cond = Condition::GT;
                                    break;
                                case Operator::GreaterEqual:
                                    cond = Condition::GE;
                                    break;
                                case Operator::NotEqual:
                                    cond = Condition::NE;
                                    break;
                                default:
                                    cout<<"Error: unknown operator"<<endl;
                                    // clear conditions 
                                    for(int i=0; i<conditions.size(); i++){
                                        delete conditions[i];
                                    }
                                    return false;
                                    break;
                            }

                            // assemble condition
                            double bound;
                            if(value->value_type == Value::FLOAT){
                                bound = value->float_value;
                            } else {
                                bound = value->int_value;
                            }
                            condition = new CompareCondition(cond, column_id1, bound);
                        }

                        break;
                    }
                    default:
                        cout<<"Error: unknown expression type"<<endl;
                        // clear conditions 
                        for(int i=0; i<conditions.size(); i++){
                            delete conditions[i];
                        }
                        return false;
                        break;
                }
                break;
            }

            case WhereClause::OperatorSelect:
            {
                cout<<"Error: cannot use select in where clause"<<endl;
                // clear conditions
                for(int i=0; i<conditions.size(); i++){
                    delete conditions[i];
                }
                return false;
            }

            case WhereClause::IsNull:
            {
                WhereIsNull* where_is_null = (WhereIsNull*)where_clause;
                bool is_null = where_is_null->is_null;
                Condition::cond cond;
                cond = is_null ? Condition::IS_NULL : Condition::IS_NOT_NULL;
                condition = new NullCondition(cond, column_id1);
                break;
            }

            case WhereClause::InList:
            {

                WhereInList* where_in_list = (WhereInList*)where_clause;
                vector<Value*> values = where_in_list->value_list->values;

                if(table_meta->columns[column_id1]->kind == 2){
                    vector<string> value_list;
                    for(int i=0; i<values.size(); i++){
                        if(values[i]->value_type != table_meta->columns[column_id1]->kind){
                            cout<<"Error: the value is not the same type as the column"<<endl;
                            // clear conditions
                            for(int i=0; i<conditions.size(); i++){
                                delete conditions[i];
                            }
                            return false;
                        }
                        value_list.push_back(values[i]->string_value);
                    }
                    
                    condition = new StrListCondition(Condition::IN, column_id1);
                    for(int i=0; i<value_list.size(); i++){
                        ((StrListCondition*)condition)->add(value_list[i]);
                    }
                } else {
                    vector<double> value_list;
                    for(int i=0; i<values.size(); i++){
                        if(values[i]->value_type != table_meta->columns[column_id1]->kind){
                            cout<<"Error: the value is not the same type as the column"<<endl;
                            // clear conditions
                            for(int i=0; i<conditions.size(); i++){
                                delete conditions[i];
                            }
                            return false;
                        }
                        switch(values[i]->value_type){
                            case Value::INT:
                                value_list.push_back((double)values[i]->int_value);
                                break;
                            case Value::FLOAT:
                                value_list.push_back((double)values[i]->float_value);
                                break;
                            default:
                                cout<<"Error: unknown value type"<<endl;
                                // clear conditions
                                for(int i=0; i<conditions.size(); i++){
                                    delete conditions[i];
                                }
                                return false;
                                break;
                        }
                    }

                    condition = new NumListCondition(Condition::IN, column_id1);
                    for(int i=0; i<value_list.size(); i++){
                        ((NumListCondition*)condition)->add(value_list[i]);
                    }
                }
                
                break;
            }

            case WhereClause::InSelect:
            {
                cout<<"Error: cannot use select in where clause"<<endl;
                // clear conditions
                for(int i=0; i<conditions.size(); i++){
                    delete conditions[i];
                }
                return false;
            }

            case WhereClause::Like:
            {
                WhereLike* where_like = (WhereLike*)where_clause;
                string pattern = where_like->pattern;
                condition = new StringCondition(Condition::LIKE, column_id1, pattern);
                break;
            }

            default:
            {   
                cout<<"Error: unknown where clause type"<<endl;
                // clear conditions
                for(int i=0; i<conditions.size(); i++){
                    delete conditions[i];
                }
                return false;
                break;
            }  
        }

        conditions.push_back(condition);
    }

    return true;
}

bool QueryManager::assembleJoinConditions(const TableMeta* table_meta1, const TableMeta* table_meta2, vector<WhereClause*>& where_clauses, vector<Condition*>& conditions1, vector<Condition*>& conditions2, vector<Condition*>& cross_condition){
    // divide the conditions into three parts 
    // conditions1: conditions on table1
    // conditions2: conditions on table2
    // cross_condition: conditions between table1 and table2
#ifdef DEBUG
    cout << "assembleJoinConditions" << endl;
#endif
    for(int i=0; i<where_clauses.size(); i++){
        WhereClause* where_clause = where_clauses[i];
        Condition* condition;

        // get the first column
        Column* column = where_clause->column;
        string table_name1 = column->table_name;
        string column_name1 = column->column_name;

        unsigned int column_id1;
        unsigned int table_id1;
        if(table_name1 == ""){
            // find which table the column belongs to
            int column_id1_1 = findColumnId(table_meta1, column_name1);
            int column_id1_2 = findColumnId(table_meta2, column_name1);
            if(column_id1_1 == -1 && column_id1_2 == -1){
                cout<<"Error: cannot find the column "<<column_name1<<endl;
                // clear conditions
                for(int i=0; i<conditions1.size(); i++){
                    delete conditions1[i];
                }
                for(int i=0; i<conditions2.size(); i++){
                    delete conditions2[i];
                }
                for(int i=0; i<cross_condition.size(); i++){
                    delete cross_condition[i];
                }
                return false;
            } else if(column_id1_1 != -1 && column_id1_2 != -1){
                cout<<"Error: ambiguous column name "<<column_name1<<endl;
                // clear conditions
                for(int i=0; i<conditions1.size(); i++){
                    delete conditions1[i];
                }
                for(int i=0; i<conditions2.size(); i++){
                    delete conditions2[i];
                }
                for(int i=0; i<cross_condition.size(); i++){
                    delete cross_condition[i];
                }
                return false;
            } else if(column_id1_1 != -1){
                table_name1 = table_meta1->name;
                column_id1 = column_id1_1;
                table_id1 = 1;
            } else {
                table_name1 = table_meta2->name;
                column_id1 = column_id1_2;
                table_id1 = 2;
            }
        } else {
            if(table_name1 == table_meta1->name){
                column_id1 = findColumnId(table_meta1, column_name1);
                if(column_id1 == -1){
                    cout<<"Error: cannot find the column "<<column_name1<<endl;
                    // clear conditions
                    for(int i=0; i<conditions1.size(); i++){
                        delete conditions1[i];
                    }
                    for(int i=0; i<conditions2.size(); i++){
                        delete conditions2[i];
                    }
                    for(int i=0; i<cross_condition.size(); i++){
                        delete cross_condition[i];
                    }
                    return false;
                }
                table_id1 = 1;
            } else if(table_name1 == table_meta2->name){
                column_id1 = findColumnId(table_meta2, column_name1);
                if(column_id1 == -1){
                    cout<<"Error: cannot find the column "<<column_name1<<endl;
                    // clear conditions
                    for(int i=0; i<conditions1.size(); i++){
                        delete conditions1[i];
                    }
                    for(int i=0; i<conditions2.size(); i++){
                        delete conditions2[i];
                    }
                    for(int i=0; i<cross_condition.size(); i++){
                        delete cross_condition[i];
                    }
                    return false;
                }
                table_id1 = 2;
            } else {
                cout<<"Error: cannot find the table "<<table_name1<<endl;
                // clear conditions
                for(int i=0; i<conditions1.size(); i++){
                    delete conditions1[i];
                }
                for(int i=0; i<conditions2.size(); i++){
                    delete conditions2[i];
                }
                for(int i=0; i<cross_condition.size(); i++){
                    delete cross_condition[i];
                }
                return false;
            }
        }

        // short for one column condition
        const TableMeta* table_meta = table_id1 == 1 ? table_meta1 : table_meta2;
        vector<Condition*>& conditions = (table_id1 == 1) ? conditions1 : conditions2;

        // condition
        switch (where_clause->type)
        {
            case WhereClause::OperatorExpr:
            {
                WhereOperatorExpr* where_operator_expr = (WhereOperatorExpr*)where_clause;
                Operator* op = where_operator_expr->op;
                Expression* expr = where_operator_expr->expression;

                // the second value
                switch (expr->expression_type)
                {
                    case Expression::COLUMN:
                    {
#ifdef DEBUG
    cout << 1 << endl;
#endif
                        Column* column = expr->column;
                        string table_name2 = column->table_name;
                        
                        // get the second column's table
                        unsigned int column_id2;
                        unsigned int table_id2;
                        if(table_name2 == ""){
                            int column_id2_1 = findColumnId(table_meta1, column->column_name);
                            int column_id2_2 = findColumnId(table_meta2, column->column_name);

                            if(column_id2_1 == -1 && column_id2_2 == -1){
                                cout<<"Error: cannot find the column "<<column->column_name<<endl;
                                // clear conditions
                                for(int i=0; i<conditions1.size(); i++){
                                    delete conditions1[i];
                                }
                                for(int i=0; i<conditions2.size(); i++){
                                    delete conditions2[i];
                                }
                                for(int i=0; i<cross_condition.size(); i++){
                                    delete cross_condition[i];
                                }
                                return false;
                            } else if(column_id2_1 != -1 && column_id2_2 != -1){
                                cout<<"Error: ambiguous column name "<<column->column_name<<endl;
                                // clear conditions
                                for(int i=0; i<conditions1.size(); i++){
                                    delete conditions1[i];
                                }
                                for(int i=0; i<conditions2.size(); i++){
                                    delete conditions2[i];
                                }
                                for(int i=0; i<cross_condition.size(); i++){
                                    delete cross_condition[i];
                                }
                                return false;
                            } else if(column_id2_1 != -1){
                                table_name2 = table_meta1->name;
                                column_id2 = column_id2_1;
                                table_id2 = 1;
                            } else {
                                table_name2 = table_meta2->name;
                                column_id2 = column_id2_2;
                                table_id2 = 2;
                            }
                        } else {
                            if(table_name2 == table_meta1->name){
                                column_id2 = findColumnId(table_meta1, column->column_name);
                                if(column_id2 == -1){
                                    cout<<"Error: cannot find the column "<<column->column_name<<endl;
                                    // clear conditions
                                    for(int i=0; i<conditions1.size(); i++){
                                        delete conditions1[i];
                                    }
                                    for(int i=0; i<conditions2.size(); i++){
                                        delete conditions2[i];
                                    }
                                    for(int i=0; i<cross_condition.size(); i++){
                                        delete cross_condition[i];
                                    }
                                    return false;
                                }
                                table_id2 = 1;
                            } else if(table_name2 == table_meta2->name){
                                column_id2 = findColumnId(table_meta2, column->column_name);
                                if(column_id2 == -1){
                                    cout<<"Error: cannot find the column "<<column->column_name<<endl;
                                    // clear conditions
                                    for(int i=0; i<conditions1.size(); i++){
                                        delete conditions1[i];
                                    }
                                    for(int i=0; i<conditions2.size(); i++){
                                        delete conditions2[i];
                                    }
                                    for(int i=0; i<cross_condition.size(); i++){
                                        delete cross_condition[i];
                                    }
                                    return false;
                                }
                                table_id2 = 2;
                            } else {
                                cout<<"Error: cannot find the table "<<table_name2<<endl;
                                // clear conditions
                                for(int i=0; i<conditions1.size(); i++){
                                    delete conditions1[i];
                                }
                                for(int i=0; i<conditions2.size(); i++){
                                    delete conditions2[i];
                                }
                                for(int i=0; i<cross_condition.size(); i++){
                                    delete cross_condition[i];
                                }
                                return false;
                            }
                        }
#ifdef DEBUG
    cout << 2 << endl;
    cout<<"column_id1: "<<column_id1<<endl;
    cout<<"column_id2: "<<column_id2<<endl;
#endif
                        // one table or two tables
                        if(table_id1 == table_id2){
                            // the same table
                            if(table_id1 == 1){
                                // check kind
                                if(table_meta1->columns[column_id1]->kind != table_meta1->columns[column_id2]->kind){
                                    cout<<"Error: cannot compare different types"<<endl;
                                    // clear conditions
                                    for(int i=0; i<conditions1.size(); i++){
                                        delete conditions1[i];
                                    }
                                    for(int i=0; i<conditions2.size(); i++){
                                        delete conditions2[i];
                                    }
                                    for(int i=0; i<cross_condition.size(); i++){
                                        delete cross_condition[i];
                                    }
                                    return false;
                                }

                                // get op
                                // check if string
                                bool is_str = (table_meta1->columns[column_id1]->kind == 2);
                                Condition::cond cond = getTwoCondition(op->operator_type, false, is_str);
                                if(is_str){
                                    // string
                                    condition = new CompareTwoStringCondition(cond, column_id1, column_id2);
                                } else {
                                    // int or float
                                    condition = new CompareTwoColumnCondition(cond, column_id1, column_id2);
                                }

                                // add to first table's conditions
                                conditions1.push_back(condition);
                            } else {
                                // check kind
                                if(table_meta2->columns[column_id1]->kind != table_meta2->columns[column_id2]->kind){
                                    cout<<"Error: cannot compare different types"<<endl;
                                    // clear conditions
                                    for(int i=0; i<conditions1.size(); i++){
                                        delete conditions1[i];
                                    }
                                    for(int i=0; i<conditions2.size(); i++){
                                        delete conditions2[i];
                                    }
                                    for(int i=0; i<cross_condition.size(); i++){
                                        delete cross_condition[i];
                                    }
                                    return false;
                                }

                                // get op
                                // check if string
                                bool is_str = (table_meta2->columns[column_id1]->kind == 2);
                                Condition::cond cond = getTwoCondition(op->operator_type, false, is_str);
                                if(is_str){
                                    // string
                                    condition = new CompareTwoStringCondition(cond, column_id1, column_id2);
                                } else {
                                    // int or float
                                    condition = new CompareTwoColumnCondition(cond, column_id1, column_id2);
                                }

                                // add to second table's conditions
                                conditions2.push_back(condition);
                            }
                        } else {
#ifdef DEBUG
    cout << 3 << endl;
#endif
                            // different tables
                            // switch if table_id1 = 2, table_id2 = 1
                            bool opposite = false;
                            if(table_id1 == 2){
                                string temp_table_name = table_name1;
                                table_name1 = table_name2;
                                table_name2 = temp_table_name;
                                unsigned int temp_column_id = column_id1;
                                column_id1 = column_id2;
                                column_id2 = temp_column_id;
                                unsigned int temp_table_id = table_id1;
                                table_id1 = table_id2;
                                table_id2 = temp_table_id;
                                opposite = true;
                            }

                            // check kind
#ifdef DEBUG
    cout << 5 << endl;
    cout<<table_meta1->columns.size()<<" "<<column_id1<<endl;
    cout<<table_meta2->columns.size()<<" "<<column_id2<<endl;
#endif
                            if(table_meta1->columns[column_id1]->kind != table_meta2->columns[column_id2]->kind){
                                cout<<"Error: cannot compare different types"<<endl;
                                // clear conditions
                                for(int i=0; i<conditions1.size(); i++){
                                    delete conditions1[i];
                                }
                                for(int i=0; i<conditions2.size(); i++){
                                    delete conditions2[i];
                                }
                                for(int i=0; i<cross_condition.size(); i++){
                                    delete cross_condition[i];
                                }
                                return false;
                            }

                            // get op
                            // check if string
                            bool is_str = (table_meta1->columns[column_id1]->kind == 2);
                            Condition::cond cond = getTwoCondition(op->operator_type, opposite, is_str);
                            if(is_str){
                                // string
                                condition = new CompareTwoStringCondition(cond, column_id1, column_id2);
                            } else {
                                // int or float
                                condition = new CompareTwoColumnCondition(cond, column_id1, column_id2);
                            }
#ifdef DEBUG
    cout << 4 << endl;
#endif
                            // add to cross table's conditions
                            cross_condition.push_back(condition);
                        }
                        break;
                    }
                    case Expression::VALUE:
                    {
                        Value* value = expr->value;
                        // check if the value is the same type as the column
                        if(table_meta->columns[column_id1]->kind != value->value_type){
                            cout<<"Error: the value is not the same type as the column"<<endl;
                            // clear conditions 
                            for(int i=0; i<conditions.size(); i++){
                                delete conditions[i];
                            }
                            return false;
                        }

                        if(value->value_type == Value::STRING){
                            Condition::cond cond;
                            switch (op->operator_type)
                            {
                                case Operator::EqualOrAssign:
                                    cond = Condition::EQS;
                                    break;
                                case Operator::NotEqual:
                                    cond = Condition::NES;
                                    break;
                                default:
                                    cout<<"Error: unknown operator"<<endl;
                                    // clear conditions 
                                    for(int i=0; i<conditions.size(); i++){
                                        delete conditions[i];
                                    }
                                    return false;
                                    break;
                            }
                            string bound = value->string_value;
                            condition = new StringCompareCondition(cond, column_id1, bound);
                        } else {
                            // get the op
                            Condition::cond cond;
                            switch (op->operator_type)
                            {
                                case Operator::EqualOrAssign:
                                    cond = Condition::EQ;
                                    break;
                                case Operator::Less:
                                    cond = Condition::LT;
                                    break;
                                case Operator::LessEqual:
                                    cond = Condition::LE;
                                    break;
                                case Operator::Greater:
                                    cond = Condition::GT;
                                    break;
                                case Operator::GreaterEqual:
                                    cond = Condition::GE;
                                    break;
                                case Operator::NotEqual:
                                    cond = Condition::NE;
                                    break;
                                default:
                                    cout<<"Error: unknown operator"<<endl;
                                    // clear conditions 
                                    for(int i=0; i<conditions.size(); i++){
                                        delete conditions[i];
                                    }
                                    return false;
                                    break;
                            }

                            // assemble condition
                            double bound;
                            if(value->value_type == Value::FLOAT){
                                bound = value->float_value;
                            } else {
                                bound = value->int_value;
                            }
                            condition = new CompareCondition(cond, column_id1, bound);
                        }

                        // add to conditions
                        conditions.push_back(condition);

                        break;
                    }
                    default:
                        cout<<"Error: unknown expression type"<<endl;
                        // clear conditions 
                        for(int i=0; i<conditions1.size(); i++){
                            delete conditions1[i];
                        }
                        for(int i=0; i<conditions2.size(); i++){
                            delete conditions2[i];
                        }
                        for(int i=0; i<cross_condition.size(); i++){
                            delete cross_condition[i];
                        }
                        return false;
                        break;
                }
                break;
            }

            case WhereClause::OperatorSelect:
            {
                cout<<"Error: cannot use select in where clause"<<endl;
                // clear conditions 
                for(int i=0; i<conditions1.size(); i++){
                    delete conditions1[i];
                }
                for(int i=0; i<conditions2.size(); i++){
                    delete conditions2[i];
                }
                for(int i=0; i<cross_condition.size(); i++){
                    delete cross_condition[i];
                }
                return false;
            }

            case WhereClause::IsNull:
            {
                WhereIsNull* where_is_null = (WhereIsNull*)where_clause;
                bool is_null = where_is_null->is_null;
                Condition::cond cond;
                cond = is_null ? Condition::IS_NULL : Condition::IS_NOT_NULL;
                condition = new NullCondition(cond, column_id1);
                conditions.push_back(condition);
                break;
            }

            case WhereClause::InList:
            {

                WhereInList* where_in_list = (WhereInList*)where_clause;
                vector<Value*> values = where_in_list->value_list->values;

                if(table_meta->columns[column_id1]->kind == 2){
                    vector<string> value_list;
                    for(int i=0; i<values.size(); i++){
                        if(values[i]->value_type != table_meta->columns[column_id1]->kind){
                            cout<<"Error: the value is not the same type as the column"<<endl;
                            // clear conditions 
                            for(int i=0; i<conditions1.size(); i++){
                                delete conditions1[i];
                            }
                            for(int i=0; i<conditions2.size(); i++){
                                delete conditions2[i];
                            }
                            for(int i=0; i<cross_condition.size(); i++){
                                delete cross_condition[i];
                            }
                            return false;
                        }
                        value_list.push_back(values[i]->string_value);
                    }
                    
                    condition = new StrListCondition(Condition::IN, column_id1);
                    for(int i=0; i<value_list.size(); i++){
                        ((StrListCondition*)condition)->add(value_list[i]);
                    }
                } else {
                    vector<double> value_list;
                    for(int i=0; i<values.size(); i++){
                        if(values[i]->value_type != table_meta->columns[column_id1]->kind){
                            cout<<"Error: the value is not the same type as the column"<<endl;
                            // clear conditions 
                            for(int i=0; i<conditions1.size(); i++){
                                delete conditions1[i];
                            }
                            for(int i=0; i<conditions2.size(); i++){
                                delete conditions2[i];
                            }
                            for(int i=0; i<cross_condition.size(); i++){
                                delete cross_condition[i];
                            }
                            return false;
                        }
                        switch(values[i]->value_type){
                            case Value::INT:
                                value_list.push_back((double)values[i]->int_value);
                                break;
                            case Value::FLOAT:
                                value_list.push_back((double)values[i]->float_value);
                                break;
                            default:
                                cout<<"Error: unknown value type"<<endl;
                                // clear conditions 
                                for(int i=0; i<conditions1.size(); i++){
                                    delete conditions1[i];
                                }
                                for(int i=0; i<conditions2.size(); i++){
                                    delete conditions2[i];
                                }
                                for(int i=0; i<cross_condition.size(); i++){
                                    delete cross_condition[i];
                                }
                                return false;
                                break;
                        }
                    }

                    condition = new NumListCondition(Condition::IN, column_id1);
                    for(int i=0; i<value_list.size(); i++){
                        ((NumListCondition*)condition)->add(value_list[i]);
                    }
                }
                conditions.push_back(condition);
                break;
            }

            case WhereClause::InSelect:
            {
                cout<<"Error: cannot use select in where clause"<<endl;
                // clear conditions 
                for(int i=0; i<conditions1.size(); i++){
                    delete conditions1[i];
                }
                for(int i=0; i<conditions2.size(); i++){
                    delete conditions2[i];
                }
                for(int i=0; i<cross_condition.size(); i++){
                    delete cross_condition[i];
                }
                return false;
            }

            case WhereClause::Like:
            {
                WhereLike* where_like = (WhereLike*)where_clause;
                string pattern = where_like->pattern;
                condition = new StringCondition(Condition::LIKE, column_id1, pattern);
                conditions.push_back(condition);
                break;
            }

            default:
            {   
                cout<<"Error: unknown where clause type"<<endl;
                // clear conditions 
                for(int i=0; i<conditions1.size(); i++){
                    delete conditions1[i];
                }
                for(int i=0; i<conditions2.size(); i++){
                    delete conditions2[i];
                }
                for(int i=0; i<cross_condition.size(); i++){
                    delete cross_condition[i];
                }
                return false;
                break;
            }
        }
    }

    return true;
}

int QueryManager::findColumnId(const TableMeta* table_meta, const string& column_name){
    for(int i=0; i<table_meta->columns.size(); i++){
        if(table_meta->columns[i]->name == column_name){
#ifdef DEBUG
cout<<"find column id: "<<i<<endl;
#endif
            return i;
        }
    }
#ifdef DEBUG
cout<<"find column id: "<<-1<<endl;
#endif
    return -1;
}

Condition::cond QueryManager::getTwoCondition(Operator::OperatorType op, bool opposite, bool is_str){
#ifdef DEBUG
    cout<<"getTwoCondition: opposite="<<opposite<<", is_str="<<is_str<<endl;
#endif
    if(!opposite){
        if(!is_str){
            switch (op)
            {
                case Operator::EqualOrAssign:
                    return Condition::EQ2;
                case Operator::Less:
                    return Condition::LT2;
                case Operator::LessEqual:
                    return Condition::LE2;
                case Operator::Greater:
                    return Condition::GT2;
                case Operator::GreaterEqual:
                    return Condition::GE2;
                case Operator::NotEqual:
                    return Condition::NE2;
                default:
                    return Condition::EQ2;
            }
        } else {
            switch (op)
            {
                case Operator::EqualOrAssign:
                    return Condition::EQS2;
                case Operator::NotEqual:
                    return Condition::NES2;
                default:
                    return Condition::EQS2;
            }
        }
        
    } else {
        if(!is_str){
            switch (op)
            {
                case Operator::EqualOrAssign:
                    return Condition::EQ2;
                case Operator::Less:
                    return Condition::GE2;
                case Operator::LessEqual:
                    return Condition::GT2;
                case Operator::Greater:
                    return Condition::LE2;
                case Operator::GreaterEqual:
                    return Condition::LT2;
                case Operator::NotEqual:
                    return Condition::NE2;
                default:
                    return Condition::NE2;
            }
        } else {
            switch (op)
            {
                case Operator::EqualOrAssign:
                    return Condition::EQS2;
                case Operator::NotEqual:
                    return Condition::NES2;
                default:
                    return Condition::EQS2;
            }
        }  
    }
#ifdef DEBUG
    cout<<"getTwoCondition done"<<endl;
#endif
}

vector<pair<Record*, Record*>> QueryManager::joinRecords(vector<Record*>& records1, vector<Record*>& records2, vector<Condition*>& cross_condition){
    vector<pair<Record*, Record*>> result;
    for(int i=0; i<records1.size(); i++){
        for(int j=0; j<records2.size(); j++){
            bool flag = true;
            for(int k=0; k<cross_condition.size(); k++){
                unsigned int column_id1 = cross_condition[k]->column;
                unsigned int column_id2;
                if(records1[i]->items[column_id1]->kind == 2){
                    column_id2 = ((CompareTwoStringCondition*)cross_condition[k])->column2;
                } else {
                    column_id2 = ((CompareTwoColumnCondition*)cross_condition[k])->column2;
                }

                // test if null
                if(records1[i]->bitmap[column_id1] == 0 && records2[j]->bitmap[column_id2] == 0){
                    continue;
                } else if (records1[i]->bitmap[column_id1] == 0 || records2[j]->bitmap[column_id2] == 0){
                    flag = false;
                    break;
                }

                // if string
                if(records1[i]->items[column_id1]->kind == 2){
                    CompareTwoStringCondition* condition = (CompareTwoStringCondition*)cross_condition[k];
                    string value1 = ((Char_Item*)records1[i]->items[column_id1])->str;
                    string value2 = ((Char_Item*)records2[j]->items[column_id2])->str;
                    if(!condition->compare(value1, value2)){
                        flag = false;
                        break;
                    }
                } else {
                    CompareTwoColumnCondition* condition = (CompareTwoColumnCondition*)cross_condition[k];
                    double value1 = 0;
                    double value2 = 0;
                    if(records1[i]->items[column_id1]->kind == 0){
                        value1 = (double)((Int_Item*)records1[i]->items[column_id1])->integer;
                    } else {
                        value1 = (double)((Float_Item*)records1[i]->items[column_id1])->floating;
                    }
                    if(records2[j]->items[column_id2]->kind == 0){
                        value2 = (double)((Int_Item*)records2[j]->items[column_id2])->integer;
                    } else {
                        value2 = (double)((Float_Item*)records2[j]->items[column_id2])->floating;
                    }

                    if(!condition->compare(value1, value2)){
                        flag = false;
                        break;
                    }
                }
            }
            if(flag){
                result.push_back(make_pair(records1[i], records2[j]));
            }
        }
    }
    return result;
}


// delete 
bool QueryManager::delete_(DeleteStatement* delete_statement){
    // get table meta
#ifdef DEBUG
    cout << "delete_" << endl;
#endif
    string table_name = delete_statement->table_name;
    TableMeta* table_meta = new TableMeta();
    if(!system_manager->getTableMeta(table_meta, table_name)){
        cout << "Error: cannot get the table meta of " << table_name << endl;
        delete table_meta;
        return false;
    }

    // get conditions
    vector<Condition*> conditions;
    vector<WhereClause*> where_clauses;
    if(delete_statement->where_and_clause){
        where_clauses = delete_statement->where_and_clause->where_clauses;
        if(!assembleSingleConditions(table_meta, where_clauses, conditions)){
            delete table_meta;
            return false;
        }
    }

    // open file
    int fileID = record_manager->openTableFile(table_name.c_str());
    if(fileID == -1){
        cout << "Error: cannot open the file of " << table_name << endl;
        delete table_meta;
        return false;
    }

    // use index to speed up
    IndexMap* index_map = new IndexMap();
    if(!system_manager->getIndexMap(index_map)){
        // close file
        record_manager->closeTableFile(fileID);
        delete table_meta;
        delete index_map;
        return false;
    }

    int condition_id1, condition_id2;
    int index_id;

    // get table id
    TableIndex* table_index = new TableIndex();
    if(!system_manager->getTableIndex(table_index)){
        // close file
        record_manager->closeTableFile(fileID);
        delete table_meta;
        delete index_map;
        delete table_index;
        return false;
    }
    unsigned int table_id = table_index->getIndex(table_name);
    delete table_index;

    index_id = fetchSelectIndex(table_id, index_map, conditions, condition_id1, condition_id2);

    vector<Record*> records;
    // find a index
    if(index_id != -1){
#ifdef DEBUG
        cout << "find index" << endl;
#endif
        IndexMeta index_meta = index_map->index_map[index_id];
        unsigned int index_size = 0;
        if(condition_id2 == -1){
            index_size = 1;
        } else {
            index_size = 2;
        }

        // get the selected conditions
        CompareCondition* condition1 = (CompareCondition*)conditions[condition_id1];

        // get the upper and lower bound
        // index tree will get [lower, upper)
        int lower, upper;
        switch(condition1->kind){
            case Condition::EQ:
                lower = condition1->bound;
                upper = condition1->bound + 1;
                break;
            case Condition::LT:
                lower = INT_MIN;
                upper = condition1->bound;
                break;
            case Condition::LE:
                lower = INT_MIN;
                upper = condition1->bound + 1;
                break;
            case Condition::GT:
                lower = condition1->bound + 1;
                upper = INT_MAX;
                break;
            case Condition::GE:
                lower = condition1->bound;
                upper = INT_MAX;
                break;
            default:
                lower = 0;
                upper = 0;
        }

        // assemble the key of the bounds
        Key key1;
        Key key2;
        if(index_size == 1){
            key1 = Key(lower);
            key2 = Key(upper);
        } else {
            // if two dimensions, needs to use the first to select index 
            // then use the second to select records from the index's
            vector<int> key1_vals;
            vector<int> key2_vals;
            key1_vals.push_back(lower);
            key1_vals.push_back(INT_MIN);
            key2_vals.push_back(upper);
            key2_vals.push_back(INT_MIN);
            key1 = Key(key1_vals);
            key2 = Key(key2_vals);
        }
#ifdef DEBUG
        key1.print();
        key2.print();
#endif

        // get the index tree
        Tree* tree = new Tree(index_meta.index_name + "_idx", index_fm, index_bpm);
        bool result = tree->initTree();
        if(!result){
            // close
            record_manager->closeTableFile(fileID);
            delete table_meta;
            delete index_map;
            delete tree;
            return false;
        }

        // get the indexes
        list<Index> positions = tree->searchRange(key1, key2);
        vector<RID> rids;

#ifdef DEBUG
        cout << "positions size: " << positions.size() << endl;
        for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
            it->print();
        }
#endif

        // if two demensions, need to select records from the index's
        if(index_size == 2){
            // get the second condition
            CompareCondition* condition2 = (CompareCondition*)conditions[condition_id2];
            
            for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
                if(condition2->compare(it->key.vals[1])){
                    rids.push_back(it->rid);
                }
            }
        } else {
            // all
            for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
                rids.push_back(it->rid);
            }
        }

#ifdef DEBUG
        cout << "rids size: " << rids.size() << endl;
        for(int i=0; i<rids.size(); i++){
            rids[i].print();
        }
#endif
        // select from the record file
        Record example_record = table_meta->getExampleRecord();
        records = record_manager->selectRecords(fileID, conditions, rids, &example_record);

        // close
        delete tree;
    } else {
        // get records
        Record example_record = table_meta->getExampleRecord();
        records = record_manager->selectRecords(fileID, conditions, &example_record);
    }

    // check records not referenced by other tables' foreign key
    if(!checkReferenced(table_id, records)){
        // close file
        record_manager->closeTableFile(fileID);
        delete table_meta;
        delete index_map;
        record_manager->freeRecordList(records);
        return false;
    }

    // delete records from the file
    // get all the index trees
    vector<Tree*> trees;
    vector<IndexMeta> index_metas = index_map->getIndexMetas(table_id);
    for(int i=0; i<index_metas.size(); i++){
        Tree* tree = new Tree(index_metas[i].index_name + "_idx", index_fm, index_bpm);
        bool result = tree->initTree();
        if(!result){
            // close
            record_manager->closeTableFile(fileID);
            delete table_meta;
            delete index_map;
            record_manager->freeRecordList(records);
            for(int j=0; j<trees.size(); j++){
                delete trees[j];
            }
            return false;
        }
        trees.push_back(tree);
    }
    delete index_map;

    // delete the records from the record file and all the index trees
    bool result;
    for(int i=0; i<records.size(); i++){
        result = record_manager->deleteRecord(fileID, records[i]->rid);
        if(!result){
            cout << "Error: cannot delete the record" << endl;
            delete table_meta;
            return false;
        }

        // delete from index file if needed
        for(int j=0; j<trees.size(); j++){
            // get the key
            vector<unsigned int> key_cols = index_metas[j].column_idx;
            vector<int> key_vals;
            for(int k=0; k<key_cols.size(); k++){
                key_vals.push_back(((Int_Item*)records[i]->items[key_cols[k]])->integer);
            }
            Key key = Key(key_vals);

            // delete from the index tree
#ifdef DEBUG
            cout<<"delete record: "<<endl;
            records[i]->print();
            cout << "delete index: " << endl;
            key.print();
            records[i]->rid.print();
            cout<<"\n\ntree before delete: "<<endl;
            trees[j]->print();
#endif
            result = trees[j]->deleteIndex(key, records[i]->rid);
#ifdef DEBUG
            cout << "delete index result: " << result << endl;
            Index index = trees[j]->searchIndex(key);
            index.print();

            cout<<"\n\ntree after delete: "<<endl;
            trees[j]->print();
#endif
            if(!result){
                cout << "Error: cannot delete the index" << endl;
                delete table_meta;
                // close file
                record_manager->closeTableFile(fileID);
                // close index trees
                for(int k=0; k<trees.size(); k++){
                    delete trees[k];
                }
                // delete records
                record_manager->freeRecordList(records);
                return false;
            }
        }
    }

    // print delete result
    cout<<"Query OK, "<<records.size()<<" row(s) affected";

    // delete conditions
    for(int i=0; i<conditions.size(); i++){
        delete conditions[i];
    }
    // delete records
    record_manager->freeRecordList(records);
    delete table_meta;
    // close file
    record_manager->closeTableFile(fileID);
    record_manager->closeAll();

    // close index trees
    for(int i=0; i<trees.size(); i++){
        delete trees[i];
    }

    // renew the fm and bpm
    this->record_manager->closeAll();
    delete this->record_manager->fileManager;
    delete this->record_manager->bufPageManager;
    this->record_manager->fileManager = new FileManager();
    this->record_manager->bufPageManager = new BufPageManager(this->record_manager->fileManager);
    delete this->index_fm;
    delete this->index_bpm;
    this->index_fm = new FileManager();
    this->index_bpm = new BufPageManager(this->index_fm);

    return true;
}

bool QueryManager::checkReferenced(unsigned int table_id, vector<Record*> pri_records){
    // check if the records are referenced by other tables' foreign key
    // if so, return false

    // get all the tables
    TableIndex* table_index = new TableIndex();
    if(!system_manager->getTableIndex(table_index)){
        cout << "Error: cannot get the table index" << endl;
        delete table_index;
        return false;
    }

    // check each table
    for(map<unsigned int, string>::iterator it = table_index->indexToTable.begin(); it != table_index->indexToTable.end(); it++){
        // skip if the table is the same
        if(it->first == table_id){
            continue;
        }

        // get the table meta
        TableMeta* table_meta = new TableMeta();
        if(!system_manager->getTableMeta(table_meta, it->second.c_str())){
            cout << "Error: cannot get the table meta of " << it->second << endl;
            delete table_meta;
            delete table_index;
            return false;
        }

        // check each foreign key
        for(int i=0; i<table_meta->foreignKeys.size(); i++){
            ForeignKeyConstraint* foreign_key = table_meta->foreignKeys[i];
            // skip if the foreign key is not referencing the table
            if(foreign_key->target_table != table_id){
                continue;
            }

            // get all the records from the table
            int fileID = record_manager->openTableFile(it->second.c_str());
            if(fileID == -1){
                cout << "Error: cannot open the table file" << endl;
                delete table_meta;
                delete table_index;
                return false;
            }
            Record example_record = table_meta->getExampleRecord();
            vector<Condition*> conditions;
            vector<Record*> all_records = record_manager->selectRecords(fileID, conditions, &example_record);
            record_manager->closeTableFile(fileID);

            // source and target columns
            vector<unsigned int> source_columns = foreign_key->source_columns;
            vector<unsigned int> target_columns = foreign_key->target_columns;
            
            // check each record of this referenced table: pri_records 
            bool referenced = false;
            for(int j=0; j<pri_records.size(); j++){
                // check if any record of the foreign key table is referencing the record
                for(int k=0; k<all_records.size(); k++){
                    Record* src = all_records[k];
                    Record* tar = pri_records[j];
                    // check each column
                    if(source_columns.size() == 1){
                        // one column key
                        unsigned int source_column = source_columns[0];
                        unsigned int target_column = target_columns[0];
                        if(((Int_Item*)tar->items[target_column])->integer == ((Int_Item*)src->items[source_column])->integer){
                            referenced = true;
                            break;
                        }
                    } else {
                        // two columns key
                        unsigned int source_column1 = source_columns[0];
                        unsigned int source_column2 = source_columns[1];
                        unsigned int target_column1 = target_columns[0];
                        unsigned int target_column2 = target_columns[1];
                        if(((Int_Item*)tar->items[target_column1])->integer == ((Int_Item*)src->items[source_column1])->integer &&
                            ((Int_Item*)tar->items[target_column2])->integer == ((Int_Item*)src->items[source_column2])->integer){
                            referenced = true;
                            break;
                        }
                    }
                }
                if(referenced){
                    break;
                }
            }
            if(referenced){
                cout << "Error: the record is referenced by the foreign key " << foreign_key->name << " of the table " << it->second << endl;
                delete table_meta;
                delete table_index;
                record_manager->freeRecordList(all_records);
                return false;
            }
        }

        delete table_meta;
    }

    delete table_index;
    return true;
}

// update
bool QueryManager::update(UpdateStatement* update_statement){
    // get table meta
#ifdef DEBUG
    cout << "update" << endl;
    cout.flush();
#endif
    string table_name = update_statement->table_name;
    TableMeta* table_meta = new TableMeta();
    if(!system_manager->getTableMeta(table_meta, table_name)){
        cout << "Error: cannot get the table meta of " << table_name << endl;
        delete table_meta;
        return false;
    }

    // get conditions
    vector<Condition*> conditions;
    vector<WhereClause*> where_clauses;
    if(update_statement->where_and_clause){
        where_clauses = update_statement->where_and_clause->where_clauses;
        if(!assembleSingleConditions(table_meta, where_clauses, conditions)){
#ifdef DEBUG
    cout<< 0 <<endl;
#endif
            delete table_meta;
            return false;
        }
    }
#ifdef DEBUG
    cout<< 1 <<endl;
#endif
    // open file
    int fileID = record_manager->openTableFile(table_name.c_str());
    if(fileID == -1){
        cout << "Error: cannot open the file of " << table_name << endl;
        // delete conditions
        for(int i=0; i<conditions.size(); i++){
            delete conditions[i];
        }
        delete table_meta;
        return false;
    }

    // TODO: use index to get the records
    IndexMap* index_map = new IndexMap();
    if(!system_manager->getIndexMap(index_map)){
        // close
        record_manager->closeTableFile(fileID);
        // delete conditions
        for(int i=0; i<conditions.size(); i++){
            delete conditions[i];
        }
        delete table_meta;
        delete index_map;
        return false;
    }

    int condition_id1, condition_id2;
    int index_id;

    // get table_id
    TableIndex* table_index = new TableIndex();
    if(!system_manager->getTableIndex(table_index)){
        // close file
        record_manager->closeTableFile(fileID);
        // delete conditions
        for(int i=0; i<conditions.size(); i++){
            delete conditions[i];
        }
        delete table_meta;
        delete index_map;
        delete table_index;
        return false;
    }
    unsigned int table_id = table_index->getIndex(table_name);
    delete table_index;

    // try selecting index
    index_id = fetchSelectIndex(table_id, index_map, conditions, condition_id1, condition_id2);
    
    // select result
    vector<Record*> records;

    // if there is an index
    if(index_id != -1){
#ifdef DEBUG
        cout << "find index" << endl;
#endif
        IndexMeta index_meta = index_map->index_map[index_id];
        unsigned int index_size = 0;
        if(condition_id2 == -1){
            index_size = 1;
        } else {
            index_size = 2;
        }

        // get the selected conditions
        CompareCondition* condition1 = (CompareCondition*)conditions[condition_id1];

        // get the upper and lower bound
        // index tree will get [lower, upper)
        int lower, upper;
        switch(condition1->kind){
            case Condition::EQ:
                lower = condition1->bound;
                upper = condition1->bound + 1;
                break;
            case Condition::LT:
                lower = INT_MIN;
                upper = condition1->bound;
                break;
            case Condition::LE:
                lower = INT_MIN;
                upper = condition1->bound + 1;
                break;
            case Condition::GT:
                lower = condition1->bound + 1;
                upper = INT_MAX;
                break;
            case Condition::GE:
                lower = condition1->bound;
                upper = INT_MAX;
                break;
            default:
                lower = 0;
                upper = 0;
        }

        // assemble the key of the bounds
        Key key1;
        Key key2;
        if(index_size == 1){
            key1 = Key(lower);
            key2 = Key(upper);
        } else {
            // if two dimensions, needs to use the first to select index 
            // then use the second to select records from the index's
            vector<int> key1_vals;
            vector<int> key2_vals;
            key1_vals.push_back(lower);
            key1_vals.push_back(INT_MIN);
            key2_vals.push_back(upper);
            key2_vals.push_back(INT_MIN);
            key1 = Key(key1_vals);
            key2 = Key(key2_vals);
        }
#ifdef DEBUG
        key1.print();
        key2.print();
#endif

        // get the index tree
        Tree* tree = new Tree(index_meta.index_name + "_idx", index_fm, index_bpm);
        bool result = tree->initTree();
        if(!result){
            // close
            record_manager->closeTableFile(fileID);
            delete table_meta;
            delete index_map;
            delete tree;
            // delete conditions
            for(int i=0; i<conditions.size(); i++){
                delete conditions[i];
            }
            return false;
        }

        // get the indexes
        list<Index> positions = tree->searchRange(key1, key2);
        vector<RID> rids;

#ifdef DEBUG
        cout << "positions size: " << positions.size() << endl;
        for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
            it->print();
        }
#endif


        // if two demensions, need to select records from the index's
        if(index_size == 2){
            // get the second condition
            CompareCondition* condition2 = (CompareCondition*)conditions[condition_id2];
            
            for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
                if(condition2->compare(it->key.vals[1])){
                    rids.push_back(it->rid);
                }
            }
        } else {
            // all
            for(list<Index>::iterator it = positions.begin(); it != positions.end(); it++){
                rids.push_back(it->rid);
            }
        }

#ifdef DEBUG
        cout << "rids size: " << rids.size() << endl;
        for(int i=0; i<rids.size(); i++){
            rids[i].print();
        }
#endif

        // select from the record file
        Record example_record = table_meta->getExampleRecord();
        records = record_manager->selectRecords(fileID, conditions, rids, &example_record);

#ifdef DEBUG
        cout << "records size: " << records.size() << endl;
        for(int i=0; i<records.size(); i++){
            records[i]->print();
        }
#endif
        // delete
        delete tree;
#ifdef DEBUG
        cout<<"end"<<endl;
#endif
    
    } else {
        // select all the records according to the conditions;
        Record example_record = table_meta->getExampleRecord();
        records = record_manager->selectRecords(fileID, conditions, &example_record);
    }
    for(int i=0; i<conditions.size(); i++){
        delete conditions[i];
    }
#ifdef DEBUG
    cout<< 2 <<endl;
    cout<<"records size: "<<records.size()<<endl;
#endif
    // update records
    vector<SetValue*> set_values = update_statement->set_clause->set_values;
    vector<pair<unsigned int, int>> new_int_vals;
    vector<pair<unsigned int, float>> new_float_vals;
    vector<pair<unsigned int, string>> new_string_vals;
    vector<unsigned int> set_null;

    // check the type of the values
    for(int i=0; i<set_values.size(); i++){
        string column_name = set_values[i]->column_name;
        int column_id = findColumnId(table_meta, column_name);
        if(column_id == -1){
            cout << "Error: cannot find the column " << column_name << endl;
            delete table_meta;
            // close
            record_manager->closeTableFile(fileID);
            delete index_map;
            // free records
            for(int i=0; i<records.size(); i++){
                delete records[i];
            }
            return false;
        }

        if(set_values[i]->value->value_type == Value::INT){
            // check the type
            if(table_meta->columns[column_id]->kind != 0){
                cout << "Error: the type of the column " << column_name << " is not int" << endl;
                delete table_meta;
                // close
                record_manager->closeTableFile(fileID);
                delete index_map;
                // free records
                for(int i=0; i<records.size(); i++){
                    delete records[i];
                }
                return false;
            }
            int value = set_values[i]->value->int_value;
            new_int_vals.push_back(make_pair(column_id, value));
        } else if (set_values[i]->value->value_type == Value::FLOAT){
            // check the type
            if(table_meta->columns[column_id]->kind != 1){
                cout << "Error: the type of the column " << column_name << " is not float" << endl;
                delete table_meta;
                // close
                record_manager->closeTableFile(fileID);
                delete index_map;
                // free records
                for(int i=0; i<records.size(); i++){
                    delete records[i];
                }
                return false;
            }
            float value = set_values[i]->value->float_value;
            new_float_vals.push_back(make_pair(column_id, value));
        } else if (set_values[i]->value->value_type == Value::STRING){
            // check the type
            if(table_meta->columns[column_id]->kind != 2){
                cout << "Error: the type of the column " << column_name << " is not string" << endl;
                delete table_meta;
                // close
                record_manager->closeTableFile(fileID);
                delete index_map;
                // free records
                for(int i=0; i<records.size(); i++){
                    delete records[i];
                }
                return false;
            }
            string value = set_values[i]->value->string_value;
            new_string_vals.push_back(make_pair(column_id, value));
        } else {
            // NULLVAL
            if(table_meta->columns[column_id]->notNull){
                cout << "Error: the column " << column_name << " is not null" << endl;
                delete table_meta;
                // close
                record_manager->closeTableFile(fileID);
                delete index_map;
                // free records
                for(int i=0; i<records.size(); i++){
                    delete records[i];
                }
                return false;
            } 
            set_null.push_back(column_id);
        }
    }

#ifdef DEBUG
    cout<< 3 <<endl;
    cout<<"set clauses: "<<endl;
    cout<<"int"<<endl;
    for(int i=0; i<new_int_vals.size(); i++){
        cout<<new_int_vals[i].first<<" "<<new_int_vals[i].second<<endl;
    }
    cout<<"float"<<endl;
    for(int i=0; i<new_float_vals.size(); i++){
        cout<<new_float_vals[i].first<<" "<<new_float_vals[i].second<<endl;
    }
    cout<<"string"<<endl;
    for(int i=0; i<new_string_vals.size(); i++){
        cout<<new_string_vals[i].first<<" "<<new_string_vals[i].second<<endl;
    }
    cout<<"null"<<endl;
    for(int i=0; i<set_null.size(); i++){
        cout<<set_null[i]<<endl;
    }
#endif

    // check if referenced by foreign key
    if(!checkReferenced(table_id, records)){
        delete table_meta;
        delete index_map;
        // close
        record_manager->closeTableFile(fileID);
        // free records
        record_manager->freeRecordList(records);
        return false;
    }

    // check if referencing other table's primary keys

    // // modify the records
    // for(int i=0; i<records.size(); i++){
    //     for(int j=0; j<new_int_vals.size(); j++){
    //         ((Int_Item*)records[i]->items[new_int_vals[j].first])->integer = new_int_vals[j].second;
    //     }
    //     for(int j=0; j<new_float_vals.size(); j++){
    //         ((Float_Item*)records[i]->items[new_float_vals[j].first])->floating = new_float_vals[j].second;
    //     }
    //     for(int j=0; j<new_string_vals.size(); j++){
    //         char* str = ((Char_Item*)records[i]->items[new_string_vals[j].first])->str;
    //         int len = ((Char_Item*)records[i]->items[new_string_vals[j].first])->len;
    //         memset(str, 0, len);
    //         int new_len = new_string_vals[j].second.length();
    //         memcpy(str, new_string_vals[j].second.c_str(), new_len < len ? new_len : len);
    //     }
    //     for(int j=0; j<set_null.size(); j++){
    //         records[i]->bitmap[set_null[j]] = 0;
    //     }
    // }

    // make a copy of the records
    vector<Record*> new_records;
    for(int i=0; i<records.size(); i++){
        Record* new_record = new Record();
        for(int j=0; j<records[i]->items.size(); j++){
            switch(records[i]->items[j]->kind){
                case 0:
                    {
                        Int_Item* int_item = (Int_Item*)records[i]->items[j];
                        new_record->append(records[i]->bitmap[j], int_item->integer);
                        break;
                    }
                case 1:
                    {
                        Float_Item* float_item = (Float_Item*)records[i]->items[j];
                        new_record->append(records[i]->bitmap[j], float_item->floating);
                        break;
                    }
                case 2:
                    {
                        Char_Item* char_item = (Char_Item*)records[i]->items[j];
                        new_record->append(records[i]->bitmap[j], char_item->str, char_item->len);
                        break;
                    }
                default:
                    {
                        cout << "Error: unknown item kind" << endl;
                        return false;
                    }
            }
        }
        new_record->rid = records[i]->rid;
        new_records.push_back(new_record);
    }

    // modify the new records
    for(int i=0; i<new_records.size(); i++){
        for(int j=0; j<new_int_vals.size(); j++){
            ((Int_Item*)new_records[i]->items[new_int_vals[j].first])->integer = new_int_vals[j].second;
        }
        for(int j=0; j<new_float_vals.size(); j++){
            ((Float_Item*)new_records[i]->items[new_float_vals[j].first])->floating = new_float_vals[j].second;
        }
        for(int j=0; j<new_string_vals.size(); j++){
            char* str = ((Char_Item*)new_records[i]->items[new_string_vals[j].first])->str;
            int len = ((Char_Item*)new_records[i]->items[new_string_vals[j].first])->len;
            memset(str, 0, len);
            int new_len = new_string_vals[j].second.length();
            memcpy(str, new_string_vals[j].second.c_str(), new_len < len ? new_len : len);
        }
        for(int j=0; j<set_null.size(); j++){
            new_records[i]->bitmap[set_null[j]] = 0;
        }
    }
#ifdef DEBUG
    cout<< 4 <<endl;
    cout<<"new records:"<<endl;
    for(int i=0; i<new_records.size(); i++){
        new_records[i]->print();
    }
#endif

    // check if primary key is unique
    for(int i=0; i<new_records.size(); i++){
        if(!checkPrimary(table_meta, new_records[i])){
            delete table_meta;
            delete index_map;
            // close
            record_manager->closeTableFile(fileID);
            // delete records
            record_manager->freeRecordList(new_records);
            record_manager->freeRecordList(records);
            return false;
        }
    }
#ifdef DEBUG
    cout << "Primary key check passed" << endl;
#endif

    // check the foreign keys
    for(int i=0; i<new_records.size(); i++){
#ifdef DEBUG
        cout << "Checking foreign key for record " << i << endl;
        new_records[i]->print();
#endif
        if(!checkForeign(table_meta, new_records[i])){
            cout << "Error: foreign key constraint failed" << endl;
            delete table_meta;
            delete index_map;
            // close
            record_manager->closeTableFile(fileID);
            // delete records
            record_manager->freeRecordList(new_records);
            record_manager->freeRecordList(records);
            return false;
        }
    }
    // record_manager->freeRecordList(records);
#ifdef DEBUG
    cout << "Foreign key check passed" << endl;
#endif

    // get all the index trees
    vector<Tree*> index_trees;
    vector<IndexMeta> index_metas = index_map->getIndexMetas(table_id);
    for(int i=0; i<index_metas.size(); i++){
        Tree* tree = new Tree(index_metas[i].index_name + "_idx", index_fm, index_bpm);
        bool result = tree->initTree();
        if(!result){
            // close
            record_manager->closeTableFile(fileID);
            // delete records
            record_manager->freeRecordList(new_records);
            record_manager->freeRecordList(records);
            // delete trees
            for(int j=0; j<index_trees.size(); j++){
                delete index_trees[j];
            }
            delete table_meta;
            delete index_map;
            return false;
        }
        index_trees.push_back(tree);
    }
    delete index_map;
    delete table_meta;

#ifdef DEBUG
    cout << "Index trees loaded" << endl;
#endif

    // update the records
    for(int i=0; i<new_records.size(); i++){
#ifdef DEBUG
    cout<< 4 <<endl;
#endif  
        bool result = record_manager->updateRecord(fileID, new_records[i]->rid, new_records[i]);
        if(!result){
            cout << "Error: cannot update the record" << endl;
            // close
            record_manager->closeTableFile(fileID);
            // delete records
            record_manager->freeRecordList(new_records);
            record_manager->freeRecordList(records);
            // delete trees
            for(int j=0; j<index_trees.size(); j++){
                delete index_trees[j];
            }
            return false;
        }

        // update in all index trees
        for(int j=0; j<index_trees.size(); j++){
            // get the old key
            vector<unsigned int> key_cols = index_metas[j].column_idx;
            vector<int> old_key_vals;
            for(int k=0; k<key_cols.size(); k++){
                old_key_vals.push_back(((Int_Item*)records[i]->items[key_cols[k]])->integer);
            }
            Key old_key = Key(old_key_vals);

            // get the new key
            vector<int> key_vals;
            for(int k=0; k<key_cols.size(); k++){
                key_vals.push_back(((Int_Item*)new_records[i]->items[key_cols[k]])->integer);
            }
            Key key = Key(key_vals);

            // delete the old key
            result = index_trees[j]->deleteIndex(old_key, new_records[i]->rid);
            if(!result){
                cout << "Error: cannot delete the old index" << endl;
#ifdef DEBUG
                cout << "key: ";
                key.print();
                cout << "r id: ";
                new_records[i]->rid.print();
#endif
                // close
                record_manager->closeTableFile(fileID);
                // delete records
                record_manager->freeRecordList(new_records);
                record_manager->freeRecordList(records);
                // delete trees
                for(int j=0; j<index_trees.size(); j++){
                    delete index_trees[j];
                }
                return false;
            }

#ifdef DEBUG 
            cout << "old key deleted" << endl;
            index_trees[j]->print();
#endif
            // insert the new key
            result = index_trees[j]->insertIndex(Index(key, new_records[i]->rid));

            if(!result){
                cout << "Error: cannot insert the new index" << endl;
                // close
                record_manager->closeTableFile(fileID);
                // delete records
                record_manager->freeRecordList(new_records);
                record_manager->freeRecordList(records);
                // delete trees
                for(int j=0; j<index_trees.size(); j++){
                    delete index_trees[j];
                }
                return false;
            }
#ifdef DEBUG
            cout << "new key inserted" << endl;
#endif
        }
    }

    // print update result
    cout<<"Query OK, "<< new_records.size() <<" row(s) affected";
      

#ifdef DEBUG
    cout<< 5 <<endl;
#endif
    // delete records
    record_manager->freeRecordList(new_records);
    record_manager->freeRecordList(records);
    // close file
    record_manager->closeTableFile(fileID);
    // delete trees
    for(int j=0; j<index_trees.size(); j++){
        delete index_trees[j];
    }

    // renew the fm and bpm
    this->record_manager->closeAll();
    delete this->record_manager->fileManager;
    delete this->record_manager->bufPageManager;
    this->record_manager->fileManager = new FileManager();
    this->record_manager->bufPageManager = new BufPageManager(this->record_manager->fileManager);
    delete this->index_fm;
    delete this->index_bpm;
    this->index_fm = new FileManager();
    this->index_bpm = new BufPageManager(this->index_fm);

    return true;
}


// file in out
bool QueryManager::safe_insert(string file_name, string table_name){
    // get the table meta from the index file
    TableMeta* table_meta = new TableMeta();
    if (!system_manager->getTableMeta(table_meta, table_name)) {
        cout << "Error: cannot get the table meta of " << table_name << endl;
        delete table_meta;
        return false;
    }

    // get the table id
    TableIndex* table_index = new TableIndex();
    if (!system_manager->getTableIndex(table_index)) {
        cout << "Error: cannot get the table index" << endl;
        delete table_index;
        delete table_meta;
        return false;
    }
    unsigned int table_id = table_index->getIndex(table_name);
    if (table_id == 0) {
        cout << "Error: cannot find the table " << table_name << endl;
        delete table_index;
        delete table_meta;
        return false;
    }
    delete table_index;

    // open the table file
    int fileId = record_manager->openTableFile(table_name.c_str());
    if (fileId == -1) {
        cout << "Error: cannot open the table " << table_name << endl;
        delete table_meta;
        return false;
    }

    // get the index map
    IndexMap* index_map = new IndexMap();
    if (!system_manager->getIndexMap(index_map)) {
        cout << "Error: cannot get the index map" << endl;
        // close file
        if (!record_manager->closeTableFile(fileId)) {
            cout << "Error: cannot close the table " << table_name << endl;
        }
        delete index_map;
        delete table_meta;
        return false;
    }
    vector<IndexMeta> indexes = index_map->getIndexMetas(table_id);
    // open and save all the index trees
    vector<Tree*> index_trees;
    for (int i = 0; i < indexes.size(); i++) {
        Tree* tree = new Tree(indexes[i].index_name + "_idx", index_fm, index_bpm);
        tree->initTree();
        index_trees.push_back(tree);
    }
    delete index_map;

    // open the input file and read the records
    ifstream in_file(file_name);
    // read a line each time until the end of the file
    string line;
    if(!in_file.is_open()){
        cout << "Error: cannot open the file " << file_name << endl;
        // close file
        if (!record_manager->closeTableFile(fileId)) {
            cout << "Error: cannot close the table " << table_name << endl;
        }
        // delete trees
        for (int i = 0; i < index_trees.size(); i++) {
            delete index_trees[i];
        }
        delete table_meta;
        return false;
    } 

    int count = 0;
    int total_line = 0;
    int percent = 0;
    // count lines
    string temp;
    while(getline(in_file, temp)){
        total_line++;
    }
    in_file.clear();
    in_file.seekg(ios::beg);
    
    while(getline(in_file, line)){
        // parse the line 
        vector<string> items;
        line = line + ',';
        size_t pos = line.find(',');
        while(pos != string::npos){
            // cout<<"item: "<<line.substr(0, pos)<<endl;
            items.push_back(line.substr(0, pos));
            line = line.substr(pos+1);
            pos = line.find(',');
            if(line.length() == 0)
                break;
        }

        // assemble a record
        Record* record = new Record();
        for(int i=0; i<items.size(); i++){
            switch(table_meta->columns[i]->kind){
                case 0:
                    record->append(true, atoi(items[i].c_str()));
                    break;
                case 1:
                    record->append(true, (float)atof(items[i].c_str()));
                    break;
                case 2:{
                    int len = table_meta->columns[i]->len;
                    record->append(true, items[i].c_str(), len);
                    break;
                }
            }
        }

        // insert to the table and index files
        RID rid = record_manager->insertRecord(fileId, record);

        // cout<<"insert"<<endl;

        // insert to the index trees
        for(int i=0; i<indexes.size(); i++){
            vector<int> vals;
            vector<unsigned int> key_cols = indexes[i].column_idx;
            for(int j=0; j<key_cols.size(); j++){
                vals.push_back(((Int_Item*)record->items[key_cols[j]])->integer);
            }
            Key key(vals);
            // cout<<"to index tree"<<endl;
            // key.print();
            index_trees[i]->insertIndex(Index(key, rid));
            // cout<<"after index tree"<<endl;
        }

        delete record;

        count++;
        int new_percent = (int)((double)count * 100 / total_line);
        if(new_percent > percent){
            system("clear");
            percent = new_percent;
            cout<<percent<<"% finished"<<endl;
        }
    }

    // close file
    if (!record_manager->closeTableFile(fileId)) {
        cout << "Error: cannot close the table " << table_name << endl;
    }
    // delete trees
    for (int i = 0; i < index_trees.size(); i++) {
        delete index_trees[i];
    }
    delete table_meta;
    in_file.close();

    // renew the fm and bpm
    this->record_manager->closeAll();
    delete this->record_manager->fileManager;
    delete this->record_manager->bufPageManager;
    this->record_manager->fileManager = new FileManager();
    this->record_manager->bufPageManager = new BufPageManager(this->record_manager->fileManager);
    delete this->index_fm;
    delete this->index_bpm;
    this->index_fm = new FileManager();
    this->index_bpm = new BufPageManager(this->index_fm);

    cout<<"load file done";
    return true;
}


bool QueryManager::safe_select(string file_name, string table_name){
    // dump the table to the file
    ofstream out_file(file_name);
    if(!out_file.is_open()){
        cout << "Error: cannot open the file " << file_name << endl;
        return false;
    }

    // open the table file
    int fileId = record_manager->openTableFile(table_name.c_str());

    // get the table meta from the index file
    TableMeta* table_meta = new TableMeta();
    if (!system_manager->getTableMeta(table_meta, table_name)) {
        cout << "Error: cannot get the table meta of " << table_name << endl;
        delete table_meta;
        // close file
        if (!record_manager->closeTableFile(fileId)) {
            cout << "Error: cannot close the table " << table_name << endl;
        }
        out_file.close();
        return false;
    }

    // select all the records
    Record example_record = table_meta->getExampleRecord();
    vector<Condition*> conds;
    vector<Record*> records = record_manager->selectRecords(fileId, conds, &example_record);

    int total_line = records.size();
    int percent = 0;
    cout<<"fetch "<<total_line<<" records, writing to file"<<endl;
    // write the records to the file
    for(int i=0; i<records.size(); i++){
        Record* record = records[i];
        for(int j=0; j<record->items.size(); j++){
            if(record->bitmap[j] == false){
                switch(table_meta->columns[j]->kind){
                    case 0:
                        out_file << "0";
                        break;
                    case 1:
                        out_file << "0.0";
                        break;
                    case 2:
                        out_file << "";
                        break;
                }
            }else{
                switch(table_meta->columns[j]->kind){
                    case 0:
                        out_file<<((Int_Item*)record->items[j])->integer;
                        break;
                    case 1:
                        out_file<<((Float_Item*)record->items[j])->floating;
                        break;
                    case 2:
                        out_file<<((Char_Item*)record->items[j])->str;
                        break;
                }
            }
            if(j != record->items.size()-1){
                out_file<<",";
            }
        }
        out_file<<endl;

        int new_percent = (int)((double)i * 100 / total_line);
        if(new_percent > percent){
            system("clear");
            percent = new_percent;
            cout<<percent<<"% finished"<<endl;
        }
    }

    // close file
    if (!record_manager->closeTableFile(fileId)) {
        cout << "Error: cannot close the table " << table_name << endl;
    }
    delete table_meta;
    out_file.close();

    // renew the fm and bpm
    this->record_manager->closeAll();
    delete this->record_manager->fileManager;
    delete this->record_manager->bufPageManager;
    this->record_manager->fileManager = new FileManager();
    this->record_manager->bufPageManager = new BufPageManager(this->record_manager->fileManager);
    delete this->index_fm;
    delete this->index_bpm;
    this->index_fm = new FileManager();
    this->index_bpm = new BufPageManager(this->index_fm);

    cout<<"dump file done";
    return true;
}