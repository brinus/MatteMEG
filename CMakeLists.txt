project(MatteMEG CXX)
cmake_minimum_required(VERSION 3.9)
#if(POLICY CMP0074)
#    cmake_policy(SET CMP0074 OLD)
#endif()

# Imposta lo standard del linguaggio C++ su C++17 e richiedi l'uso di tale standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trova e include CERN ROOT
find_package(ROOT CONFIG REQUIRED)

# Includi le directory di inclusione di ROOT e quelle personalizzate
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(include)

# Definisci la directory di output per le librerie condivise
set(LIBRARY_OUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Crea la libreria condivisa MMUtils utilizzando il file di implementazione MMUtils.C
add_library(MMUtils SHARED src/MMUtils.C)

# Definisci il target personalizzato "utils" che dipende dalla libreria MMUtils
add_custom_target(utils DEPENDS MMUtils)

