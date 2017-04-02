//
// Created by Morgan Weaver on 4/1/17.
//
#include<iostream>
#include "SQLParser.h"

namespace mdb {
    void printExpression(const hsql::Expr* expr);
    void printCreate(const hsql::CreateStatement* stmt);
    void printSelect(const hsql::SelectStatement* stmt);
    //std::void printInsert(const hsql::InsertStatement* stmt);

    void Execute(const ::hsql::SQLStatement* result){
        switch (result->type()) {
            case hsql::StatementType::kStmtCreate:
                printCreate((const hsql::CreateStatement*) result);
                break;
            case hsql::StatementType::kStmtSelect:
                printSelect((const hsql::SelectStatement*) result);
                break;
//            case hsql::StatementType::kStmtInsert:
//                printInsert((const hsql::InsertStatement*) result);
//                break;
            default:
                break;
        }
    }

    void printExpression(const hsql::Expr* ex) {

    }
    void printCreate(const hsql::CreateStatement* stmt) {
        std::cout << "CREATE" << stmt->tableName;
        std::cout << "filepath test" << stmt->filePath;
    }
    void printSelect(const hsql::SelectStatement* stmt) {
        std::cout << "SELECT" << "fields test:" << stmt->selectList;
        std::cout << "sources test:" << stmt->fromTable;
    }

} //namespace mdb

