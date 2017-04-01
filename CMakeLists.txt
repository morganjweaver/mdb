cmake_minimum_required(VERSION 3.6)
project(sql_interpreter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp database.h database.cpp)
add_executable(sql_interpreter ${SOURCE_FILES})
link_directories(/cygdrive/c/Users/Morgan/Documents/Master's\ HW/CPSC4300-DB/sql-parser)
link_libraries(sqlparser)
include_directories(/cygdrive/c/Users/Morgan/Documents/Master's\ HW/CPSC4300-DB/sql-parser/src)
link_libraries(db_cxx) #libname
link_directories(/usr/local/BerkeleyDB.6.2/lib)  #//where to find libs
include_directories(/usr/local/BerkeleyDB.6.2/include) #where to find header files