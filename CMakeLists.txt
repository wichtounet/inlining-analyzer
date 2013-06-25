cmake_minimum_required(VERSION 2.6)

project("ATLAS Inlining")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -pedantic -Wall -Wextra -Wno-long-long")

#Configure output paths
set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH lib)

#Folder for the header files
include_directories(include)

#Create the executables

add_executable(analyzer 
	src/main.cpp 
	src/Inlining.cpp 
	src/Timer.cpp 
	src/Analyzer.cpp 
	src/CallGraph.cpp 
	src/Infos.cpp 
	src/Reports.cpp
	src/GraphReader.cpp
	src/Parameters.cpp
	src/Utils.cpp )

add_executable(bench 
	src/bench.cpp 
	src/Timer.cpp 
	src/Analyzer.cpp 
	src/CallGraph.cpp 
	src/Infos.cpp 
	src/InfosOld.cpp 
	src/Reports.cpp
	src/Parameters.cpp
	src/Utils.cpp)

add_executable(appgenerator 
	src/appgenerator.cpp 
	src/Timer.cpp 
	src/CallGraph.cpp 
	src/Infos.cpp 
	src/GraphReader.cpp
	src/Parameters.cpp
	src/Utils.cpp )

add_executable(functions 
	src/function-benchmark.cpp 
	src/functions.cpp 
	src/Timer.cpp)

add_executable(generator src/generator.cpp)

#Create the libraries

add_library(inlining SHARED
	src/Inlining.cpp 
	src/Timer.cpp 
	src/Analyzer.cpp 
	src/CallGraph.cpp 
	src/Infos.cpp 
	src/Reports.cpp
	src/GraphReader.cpp
	src/Parameters.cpp
	src/Utils.cpp)

add_library(inlining_static STATIC
	src/Inlining.cpp 
	src/Timer.cpp 
	src/Analyzer.cpp 
	src/CallGraph.cpp 
	src/Infos.cpp 
	src/Reports.cpp
	src/GraphReader.cpp
	src/Parameters.cpp
	src/Utils.cpp)

add_library(bench_functions SHARED src/function-library.cpp)

#Link targets to the libraries

target_link_libraries (analyzer boost_graph) 
target_link_libraries (analyzer elf) 

target_link_libraries (appgenerator boost_graph) 
target_link_libraries (appgenerator elf) 

target_link_libraries (inlining boost_graph) 
target_link_libraries (inlining elf) 

target_link_libraries (bench boost_graph) 
target_link_libraries (bench elf) 

target_link_libraries (functions bench_functions) 

#Install the analyzer on the computer

install (TARGETS analyzer DESTINATION bin)
install (TARGETS inlining DESTINATION lib)
install (TARGETS inlining_static DESTINATION lib)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.config ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)
