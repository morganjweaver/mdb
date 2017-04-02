# build instructions

This program was created in CLion but can also be make in cmake.

To build with cmake, type `cmake build`

To generate a Unix makefile, type `cmake -G "Unix Makefiles" .`

To build with make, type `make all`

## To modify for your system:

  * modify CMakeList to include your sql-parser directory and bdb directories
  * then rebuild with `cmake -G "Unix Makefiles" .`

## Usage:

`./sql_interpreter /YourDirectory/sql4300env/data`
