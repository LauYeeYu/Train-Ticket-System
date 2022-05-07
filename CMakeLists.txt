cmake_minimum_required(VERSION 3.16)
project(TicketSystem)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g")

set(TICKET_INCLUDES
        ${ticket_SOURCE_DIR}/include)

set(TICKET_SOURCES)

add_executable(code ${TICKET_SOURCES} ${TICKET_INCLUDES})
target_include_directories(code PRIVATE ${TICKET_INCLUDES})
