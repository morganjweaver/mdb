#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdint>
#include "SQLParser.h"
#include "sqlhelper.h"
#include "db_cxx.h"
#include "database.h"


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Usage: ./example \"SELECT * FROM test;\"\n");
        return -1;
    }
    std::string query = argv[1];

    // parse a given query
    hsql::SQLParserResult* result = hsql::SQLParser::parseSQLString(query);

    // check whether the parsing was successful
    if (result->isValid()) {
        printf("Parsed successfully!\n");
        printf("Number of statements: %lu\n", result->size());

        for (unsigned i = 0; i < result->size(); ++i) {
            // Print a statement summary.
            hsql::printStatementInfo(result->getStatement(i));
        }

        delete result;
        return 0;
    } else {
        fprintf(stderr, "Given string is not a valid SQL query.\n");
        fprintf(stderr, "%s (L%d:%d)\n",
                result->errorMsg(),
                result->errorLine(),
                result->errorColumn());
        delete result;
        return -1;
    }
}
