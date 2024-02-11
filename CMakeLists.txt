cmake_minimum_required(VERSION 3.0)
project(MatteMEG)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CERN ROOT
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(/opt/homebrew/Cellar/nlohmann-json/3.11.2)

# Main files for static library
add_library(LibXec STATIC
    include/XecHeader.hpp
    src/XecHeader.cpp
)

# Macros' main
add_executable(readrun macros/ReadRun.cpp)
add_executable(cexmappatch macros/xec/CexMapPatch.cpp)

# Collega gli eseguibili alla libreria statica e a CERN ROOT
target_link_libraries(readrun LibXec ${ROOT_LIBRARIES})
target_link_libraries(cexmappatch LibXec ${ROOT_LIBRARIES})

# Aggiungi le directory di inclusione di CERN ROOT
target_include_directories(readrun PRIVATE ${ROOT_INCLUDE_DIRS})
target_include_directories(cexmappatch PRIVATE ${ROOT_INCLUDE_DIRS})