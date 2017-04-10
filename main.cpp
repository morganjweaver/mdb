#include <stdlib.h>
#include <iostream>
#include "SQLParser.h"
#include "heap_storage.h"
#include <vector>
#include <string>

//const char *HOME = "cpsc4300_5300/data";
//const char *EXAMPLE = "example.db";
DbEnv* _DB_ENV;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./sql_interpreter path/data");//./example \"SELECT * FROM test;\"\n");
        return -1;
    }
    char *envHome = argv[1];
    std::cout << "(sql4300: running with database environment at " << envHome
              << ")" << std::endl;
    DbEnv env(0U);
    env.set_message_stream(&std::cout);
    env.set_error_stream(&std::cerr);

    try {
        env.open(envHome, DB_CREATE | DB_INIT_MPOOL, 0);
    } catch (DbException& exc) {
        std::cerr << "(sql4300: " << exc.what() << ")";
        exit(1);
    }
    _DB_ENV = &env;
    std::cout << "About to create DB SLotted page and add items";
    unsigned char testBuf[4096];
    Dbt test(&testBuf, 4096);

    SlottedPage testSlotted(test,1,true);

    std::vector<std::string> testStrings;
    std::vector<uint16_t> ids;
    testStrings.push_back("Banana");
    testStrings.push_back("Travis");
    testStrings.push_back("Ginsburg");
    for (auto& p : testStrings) {

        const Dbt x(&p, sizeof(p));

        ids.push_back(testSlotted.add(&x));
    }
    std::cout<< "SlottedPage instance contains:" << std::endl;
    for (auto& p : ids) {
        std::cout << *((std::string*)testSlotted.get(p)->get_data()) << std::endl;
    }
    std::cout << "Replacing second data instance with Totoro\n";
    std::string T = "Totoro";
    const Dbt y(&T, sizeof(T));
    testSlotted.put(ids[1], y);

    for (auto& p : ids) {
        std::cout << *((std::string*)testSlotted.get(p)->get_data()) << std::endl;
    }
    std::cout << "Deleting first value:\n";
    testSlotted.del(ids[0]);
    ids.erase(ids.begin());
    std::cout<< "SlottedPage instance now contains:\n" << std::endl;
    for (auto& p : ids) {
        std::cout << *((std::string*)testSlotted.get(p)->get_data()) << std::endl;
    }


   /* mdb::Database* d = new mdb::Database(argv[1]);
    if(!(d->InitializeDB())){
        return -1;
    }
    bool running = true;
    d->printDBInfo();
    while (running){
        char query[512];
        std::cout << "\nSQL> ";
        std::cin.getline(query, 512);
        if (std::cin.eof() | (std::strcmp(query, "quit") == 0))
        {
            return 0;
        }

        hsql::SQLParserResult* result = hsql::SQLParser::parseSQLString(query);

        // check whether the parsing was successful
        if (result->isValid()) {
            const hsql::SQLStatement* test = result->getStatement(0);
            mdb::Execute(test);
        } else{
            std::cerr <<  "Given string is not a valid SQL query.\n";
            fprintf(stderr, "%s (L%d:%d)\n",
                    result->errorMsg(),
                    result->errorLine(),
                    result->errorColumn());
            delete result;
        }
    }*/
}

