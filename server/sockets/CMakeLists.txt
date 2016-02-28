project(sockets)
cmake_minimum_required(VERSION 2.8)

aux_source_directory(. ${PROJECT_NAME}_SRC_LIST)
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC_LIST})

