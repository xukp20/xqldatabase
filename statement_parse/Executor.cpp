#include "Executor.h"
#include <iomanip>

Executor::Executor(string base_path, bool init){
    system_manager = new SystemManager(base_path);
    if(init){
        cout<<"Initializing system..."<<endl;
        system_manager->initSys();
    }
    system_manager->initPath();
    record_manager = new RecordManager();

    // for index trees
    index_fm = new FileManager();
    index_bpm = new BufPageManager(index_fm);

    // query manager
    query_manager = new QueryManager(record_manager, system_manager, index_fm, index_bpm);
}

Executor::~Executor(){
    delete system_manager;
    delete record_manager;
    delete index_fm;
    delete index_bpm;
}

bool Executor::init(){
    return system_manager->initSys();
}

bool Executor::run(){
    return true;
}

bool Executor::run(DBStatement* statement){
    startTimer();
    switch(statement->type){
        case DBStatement::CREATE:
            // call create database
            // cout<<"create database "<<statement->identifier<<endl;
            return system_manager->createDatabase(statement->identifier);
            break;
        case DBStatement::DROP:
            // call drop database
            // cout<<"drop database "<<statement->identifier<<endl;
            return system_manager->dropDatabase(statement->identifier);
            break;
        case DBStatement::USE:
            // cout<<"use database "<<statement->identifier<<endl;
            return system_manager->useDatabase(statement->identifier);
            break;
        case DBStatement::SHOWDBS:
            // call show databases
            // cout<<"show databases"<<endl;
            return system_manager->showDatabases();
            break;
        case DBStatement::SHOWTABLES:
            // call show tables
            // cout<<"show tables"<<endl;
            return system_manager->showTables();
            break;
        case DBStatement::SHOWINDEXES:
            // call show indexes
            // cout<<"show indexes"<<endl;
            return system_manager->showIndexes();
            break;
        default:
            break;
    }
    return true;
}

bool Executor::run(IOStatement* statement){
    switch(statement->type){
        case IOStatement::LOAD:
            {
                bool result = query_manager->safe_insert(statement->file_path, statement->table_name);
                if(!result){
                    return false;
                }
                break;
            }
        case IOStatement::DUMP:
            {
                bool result = query_manager->safe_select(statement->file_path, statement->table_name);
                if(!result){
                    return false;
                }
                break;
            }
        default:
            break;
    }
    return true;
}

bool Executor::run(TableStatement* statement){
    startTimer();
    bool result;
    switch(statement->type){
        case TableStatement::CREATE:
            {
                TableMeta* table_meta = new TableMeta();
                vector<ForeignKeyField*> foreign_keys;
                if(!convertTableMeta((CreateTableStatement*)statement, table_meta, foreign_keys)){
                    return false;
                }
                // cout<<"after convert"<<endl;
                // table_meta->print();
                result = system_manager->createTable(table_meta, foreign_keys, record_manager);
                if(!result){
                    return false;
                }
                delete table_meta;
                break;
            }
        case TableStatement::DROP:
            {
                DropTableStatement* drop_statement = (DropTableStatement*)statement;
                result = system_manager->dropTable(drop_statement->table_name);
                if(!result){
                    return false;
                }
                break;
            }
        case TableStatement::DESC:
            {
                DescTableStatement* desc_statement = (DescTableStatement*)statement;
                result = system_manager->descTable(desc_statement->table_name);
                if(!result){
                    return false;
                }
                break;
            }
        case TableStatement::INSERT:
            {
                InsertStatement* insert_statement = (InsertStatement*)statement;
                result = query_manager->insert(insert_statement);
                if(!result){
                    return false;
                }
                break;
            }
        case TableStatement::DELETE:
            {
                DeleteStatement* delete_statement = (DeleteStatement*)statement;
                result = query_manager->delete_(delete_statement);
                if(!result){
                    return false;
                }
                break;
            }
        case TableStatement::SELECT:
            // call select
            {
                SelectStatement* select_statement = (SelectStatement*)statement;
                result = query_manager->select(select_statement);
                if(!result){
                    return false;
                }
                break;
            }
        case TableStatement::UPDATE:
            {
                UpdateStatement* update_statement = (UpdateStatement*)statement;
                result = query_manager->update(update_statement);
                if(!result){
                    return false;
                }
                break;
            }
        default:
            break;
    }
    return true;
}

bool Executor::run(AlterStatement* statement){
    startTimer();
    bool result;
    switch(statement->alter_type){
        case AlterStatement::ADD:
            {
                switch(statement->target_type){
                    case AlterStatement::INDEX:
                    {
                        result = system_manager->createIndex(statement, record_manager, index_fm, index_bpm);
                        if(!result){
                            return false;
                        }
                        break;
                    }
                    case AlterStatement::PRIMARY:
                    {
                        result = system_manager->addPrimaryKey(statement, record_manager, index_fm, index_bpm);
                        if(!result){
                            return false;
                        }
                        break;
                    }
                    case AlterStatement::FOREIGN:
                    {
                        result = system_manager->addForeignKey((AlterForeignKeyStatement*)statement, record_manager, index_fm, index_bpm);
                        if(!result){
                            return false;
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
        case AlterStatement::DROP:
            {
                switch(statement->target_type){
                    case AlterStatement::INDEX:
                    {
                        result = system_manager->dropIndex(statement);
                        if(!result){
                            return false;
                        }
                        break;
                    }
                    case AlterStatement::PRIMARY:
                    {
                        result = system_manager->dropPrimaryKey(statement);
                        if(!result){
                            return false;
                        }
                        break;
                    }
                    case AlterStatement::FOREIGN:
                    {
                        result = system_manager->dropForeignKey((AlterForeignKeyStatement*)statement);
                        if(!result){
                            return false;
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
        default:
            break;
    }
    return true;
}

// tools
void Executor::startTimer(){
    start_time = clock();
}

void Executor::endTimer(){
    end_time = clock();
}

void Executor::printTime(){
    cout<<" ("<<setiosflags(ios::fixed)<<setprecision(2)<<(double)(end_time - start_time) / CLOCKS_PER_SEC<<" sec)"<<endl;
}

void Executor::showNextPrefix(){
    string current_db = system_manager->getCurrentDB();
    if(current_db == ""){
        cout<<"Xql(no database in use)> ";
    } else {
        cout<<current_db<<"> ";
    }
    return;
}

void Executor::printEnd(){
    endTimer();
    printTime();
    // showNextPrefix();
}

// converters
bool Executor::convertTableMeta(CreateTableStatement* statement, TableMeta* table_meta, vector<ForeignKeyField*>& foreign_key_fields){
    table_meta->name = statement->table_name;

    // check through the fields, record the foreign keys and primary key to be used later
    Identifiers* primary_key_field = nullptr;

    vector<Field*> fields = statement->field_list->fields;
    for(int i = 0; i < fields.size(); i++){
        Field* field = fields[i];
        switch(field->field_type){
            case Field::COLUMN:
            {
                ColumnMeta* column_meta = field->column_meta;
                bool result = table_meta->addColumn(column_meta);
                if(!result){
                    return false;
                }
                break;
            }
            case Field::FOREIGN_KEY:
            {    
                foreign_key_fields.push_back(field->foreign_key_constraint);
                break;
            }
            case Field::PRIMARY_KEY:
            {
                if(primary_key_field != nullptr){
                    cout<<"Error: multiple primary keys"<<endl;
                    return false;
                }
                primary_key_field = field->primary_key_constraint;
                break;
            }
            default:
                break;
        }
    }
    table_meta->column_num = table_meta->columns.size();

    // change primary key index to unsigned int
    if(primary_key_field != nullptr){
        vector<string> primary_key_fields = primary_key_field->identifiers;
        vector<unsigned int> primary_key_fields_index;
        for(int i = 0; i < primary_key_fields.size(); i++){
            string primary_key_field = primary_key_fields[i];
            bool found = false;
            for(int j = 0; j < table_meta->column_num; j++){
                if(table_meta->columns[j]->name == primary_key_field){
                    primary_key_fields_index.push_back(j);
                    found = true;
                    break;
                }
            }
            if(!found){
                cout<<"Error: primary key field "<<primary_key_field<<" not found"<<endl;
                return false;
            }
        }
        PrimaryKeyConstraint* primary_key_constraint = new PrimaryKeyConstraint(primary_key_fields_index);
        table_meta->addPrimaryKey(primary_key_constraint);
    }

    return true;
}