/**
 * Storage engine abstract classes.
 * DbBlock
 * DbFile
 * DbRelation
 *
 * @author Kevin Lundeen
 * @version 1.0, 04/04/2017
 * For CPSC4300/5300 S17, Seattle University
 *
 */
#pragma once

#include <exception>
#include <map>
#include <utility>
#include <vector>
#include "db_cxx.h"

extern DbEnv* _DB_ENV;
const uint DB_BLOCK_SZ = 4096;
typedef u_int16_t RecordID;
typedef u_int32_t BlockID;
typedef std::vector<RecordID> RecordIDs;
typedef std::length_error DbBlockNoRoomError;

class DbBlock {
public:
	DbBlock(Dbt &block, BlockID block_id, bool is_new=false) : block(block), block_id(block_id) {}
	virtual ~DbBlock() {}

	virtual void initialize_new() {}
	virtual RecordID add(const Dbt* data) throw(DbBlockNoRoomError) = 0;
	virtual Dbt* get(RecordID record_id) = 0;
	virtual void put(RecordID record_id, const Dbt &data) throw(DbBlockNoRoomError) = 0;
	virtual void del(RecordID record_id) = 0;
	virtual RecordIDs* ids() = 0;

	virtual Dbt* get_block() {return &block;}
	virtual void* get_data() {return block.get_data();}
	virtual BlockID get_block_id() {return block_id;}

protected:
	Dbt block;
	BlockID block_id;
};

typedef std::vector<BlockID> BlockIDs;  // FIXME: will need to turn this into an iterator at some point

class DbFile {
public:
	DbFile(std::string name) : name(name) {}
	virtual ~DbFile() {}

	virtual void create() = 0;
	virtual void drop() = 0;
	virtual void open() = 0;
	virtual void close() = 0;
	virtual DbBlock* get_new() = 0;
	virtual DbBlock* get(BlockID block_id) = 0;
	virtual void put(DbBlock* block) = 0;
	virtual BlockIDs* block_ids() = 0;

protected:
	std::string name;
};

class ColumnAttribute {
public:
	enum DataType {
		INT,
		TEXT
	};
	ColumnAttribute(DataType data_type) : data_type(data_type) {}
	virtual ~ColumnAttribute() {}

	virtual DataType get_data_type() { return data_type; }
	virtual void set_data_type(DataType data_type) {this->data_type = data_type;}

protected:
	DataType data_type;
};

class Value {
public:
	ColumnAttribute::DataType data_type;
	int32_t n;
	std::string s;

	Value() : n(0) {data_type = ColumnAttribute::INT;}
	Value(int32_t n) : n(n) {data_type = ColumnAttribute::INT;}
	Value(std::string s) : s(s) {data_type = ColumnAttribute::TEXT; }
};

typedef std::string Identifier;
typedef std::vector<Identifier> ColumnNames;
typedef std::vector<ColumnAttribute> ColumnAttributes;
typedef std::pair<BlockID, RecordID> Handle;
typedef std::vector<Handle> Handles;  // FIXME: will need to turn this into an iterator at some point
typedef std::map<Identifier, Value> ValueDict;

class DbRelationError : public std::runtime_error {
public:
	explicit DbRelationError(std::string s) : runtime_error(s) {}
};

class DbRelation {
public:
	DbRelation(Identifier table_name, ColumnNames column_names, ColumnAttributes column_attributes ) :
		table_name(table_name), column_names(column_names), column_attributes(column_attributes) {}
	virtual ~DbRelation() {}

	virtual void create() = 0;
	virtual void create_if_not_exists() = 0;
	virtual void drop() = 0;

	virtual void open() = 0;
	virtual void close() = 0;

	virtual Handle insert(const ValueDict* row) = 0;
	virtual void update(const Handle handle, const ValueDict* new_values) = 0;
	virtual void del(const Handle handle) = 0;

	virtual Handles* select() = 0;
	virtual Handles* select(const ValueDict* where) = 0;
	virtual ValueDict* project(Handle handle) = 0;
	virtual ValueDict* project(Handle handle, const ColumnNames* column_names) = 0;

protected:
	Identifier table_name;
	ColumnNames column_names;
	ColumnAttributes column_attributes;
};
