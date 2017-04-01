//
// Created by Morgan on 3/31/2017.
//

#ifndef SQL_INTERPRETER_DB_H
#define SQL_INTERPRETER_DB_H
namespace mdb {
    class Database {

    public:

        Database(std::string dir_name, std::string db_name);

        int InitializeDB(std::string path);

    private:
        std::string db_name = "";
        std::string dir_name = "";
//const char *HOME = "cpsc4300_5300/data";
//const char *EXAMPLE = "example.db";
    };
} //namespace mdb
#endif //SQL_INTERPRETER_DB_H

