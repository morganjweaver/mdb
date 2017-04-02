cmake_minimum_required(VERSION 3.6)
project(sql_interpreter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp database.h database.cpp SQLprinter.cpp SQLprinter.h)
link_directories(~/Documents/4300_DB/sql-parser)
link_libraries(sqlparser)
include_directories(~/Documents/4300_DB/sql-parser/src)
link_libraries(db_cxx) #libname
link_directories(/usr/local/BerkeleyDB.6.2/lib)  #//where to find libs
include_directories(/usr/local/BerkeleyDB.6.2/include) #where to find header files
add_executable(sql_interpreter ${SOURCE_FILES})
