//
// Created by Morgan on 3/31/2017.
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned long u_long;
typedef unsigned int u_int;
#include "db_cxx.h"
#include "database.h"
#include <sys/stat.h>
// CREATE A DIRECTORY IN YOUR HOME DIR ~/cpsc4300_5300/data before running this
namespace mdb{

    const unsigned int BLOCK_SZ = 4096;
    Database::Database(std::string dir_name) {
        this->dir_name = dir_name;
    }
    bool Database::InitializeDB() {
        struct stat sb;

        if (stat(this->dir_name.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        {

            const char *EXAMPLE = "example.db";
            const char *home = std::getenv("HOME");
            const unsigned int BLOCK_SZ = 4096;

            DbEnv env(0U);
            env.set_message_stream(&std::cout);
            env.set_error_stream(&std::cerr);
            env.open(this->dir_name.c_str(), DB_CREATE | DB_INIT_MPOOL, 0);

            Db db(&env, 0);
            db.set_message_stream(env.get_message_stream());
            db.set_error_stream(env.get_error_stream());
            db.set_re_len(BLOCK_SZ); // Set record length to 4K
            db.open(NULL, EXAMPLE, NULL, DB_RECNO, DB_CREATE | DB_TRUNCATE, 0644); // Erases anything already there

            char block[BLOCK_SZ];
            Dbt data(block, sizeof(block));
            int block_number;
            Dbt key(&block_number, sizeof(block_number));
            block_number = 1;
            //strcpy(block, "hello!");
            db.put(NULL, &key, &data, 0);  // write block #1 to the database

            Dbt rdata;
            db.get(NULL, &key, &rdata, 0); // read block #1 from the database
            //std::cout << "Read (block #" << block_number << "): '" << (char *)rdata.get_data() << "'" << std::endl;

            return true;
        }
        else {
            std::cout << "Invalid path.  Please create " << this->dir_name << " directory and retry.";
            return false;
        }
}
    void Database::printDBInfo(){
        printf("\nWelcome to mdb. Directory: %s\n", this->dir_name.c_str() );
    }
} // namespace mdb

