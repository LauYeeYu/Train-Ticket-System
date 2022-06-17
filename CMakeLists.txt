cmake_minimum_required(VERSION 3.16)
project(TicketSystem)
include(CTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Ofast")

if(DEFINED ROLLBACK)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DROLLBACK")
endif()

if(DEFINED PRETTY_PRINT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPRETTY_PRINT")
elseif(DEFINED BOOST)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST")
endif()

set(TICKET_INCLUDES
        include)

set(TICKET_SOURCES
        src/main.cpp
        src/parameter_table.cpp
        src/token_scanner.cpp
        src/train.cpp
        src/train_manage.cpp
        src/user_manage.cpp
        src/utility.cpp)

add_executable(train-ticket-system ${TICKET_SOURCES} ${TICKET_INCLUDES})
target_include_directories(train-ticket-system PRIVATE ${TICKET_INCLUDES})
