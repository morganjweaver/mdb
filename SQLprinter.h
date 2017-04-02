//
// Created by Morgan Weaver on 4/1/17.
//
#ifndef SQL_INTERPRETER_SQLPRINTER_H
#define SQL_INTERPRETER_SQLPRINTER_H

#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include "db_cxx.h"
#include "database.h"
#include "SQLParser.h"
#include "sql/SQLStatement.h"


namespace mdb {
    void Execute(const ::hsql::SQLStatement* result);
    void printExpression(const hsql::Expr* ex);
    void printCreate(const hsql::CreateStatement* stmt);
    void printSelect(const hsql::SelectStatement* stmt);
    void printOpEx(const hsql::Expr* ex);
    void printTableRef(const hsql::TableRef* tbl);
    void printColDef(const hsql::ColumnDefinition* col);
}
#endif //SQL_INTERPRETER_SQLPRINTER_H
