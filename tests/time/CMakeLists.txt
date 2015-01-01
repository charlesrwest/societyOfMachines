cmake_minimum_required (VERSION 2.8.3)
PROJECT(test)

#Get c++11
ADD_DEFINITIONS(
    -std=c++11
)

#set path to library
link_directories(/usr/lib/x86_64-linux-gnu)

link_libraries(pthread)

FILE(GLOB SOURCEFILES *.cpp)

message( ${SOURCEFILES} )




#Add the compilation target
ADD_EXECUTABLE(test ${SOURCEFILES})

#link libraries to executable
target_link_libraries(test)
