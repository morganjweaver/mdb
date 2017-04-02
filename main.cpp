#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include "SQLParser.h"
//#include "sqlhelper.h"
//#include "SQLprinter.h"
#include "db_cxx.h"
#include "database.h"
#include "sql/CreateStatement.h"
#include "sql/Expr.h"


void printExpression(const hsql::Expr* ex);
void printCreate(const hsql::CreateStatement* stmt);
void printSelect(const hsql::SelectStatement* stmt);
void printOpEx(const hsql::Expr* ex);
void printTableRef(const hsql::TableRef* tbl);
void printColDef(const hsql::ColumnDefinition* col);
//STILL TO DO?
//  void printImportStatementInfo(const ImportStatement* stmt, uintmax_t numIndent)
//  void printInsertStatementInfo(const InsertStatement* stmt, uintmax_t numIndent)
//  void printStatementInfo(const SQLStatement* stmt) {


void Execute(const ::hsql::SQLStatement* result){
    switch (result->type()) {
        case hsql::StatementType::kStmtCreate:
            printCreate((const hsql::CreateStatement*) result);
            break;
        case hsql::StatementType::kStmtSelect:
            printSelect((const hsql::SelectStatement*) result);
            break;
        default:
        std::cout << "Couldn't match statement type";
            break;
    }
}

void printCreate(const hsql::CreateStatement* stmt) {
    std::cout << "CREATE TABLE " << stmt->tableName << " ";
    printf("(");
    for(int i = 0; i<stmt->columns->size()-1; i++){
        printColDef(stmt->columns->at(i));
        printf(", ");
    }
    printColDef(stmt->columns->at(stmt->columns->size()-1));
    printf(")");
}
void printColDef(const hsql::ColumnDefinition* col){
    printf("%s ", col->name );
    switch (col->type) {
        case hsql::ColumnDefinition::UNKNOWN:
            printf("UNKNOWN");
            break;
        case hsql::ColumnDefinition::TEXT:
            printf("TEXT");
            break;
        case hsql::ColumnDefinition::INT:
            printf("INT");
            break;
        case hsql::ColumnDefinition::DOUBLE:
            printf("DOUBLE");
            break;
        default:
            printf("Unknown type");
            return;
    }
}

void printSelect(const hsql::SelectStatement* stmt) {
    std::cout << "SELECT ";
    for (int i = 0; i < stmt->selectList->size() - 1; i++) {
        printExpression(stmt->selectList->at(i));
        printf(", ");
    }
    printExpression(stmt->selectList->at(stmt->selectList->size() - 1));
    printf(" ");
        printf("FROM ");
        printTableRef(stmt->fromTable);
        printf(" ");

        if (stmt->whereClause != NULL) {
            printf("WHERE ");
            printExpression(stmt->whereClause);
            printf(" ");
        }

        if (stmt->unionSelect != NULL) {
            printf("UNION ");
            printSelect(stmt->unionSelect);
            printf(" ");
        }

        if (stmt->order != NULL) {
            printf("ORDER BY ");
            printExpression(stmt->order->at(0)->expr);
            printf(" ");
            if (stmt->order->at(0)->type == hsql::kOrderAsc) printf("ASCENDING ");
            else printf("DESCENDING ");
        }

        if (stmt->limit != NULL) {
            printf("LIMIT ");
            printf("%" PRId64, stmt->limit->limit);
            printf(" ");
        }
    }

void printExpression(const hsql::Expr* ex){

    switch(ex->type){
        case hsql::kExprLiteralFloat:
            printf("%f ", ex->fval);
            break;
        case hsql::kExprColumnRef:
            if(ex->table != NULL){printf("%s.",ex->table);}
            printf("%s", ex->name);
            break;
        case hsql::kExprLiteralString:
            printf("%s ", ex->name);
            break;
        case hsql::kExprLiteralInt:
            printf("% " PRId64, ex->ival);
            break;
        case hsql::kExprStar:
            printf("* ");
            break;
        case hsql::kExprFunctionRef:
            printf("%s %s ", ex->name, ex->expr->name);
            break;
        case hsql::kExprOperator:
            printOpEx(ex);
            break;
        case hsql::kExprSelect:
        default:
            printf("Unrecognized expression: %d\n", ex->type);
            return;
    }
    if(ex->alias != NULL){
        printf("AS %s ", ex->alias);
    }
}

void printOpEx(const hsql::Expr* ex) {
    if (ex == NULL) {
        printf("null");
        return;
    }
    printExpression(ex->expr);
    printf(" ");
    switch (ex->opType) {
        case hsql::Expr::SIMPLE_OP:
            printf("%c", ex->opChar);
            break;
        case hsql::Expr::AND:
            printf("AND");
            break;
        case hsql::Expr::OR:
            printf("OR");
            break;
        case hsql::Expr::NOT:
            printf("NOT");
            break;
        default:
            printf("%u", ex->opType);
            break;
    }
    printf(" ");

if(ex->expr2!= NULL) printExpression(ex->expr2);
}

void printTableRef(const hsql::TableRef* tbl){
    switch (tbl->type) {
        case hsql::kTableName:
            printf("%s ", tbl->name);
            break;
        case hsql::kTableSelect:
            printSelect(tbl->select);
            break;
        case hsql::kTableJoin:

            printTableRef(tbl->join->left);
            switch(tbl->join->type) {
                case (hsql::kJoinLeft):
                    printf("LEFT JOIN ");

                    break;
                case(hsql::kJoinRight):
                    printf("RIGHT JOIN ");

                    break;
                default:
                    printf("JOIN ");
            }
            printTableRef(tbl->join->right);

            printf("ON ");
            printExpression(tbl->join->condition);
            printf(" ");
            break;
        case hsql::kTableCrossProduct:
            for (int i = tbl->list->size()-1; i>0; i--){
                printTableRef(tbl->list->at(i));
                printf(", ");
            }
            printTableRef(tbl->list->at(0));
            //for (hsql::TableRef* tbl : *tbl->list) printTableRef(tbl);
            break;
    }
    if (tbl->alias != NULL) {
        printf("AS ");
        printf("%s ",tbl->alias);
    }
}



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
            Execute(test);
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

