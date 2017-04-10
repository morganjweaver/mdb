#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include "heap_storage.h"
#include "storage_engine.h"


//SlottedPage functions here:
SlottedPage::SlottedPage(Dbt &block, BlockID block_id, bool is_new): DbBlock(block, block_id, is_new){
    if (is_new) {
        this->num_records = 0;
        //set new SlottedPg with no records
        this->end_free = DB_BLOCK_SZ - 1; //set free end to very end of total block size, 0-indexed
        put_header(); //load the block header by leaving args empty
    }
        else{ //do this if NOT new:
            get_header(this->num_records, this->end_free);
        }
}

///ADD SPECIFIC SLOTTED BLOCK PUT_HEADER
//PUBLIC SlottedPage Functions:

RecordID SlottedPage::add(const Dbt* data) throw (DbBlockNoRoomError){
    if(has_room((u_int16_t)data->get_size())){
        this->end_free -= (u_int16_t)data->get_size();
        put_header();//load block header
        this->num_records++;
        put_header(this->num_records, (u_int16_t)data->get_size(), this->end_free+(u_int16_t)1);
        std::memcpy(this->address(end_free+(u_int16_t)1), data->get_data(), (u_int16_t)data->get_size());
        return this->num_records;
    } else{
        throw DbBlockNoRoomError("");
    }
 }
//Note: uses get_header which passes by REFERENCE instead of value for args
Dbt* SlottedPage::get(RecordID record_id) {
    u_int16_t test_size;
    u_int16_t test_loc;
    //create vars to hold return values IF SUCCESS as get_header takes args by ref
    get_header(test_size, test_loc, record_id);
    if (test_loc == 0)
        return nullptr;
        //not a success, no record
    else { //on success, vars will be populated
        return new Dbt(this->address(test_loc), test_size);
    }
}

//For REPLACING/UPDATING, not adding
void SlottedPage::put(RecordID record_id, const Dbt &data)throw(DbBlockNoRoomError){
    u_int16_t current_size;
    u_int16_t current_loc;
    //first find the record
    get_header(current_size, current_loc, record_id);
    u_int16_t updated_size = (u_int16_t) data.get_size();
    if(current_size > updated_size){ //if new size is SMALLER
        std::memcpy(this->address(current_loc), data.get_data(), updated_size);
        slide(current_loc+updated_size, current_loc+current_size);
    } else{ //if new size is LARGER
        u_int16_t delta = updated_size - current_size;
        //now check to see if there is room for new record update
        if(!has_room(delta)) {
            throw DbBlockNoRoomError("");
        }
            slide(current_loc, current_loc-delta); //After checking, update by sliding and putting
            std::memcpy(this->address(current_loc-delta), data.get_data(), updated_size); //memcpy to block
        }
    //now update header
    get_header(current_size, current_loc, record_id); //
    put_header(record_id, updated_size, current_loc); //now update header info
    }


void SlottedPage::del(RecordID record_id) {
    u_int16_t del_size;
    u_int16_t del_address; //set up address and size vars
    get_header(del_size, del_address, record_id); //populate them with record info
    put_header(record_id, 0, 0); //zero out the record info for future searches
    slide(del_address, del_address+del_size); //overwrite it

 }

RecordIDs* SlottedPage::ids(void) { //catalogues all legitimate records, skipping IDs with no data
    u_int16_t size;
    u_int16_t address; //these will hold the record info for get_header
    RecordIDs *IDvector =  new RecordIDs();
    for(int i = 1; i <= this->num_records; i++){
        get_header(size, address, i);
        if(address!=0){ IDvector->push_back(i);}
    }
return IDvector;
 }

//PRIVATE SlottedPage functions: MAKING MANY ASSUMPTIONS HERE

//Takes by REFERENCE, instead of returning updates args with size and address
 void SlottedPage::get_header(u_int16_t &size, u_int16_t &loc, RecordID id) {
    //DISCUSSED WITH STUDY GROUP:
    //populate size with contents of pointer to size portion of header
    size = get_n((u_int16_t)4*id);
    //now populate loc with offset
    loc = get_n((u_int16_t)2+((u_int16_t)4*id));
}
 void SlottedPage::put_header(RecordID id, u_int16_t size, u_int16_t loc) {
    //if 0, just update the main block header:

     if(id==0) {

         put_n((u_int16_t) 4 * id, this->num_records);
         put_n((u_int16_t) 4 * id + (u_int16_t) 2, this->end_free);
     } else {
         put_n((u_int16_t)4*id, size);
         put_n((u_int16_t)2+((u_int16_t)4*id), loc);
     }
 }

 bool SlottedPage::has_room(u_int16_t size) {
     //check desired size against available, calc free space by subtracting record id info from free end ptr
     return size <= (u_int16_t) this->end_free - 4 * (this->num_records + 1);
 }
//overwrite region of block by sliding data up block to increase free space.  Can ALSO be used to MAKE ROOM for data
//that needs to be inserted!  start > end or end > start is indicator of this.
 void SlottedPage::slide(u_int16_t start, u_int16_t end) {
    int delta = end - start;
    if(delta==0){ //Gah!  Added no-difference check.
        return; }
    int size_in_bytes = start - (this->end_free + 1);
    char tempBuf[size_in_bytes];
    void* to_addr = this->address(this->end_free + (u_int16_t)1 + (u_int16_t)delta);//calculate new address
    void* from_addr = this->address(this->end_free + (u_int16_t)1);
    std::memcpy(tempBuf, from_addr, (size_t)size_in_bytes );//copy to temp buffer
    std::memcpy(to_addr, tempBuf, (size_t)size_in_bytes);//copy into block

    //give new locations to headers that were actually effected
    //GET RECORD ID VECTOR:
    RecordIDs* to_update = ids();
    //now update the headers:
    for (int i = 0; i< to_update->size(); i++ ){
        u_int16_t address;
        u_int16_t size;
        get_header(size, address, to_update->at(i));
        if (address <= start){
            address+=delta;
            put_header(to_update->at(i), size, address);
        }
    }
    //now adjust pointer to free space
    this->end_free+=delta;
    put_header();//clean up first directory header
    delete(to_update); //free up dynamically alloc. mem
}

//NEXT 3 WERE GIVEN BY PROFESSOR LUNDEEN ON CANVAS:

// Get 2-byte integer at given offset in block.
// RETURNS ADDRESS
u_int16_t SlottedPage::get_n(u_int16_t offset) {
    return *(u_int16_t*)this->address(offset);
}
//PLACES NEW ADDRESS
// Put a 2-byte integer at given offset in block.
void SlottedPage::put_n(u_int16_t offset, u_int16_t n) {
    *(u_int16_t*)this->address(offset) = n;
}
//mentioned in class--returns pointer into block based on offset given
// Get a void* pointer into the data block.
void* SlottedPage::address(u_int16_t offset) {
    return (void*)((char*)this->block.get_data() + offset);
}

//--------------------------------------------------------------------------------------------------
//HeapFile Functions:


void HeapFile::create() {
    db_open(DB_CREATE|DB_EXCL);
    //create DB, check to see if already exists
    get_new();//from BerkeleyDB
}
void HeapFile::drop() {
    close();
    Db db(_DB_ENV, 0);
    db.remove(this->dbfilename.c_str(), nullptr, 0);
}
void HeapFile::close() {
    this->db.close(0);
    //now set the bool
    this->closed = true;
}
void HeapFile::open() {
    db_open();
}

SlottedPage* HeapFile::get_new() {
    //from storage engine.h file! 4096.
    char db_block[DB_BLOCK_SZ];
    //create memory space
    std::memset(db_block, 0, sizeof(db_block));
    //Create new Dbt of block size
    Dbt data(db_block, sizeof(db_block));
    //increment id
    int block_id = ++this->last;
    //create key for data
    Dbt key(&block_id, sizeof(block_id));

    // Set BDB to manage memory by writing out a block and reading back in:
    this->db.put(nullptr, &key, &data, 0);
    this->db.get(nullptr, &key, &data, 0);
    //create new page from the data,
    SlottedPage* slotted = new SlottedPage(data, this->last, true);
    this->db.put(nullptr, &key, &data, 0); // put again w/ init and return
    return slotted;
}
void HeapFile::put(DbBlock* put_block) {
    //first grab block ID from block to be placed
    int id = put_block->get_block_id();
    //create key
    Dbt key(&id, sizeof(id));
    //now use BDB's put to place the block with its ID in the BDB db
    this->db.put(nullptr, &key, put_block->get_block(), 0);
}

SlottedPage* HeapFile::get(BlockID b_id) {
    Dbt lookup_key(&b_id, sizeof(b_id));
    Dbt return_data;
    this->db.get(nullptr, &lookup_key, &return_data, 0);
    //now return the data as a slotted page, already existant, with accompanying lookup key/ID
    return new SlottedPage(return_data, b_id, false);
}

//push all the
BlockIDs* HeapFile::block_ids() {
    //create a vector
    BlockIDs* ID_vector = new BlockIDs();
    //push
    for (int i = 1; i <= (this->last); i++){
        ID_vector->push_back(i);
    }
    return ID_vector;
}
u_int32_t HeapFile::get_last_block_id(void) {return this->last;}

//-------------------------------------------------------------------------------------------------
//HeapTable Implementation:
HeapTable::HeapTable(Identifier table_name, ColumnNames column_names, ColumnAttributes column_attributes ) :
DbRelation(table_name, column_names, column_attributes), file(table_name) {} //file at end is HeapFile file from .h!

//PUBLIC functions per .h:

 void HeapTable::create() { //basic SQL create table BLAH (columns) command
    file.create();
 }
 void HeapTable::create_if_not_exists(){
    open();
}
 void HeapTable::drop(){
    file.drop();//form Berkeley defs
}

 void HeapTable::open(){
    file.open(); //basic open
}
 void HeapTable::close(){
    file.close();//wrapper for Berkeley def
}
//HANDLE def: std::pair<BlockID, RecordID> Handle;
//typedef std::map<Identifier, Value> ValueDict;
//takes a MAP of Identifier, Value
 Handle HeapTable::insert(const ValueDict* row){
    open();
    ValueDict* valdrow = validate(row);
    Handle h = append(valdrow);
    delete(valdrow);
    return h;
}
 void HeapTable::update(const Handle handle, const ValueDict* new_values){
    throw DbRelationError("Not yet supported");
}
 void HeapTable::del(const Handle handle){
     throw DbRelationError("Not yet supported");
}
//not sure what's going on here?
 Handles* HeapTable::select(){
    ValueDict nil;
    select(&nil);
}
//overloaded
 Handles* HeapTable::select(const ValueDict* where){
    Handles* returnVector = new Handles();
    BlockIDs* fileIDs = file.block_ids();
    //dreaded double for loop:
    for (int i =0; i<fileIDs->size(); i++) {
        SlottedPage* blkptr = file.get(fileIDs->at(i));
        //now have the block ptr
        RecordIDs* recIDs = blkptr->ids();
        //have the records from the block, now populate returnVector
        for(int j = 0; j<recIDs->size(); j++) {
            returnVector->push_back(Handle(i, recIDs->at(i));
        }
    }
    delete(fileIDs);
    return returnVector; //return the populated vec of Handle ptrs extracted
    }
}
 ValueDict* HeapTable::project(Handle handle){ //peel the project funct out of BDB
     return project(handle, &this->column_names);

}
//overload:
 ValueDict* HeapTable::project(Handle handle, const ColumnNames* column_names){
    //set up to return by column name:
    BlockID bID = handle.first;
    RecordID rID = handle.second;
    SlottedPage* blockID = file.get(bID); //first get the SlottedPage pt assoc with the handle location
    Dbt* data = blockID->get(rID);
    ValueDict* row_item = unmarshal(data);
    delete data;
    delete blockID;
    return row_item;
}

//PRIVATE HeapFile Functions per .h:

//Valuedict is identifier, value MAP
//checks that row is OK
ValueDict* HeapTable::validate(const ValueDict* row) {
        for (int i = 0; i<this->column_names.size(); i++) {
        ValueDict::const_iterator col = row->find(this->column_names[i]);
        if (col == row->end())
            throw DbRelationError("ROW must have values for all table columns!");
        }
    return (ValueDict*)row;
}

 Handle HeapTable::append(const ValueDict* row){
    Dbt* temp = marshal(row);
     //per study group discussion--best solution
     SlottedPage* slotted = this->file.get(this->file.get_last_block_id());
     RecordID record;
     try {
         record = slotted->add(temp);
     } catch (DbBlockNoRoomError&) {
         //on fail, create new block and add the row there
         slotted = this->file.get_new();
         record = slotted->add(temp);
     }
     this->file.put(slotted);
     delete[] (char*)temp->get_data();
     delete temp;
     Handle ret = Handle(this->file.get_last_block_id(), record);
     return ret;
}

//Iterate through all columns in row and transform into binary representation respective of type
 Dbt* HeapTable::marshal(const ValueDict* row){
    char* temp = new char[DB_BLOCK_SZ];
    u_int16_t marker = 0;
    for (int i = 0; i<this->column_names.size(); i++) {
        ColumnAttribute attr = this->column_attributes[i];
        std::string name = this->column_names[i];
        Value val = row->find(name)->second;
        if(attr.get_data_type() == ColumnAttribute::DataType::INT) {
            memcpy((void*)temp + marker, &val.n, sizeof(val.n));
            marker += sizeof(val.n);
        } else if(attr.get_data_type() == ColumnAttribute::DataType::TEXT) {
            u_int16_t stringsize = (u_int16_t)val.s.size();
            memcpy((void*)temp + marker, (void*)&stringsize, sizeof(stringsize));
            marker += sizeof(stringsize);
            memcpy((void*)temp + marker, val.s.c_str(), val.s.size());
            marker += sizeof(val.s.size());
        } else { throw DbRelationError("unsupported type"); }
    }
    Dbt* retrn_data = new Dbt(temp, DB_BLOCK_SZ); //bigger than neccessary, but simple
    return retrn_data; //TO DO: trim into smaller block to save memory
}
 ValueDict* HeapTable::unmarshal(Dbt* data){
     ValueDict *row = new ValueDict();
     Value temp;
     u_int16_t  marker = 0;
     char *marshalled = (char*)data->get_data();
     for (int i = 0; i<this->column_names.size(); i++) {
         ColumnAttribute attr = this->column_attributes[i];
         std::string name = this->column_names[i];

         if (attr.get_data_type() == ColumnAttribute::DataType::INT) {
             memcpy(&temp.n, marshalled+marker, sizeof(temp.n));
             marker += sizeof(temp.n);
         } else if (attr.get_data_type() == ColumnAttribute::DataType::TEXT) {
             u_int16_t stringsize;
             memcpy(&stringsize, marshalled+marker, sizeof(stringsize));
             marker += sizeof(stringsize);
             char* temps = new char[stringsize];
             memcpy(temps, marshalled+marker, stringsize);
             marker += stringsize;
             temp.s = std::string(temps);
         } else { throw DbRelationError("unsupported type"); }
         (*row)[name] = temp;
     }
    return row;
 }
