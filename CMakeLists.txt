cmake_minimum_required(VERSION 3.16)
project(TicketSystem)
include(CTest)

set(CMAKE_CXX_STANDARD 17)

set(TICKET_INCLUDES
        ${ticket_SOURCE_DIR}/include)

set(TICKET_SOURCES
        src/main.cpp
        src/parameter_table.cpp
        src/token_scanner.cpp
        src/train.cpp
        src/train_manage.cpp
        src/user_manage.cpp
        src/utility.cpp)

add_executable(code ${TICKET_SOURCES} ${TICKET_INCLUDES})
target_include_directories(code PRIVATE ${TICKET_INCLUDES})
