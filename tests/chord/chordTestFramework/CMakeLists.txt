cmake_minimum_required (VERSION 2.8.3)
PROJECT(compilationWithCMake)

#Get c++11
ADD_DEFINITIONS(-std=c++11)

find_package(Protobuf REQUIRED)

#SET(TOP_LEVEL_HEADER_DIRECTORIES /home/hewhosurvives/c++/libraries/cppzmq-master ../messages)

#set path to library
link_directories(/usr/lib/x86_64-linux-gnu)

#Generate the C++ for the messages from the libprotobuf markup
add_subdirectory(./messages)

#Tell the subprojects where to find the created static library and headers
include_directories(./messages ./src/ /home/hewhosurvives/c++/libraries/cppzmq-master/ )

link_directories(./messages)

#Tell cmake were to find the sub-projects
add_subdirectory(./src)

