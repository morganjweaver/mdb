//
// Created by Morgan Weaver on 4/1/17.
//
#include<iostream>
#include "SQLParser.h"
#include "sql/SQLStatement.h"

#ifndef SQL_INTERPRETER_SQLPRINTER_H
#define SQL_INTERPRETER_SQLPRINTER_H
namespace mdb {
    std::string Execute(const hsql::SQLStatement* result);
}
#endif //SQL_INTERPRETER_SQLPRINTER_H
