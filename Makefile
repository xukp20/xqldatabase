# global path
CC := g++ -std=c++17
Link := ld
PWD := $(shell pwd)
O_DIR := $(PWD)/obj
EXE_DIR := $(PWD)/build
UTILS_DIR := $(PWD)/utils

# tasks path
REC_PATH := $(PWD)/record_management
FIL_PATH := $(PWD)/filesystem
SYS_PATH := $(PWD)/system_management
IDX_PATH := $(PWD)/index_management
BTREE_SUBPATH := $(IDX_PATH)/BplusTree
FLEX_PATH := $(PWD)/statement_parse
QUERY_PATH := $(PWD)/query_management

#main
xql_db: statement_parser r_manage f_manage s_manage table_meta constraint btree index_manage $(FLEX_PATH)/Executor.cpp $(FLEX_PATH)/sql_yacc.tab.c $(FLEX_PATH)/lex.yy.c $(QUERY_PATH)/query_manager.cpp
	$(CC) -o $(EXE_DIR)/xql_db $(O_DIR)/r_manage.o $(O_DIR)/SystemManager.o $(O_DIR)/TableMeta.o $(O_DIR)/Constraint.o $(O_DIR)/BplusTree.o $(FLEX_PATH)/Executor.cpp $(FLEX_PATH)/sql_yacc.tab.c $(FLEX_PATH)/lex.yy.c $(QUERY_PATH)/query_manager.cpp

# tests
r_m_test: r_manage $(REC_PATH)/RecordManagerTester.h $(PWD)/test.cpp
	$(CC) -o $(EXE_DIR)/r_m_test $(O_DIR)/r_manage.o $(PWD)/test.cpp

cond_test: 
	$(CC) -o $(EXE_DIR)/cond_test $(PWD)/test.cpp 

table_meta_test: table_meta constraint serializer
	$(CC) -o $(EXE_DIR)/table_meta_test $(O_DIR)/TableMeta.o $(O_DIR)/Constraint.o $(O_DIR)/Serializer.o $(PWD)/test.cpp

table_index_test: serializer
	$(CC) -o $(EXE_DIR)/table_index_test $(O_DIR)/Serializer.o $(PWD)/test.cpp

s_manager_test: s_manage table_meta r_manage btree
	$(CC) -o $(EXE_DIR)/s_manager_test $(O_DIR)/SystemManager.o $(O_DIR)/r_manage.o $(O_DIR)/TableMeta.o $(O_DIR)/BplusTree.o $(PWD)/test.cpp

tree_test: btree $(IDX_PATH)/IndexTester.h
	$(CC) -o $(EXE_DIR)/tree_test $(O_DIR)/BplusTree.o $(O_DIR)/MyBitMap.o $(PWD)/test.cpp

s_parser_test: statement_parser r_manage f_manage s_manage table_meta constraint btree index_manage $(FLEX_PATH)/Executor.cpp $(FLEX_PATH)/sql_yacc.tab.c $(FLEX_PATH)/lex.yy.c $(QUERY_PATH)/query_manager.cpp
	$(CC) -o $(EXE_DIR)/s_parser_test $(O_DIR)/r_manage.o $(O_DIR)/SystemManager.o $(O_DIR)/TableMeta.o $(O_DIR)/Constraint.o $(O_DIR)/BplusTree.o $(FLEX_PATH)/Executor.cpp $(FLEX_PATH)/sql_yacc.tab.c $(FLEX_PATH)/lex.yy.c $(QUERY_PATH)/query_manager.cpp

# record management
r_manage: serializer record f_manage $(REC_PATH)/RecordManager.cpp $(REC_PATH)/RecordManager.h
	$(CC) -o $(O_DIR)/RecordManager.o -c $(REC_PATH)/RecordManager.cpp
	$(Link) -relocatable -r -s -o $(O_DIR)/r_manage.o $(O_DIR)/RecordManager.o $(O_DIR)/Serializer.o $(O_DIR)/Record.o $(O_DIR)/MyBitMap.o


serializer: $(REC_PATH)/Serializer.cpp $(REC_PATH)/Serializer.h
	$(CC) -o $(O_DIR)/Serializer.o -c $(REC_PATH)/Serializer.cpp

record: $(REC_PATH)/Record.cpp $(REC_PATH)/Record.h $(REC_PATH)/Record_Item.h $(REC_PATH)/Rid.h
	$(CC) -o $(O_DIR)/Record.o -c $(REC_PATH)/Record.cpp


# file management
f_manage: $(FIL_PATH)/utils/MyBitMap.cpp $(FIL_PATH)/utils/MyBitMap.h
	$(CC) -o $(O_DIR)/MyBitMap.o -c $(FIL_PATH)/utils/MyBitMap.cpp


# system management
s_manage: $(SYS_PATH)/SystemManager.cpp
	$(CC) -o $(O_DIR)/SystemManager.o -c $(SYS_PATH)/SystemManager.cpp

table_meta:	$(SYS_PATH)/TableMeta.cpp
	$(CC) -o $(O_DIR)/TableMeta.o -c $(SYS_PATH)/TableMeta.cpp

constraint: $(SYS_PATH)/Constraint.cpp
	$(CC) -o $(O_DIR)/Constraint.o -c $(SYS_PATH)/Constraint.cpp

# index management
index_manage: btree

btree: $(BTREE_SUBPATH)/Tree.cpp f_manage
	$(CC) -o $(O_DIR)/BplusTree.o -c $(BTREE_SUBPATH)/Tree.cpp

# statement_parser
statement_parser:
	cd $(FLEX_PATH) && lex $(FLEX_PATH)/sql_lex.l
	cd $(FLEX_PATH) && bison $(FLEX_PATH)/sql_yacc.y -d

init:
	rm -rf build
	rm -rf obj
	mkdir build
	mkdir obj

# -fexec-charset=GBK -finput-charset=UTF-8