#include <stdlib.h>
#include <iostream>
#include "SQLParser.h"
#include "SQLprinter.h"




int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./sql_interpreter path/data");//./example \"SELECT * FROM test;\"\n");
        return -1;
    }
    mdb::Database* d = new mdb::Database(argv[1]);
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
    }
}

